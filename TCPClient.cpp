#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "CSmessage.h"
#include <SFML/Graphics.hpp>

using namespace std;

enum AppState {
    TOP_MENU,
    MAIN_MENU,
    HOME_MENU,
    EXIT_APP
};

const char * ip = "100.65.30.221";
//const char * ip = "100.65.80.249";*
bool isAccess = true;
bool isLoggedIn = false;
string currentHome = "";
void homeMenu(int socket);  
Room lastReceivedRoom;

void gameRoomMenu();
string currentUsername;
			   
void createLoginMessage(CSmessage & request, string username, string password) {
    request.setType("LGIN");
    request.addParam("Username", username);
    request.addParam("Password", password);
}

void createLogoutMessage(CSmessage &request) {
    request.setType("LOUT");
}

void createListHomesMessage(CSmessage &request) {
    request.setType("LISTH");
}
void createAccessHomeMessage(CSmessage &request, string homeName) {
    request.setType("AHO");
    request.addParam("HomeName", homeName);
}

//Unlock after testing
/*
void createGetRoomMessage(CSmessage & request,int roomId) {
    request.setType("GRO");
    request.addParam("RoomId", to_string(roomId));
}
*/
void createGetRoomMessage(CSmessage & request,string roomId) {
    request.setType("GRO");
    request.addParam("RoomId", roomId);
}
void createSetRoomMessage(CSmessage & request, const Room& room) {
    request.setType("SRO");
    request.addParam("Room", request.serializeRoom(room).dump());
}
void createGetLockMessage(CSmessage &request,int lockId) {
    request.setType("GLO");
    request.addParam("LockId", to_string(lockId));
}

void createSetLockMessage(CSmessage &request, int lockId, bool open, int pin) {
    request.setType("SLO");
    request.addParam("LockId", to_string(lockId));
    request.addParam("Open", open ? "1" : "0");
    request.addParam("Pin", to_string(pin));
}

void createGetAlarmMessage(CSmessage &request,int alarmId) {
    request.setType("GAL");
    request.addParam("AlarmId", to_string(alarmId));
}

void createSetAlarmMessage(CSmessage &request,int alarmId, bool on, int pin) {
    request.setType("SAL");
    request.addParam("AlarmId", to_string(alarmId));
    request.addParam("On", on ? "1" : "0");
    request.addParam("Pin", to_string(pin));
}
void processLoginResponse(CSmessage &responseR) {
    string response = responseR.getParam("Response");

    if (response == "Ok") {
	isLoggedIn = true;
        cout << " Login Successful!" << endl;
    } else if (response == "Login Failure") {
	isLoggedIn = false;
        cout << " Login Failed: Incorrect username or password." << endl;
    } else if (response == "Invalid Message") {
        cout << " Invalid Login Message Format." << endl;
    }
}

void processGetListHomesResponse(CSmessage &responseR) {
    string jsonString = responseR.getParam("json");

    try {
        json listData = json::parse(jsonString);

        if (listData["Response"] == "Ok") {
            int numHomes = listData["Number"];
            cout << "Number of Homes: " << numHomes << endl;

            for (const auto& home : listData["List"]) {
                cout << "- " << home.get<string>() << endl;
            }
        } else {
            cout << "Failed to retrieve home list." << endl;
        }
    } catch (const json::parse_error& e) {
        cout << "Error parsing JSON: " << e.what() << endl;
    }
}

void processAccessHomeResponse(CSmessage & responseR)
{
     string jsonString = responseR.getParam("json");
     cout  << jsonString << endl;
     isAccess = true;
     try {
        json listData = json::parse(jsonString);

        if (listData["Response"] == "Ok") {
            json structure = listData["Structure"];
            cout << "Alarms: " << endl;
	    for (const auto& alarmId : structure["Alarms"]) {
                cout << "- " << alarmId.get<int>() << endl;  // Print each alarm ID inline
            } 
	    cout << "Locks: " << endl;
	    for (const auto& lockId : structure["Locks"]) {
                cout << "- " << lockId.get<int>() << endl;  // Print each alarm ID inline
            }		
	    cout << "Rooms: " << endl;
            for (const auto& roomId : structure["Rooms"]) {
                cout << "- " << roomId.get<string>() << endl;  // Print each alarm ID inline
            }
            
        } else {
	    isAccess = false;
            cout << "Failed to retrieve home structure." << endl;
        }
    } catch (const json::parse_error& e) {
	isAccess = false;
        //cout << "Error parsing JSON: " << e.what() << endl;
    }
}

void processGetRoomResponse(CSmessage &responseR) {
    string response = responseR.getParam("Response");
    if (response == "Ok") {
        cout << "plz" << endl;
        string k = responseR.getParam("Room");
        //cout << k << endl;
        lastReceivedRoom = responseR.deserializeRoom(k);
        //cout << "Room Data Received: " << lastReceivedRoom.getRoomId() << endl;
	cout << "Room Data Received:" << endl;
	cout << "Room ID: " << lastReceivedRoom.getRoomId() << endl;
	cout << "Window Blinds: " << (lastReceivedRoom.getWindowBlinds() ? "Closed" : "Open") << endl;

	// Retrieve and print all LightGroups in the room
	vector<LightGroup> lightGroups = lastReceivedRoom.GetAllLightGroups();
	cout << "Light Groups: " << endl;
	for (const LightGroup& lg : lightGroups) {
    		cout << "  - Light Group ID: " << lg.getId() << endl;
    		cout << "    Status: " << (lg.isOn() ? "On" : "Off") << endl;
    		cout << "    Color: " << lg.getColor() << endl;
	}
	cout << "[Debug] Client received Room with " << lastReceivedRoom.GetAllRobots().size() << " robots." << endl;
	// Retrieve and print all Robots in the room
	const unordered_map<int, Robot>& robots = lastReceivedRoom.GetAllRobots();
	
	cout << "Robots: " << endl;
	for (const auto& [robotId, robot] : robots) {
    		cout << "  - Robot ID: " << robotId << endl;
    		cout << "    Position: (" << robot.getX() << ", " << robot.getY() << ")" << endl;
    		cout << "    Controlled By: " << (robot.getControlledBy().empty() ? "None" : robot.getControlledBy()) << endl;
	}

	gameRoomMenu();
    } else {
        cout << "Room Not Found or Invalid Request." << endl;
    }
}

void processGetAlarmResponse(CSmessage &responseR) {
    string response = responseR.getParam("Response");

    if (response == "Ok") {
        string alarmStatus = responseR.getParam("Alarm");
        cout << " Alarm Status: " << alarmStatus << endl;
    } else {
        cout << " Alarm Not Found or Invalid Request." << endl;
    }
}

void processGetLockResponse(CSmessage &responseR) {
    string response = responseR.getParam("Response");

    if (response == "Ok") {
        string lockStatus = responseR.getParam("Lock");
        cout << " Lock Status: " << lockStatus << endl;
    } else {
        cout << " Lock Not Found or Invalid Request." << endl;
    }
}
// Send request & receive response
void sendAndReceive(int socket, CSmessage &request) {
    string requestData = request.marshal();
    send(socket, requestData.c_str(), requestData.size(), 0);

    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));
    int bytesReceived = recv(socket, buffer, sizeof(buffer) - 1, 0);
    
    if (bytesReceived > 0) {
        CSmessage response;
        response.unmarshal(buffer);
        
        string type = response.getType();
        if (type == "MESS") {
            processLoginResponse(response);
        } else if (type == "LRES") {
            processGetListHomesResponse(response);
        } else if (type == "AHRES") {
            processAccessHomeResponse(response);
        } else if (type == "GRRES") {
	    cout<< "here" << endl;
            processGetRoomResponse(response);

        } else if (type == "GARES") {
            processGetAlarmResponse(response);
        } else if (type == "GLRES") {
            processGetLockResponse(response);
        } else if (type == "SRRES"){
	     cout << response.getParam("Response");
	    /*	
	    string k = response.getParam("Room");
	    Room p = response.deserializeRoom(k);
	    cout << p.getRoomId() << endl;
	    cout << p.getWindowBlinds() << endl;*/
	}else if (type == "SARES"){

	    cout << response.getParam("Response");
	    /*	
            string k = response.getParam("Room");
            Room p = response.deserializeRoom(k);
            cout << p.getRoomId() << endl;
            cout << p.getWindowBlinds() << endl;*/
        }else if (type == "SLRES"){
	    cout << response.getParam("Response");	
	     /*
            string k = response.getParam("Room");
            Room p = response.deserializeRoom(k);
            cout << p.getRoomId() << endl;
            cout << p.getWindowBlinds() << endl;*/
        }else {
            cout << "Unknown response received: " << buffer << endl;
        }
    } else {
        cerr << "Connection lost.\n";
    }
}

void hello_udp(struct sockaddr_in &serverUDPAddr, int udpSocket)
{
    json helloMessage;
    helloMessage["Type"] = "HELLO";
    helloMessage["username"] = currentUsername;
    helloMessage["room"] = lastReceivedRoom.getRoomId();

    string helloData = helloMessage.dump();

    sendto(udpSocket, helloData.c_str(), helloData.size(), 0, (struct sockaddr*)&serverUDPAddr, sizeof(serverUDPAddr));

}

void robot_control(struct sockaddr_in &serverUDPAddr, int udpSocket)
{
 	int robotId;
        cout << "Enter Robot ID to control: ";
        cin >> robotId;

        cout << "Use W/A/S/D keys to move. Type Q to quit control mode.\n";
        int dx = 0, dy = 0;
        while (true) {
            char move;
            cin >> move;
            if (move == 'w') dy += -1;
            else if (move == 's') dy += 1;
            else if (move == 'a') dx += -1;
            else if (move == 'd') dx += 1;
            else if (move == 'q') break;
            else continue;

            json moveMessage;
            cout << currentUsername << endl;
            moveMessage["Type"] = "MRO";  // Move Robot Operation
            moveMessage["username"] = currentUsername; // You must save logged-in username!
            moveMessage["room"] = lastReceivedRoom.getRoomId(); // room you are in
            moveMessage["robotId"] = robotId;
            moveMessage["x"] = dx;
            moveMessage["y"] = dy;

            string outData = moveMessage.dump();
            cout << outData << endl;
            sendto(udpSocket, outData.c_str(), outData.size(), 0,
                   (struct sockaddr*)&serverUDPAddr, sizeof(serverUDPAddr));
        }

}

void robot_listening(struct sockaddr_in &serverUDPAddr, int udpSocket)
{
	cout << "Listening for robot updates (press 'q' to return to the previous menu, or any other key to continue listening)...\n";

    	char buffer[1024];
   	struct sockaddr_in fromAddr;
    	socklen_t fromLen = sizeof(fromAddr);

    	fd_set readfds;
    	struct timeval timeout;

    	while (true) {
        // Prepare file descriptor set
        	FD_ZERO(&readfds);
        	FD_SET(udpSocket, &readfds); // Monitor udpSocket for reading
        	FD_SET(STDIN_FILENO, &readfds); // Monitor standard input (keyboard) for reading

        	// Set timeout: 1 second timeout
        	timeout.tv_sec = 1;
        	timeout.tv_usec = 0;

        	// Call select() to monitor the file descriptors
        	int activity = select(udpSocket + 1, &readfds, NULL, NULL, &timeout);

        	if (activity > 0) {
            		if (FD_ISSET(udpSocket, &readfds)) {
                		// Data available to read from udpSocket
                		memset(buffer, 0, sizeof(buffer));
                		int bytes = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr*)&fromAddr, &fromLen);

               			 if (bytes > 0) {
                    			buffer[bytes] = '\0';
                    			try {
                        			json received = json::parse(buffer);

                        			if (received["Type"] == "MRRES") {
                            				cout << "Robot " << received["RobotId"]
                                 			<< " moved to (" << received["X"] << ", " 
                                 			<< received["Y"] << ") by " 
                                 			<< received["Username"] << endl;
                        			}
                    			} catch (...) {
                        			cerr << "Invalid message format.\n";
                    			}
                		}
            		}

            		// Check for user input
            		if (FD_ISSET(STDIN_FILENO, &readfds)) {
                		// User has entered a key
                		char input;
                		cin >> input;
                		if (input == 'q' || input == 'Q') {
                    			break;  // Exit the loop and return to the previous menu
                		}
            		}
        	} else if (activity == 0) {
            		// Timeout reached, no data or input
            		// You can add any periodic tasks you want to do here if needed
        	} else {
            		// Error in select()
            		cerr << "Error in select().\n";
            		break;
        	}
    	}

}

void gameRoomMenu() {
    cout << "\nEntered Game Room Mode!\n";

    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in serverUDPAddr{};
    serverUDPAddr.sin_family = AF_INET;
    serverUDPAddr.sin_port = htons(9090); // Same as your server UDP port
    serverUDPAddr.sin_addr.s_addr = inet_addr(ip); // Server IP
    
    
    hello_udp(serverUDPAddr, udpSocket);
    string choice;
    while(true)
    {
    	cout << "Choose:\n1. Control Robot\n2. View Robots\n3. Back\nChoice: ";
    	cin >> choice;

    	if (choice == "1") {
        	robot_control(serverUDPAddr, udpSocket);
    	}
    	else if (choice == "2") {
		robot_listening(serverUDPAddr, udpSocket);
    	}
	else if(choice == "3")
	{
		break;
	}
    }	
    close(udpSocket);
}


// **Top Menu (Login/Exit)**
void topMenu(int socket, AppState &state) {
    string choice;
    while (state == TOP_MENU) {
        cout << "\nTop Menu:\n1. Login\n99. Exit\nChoice: ";
        cin >> choice;

        if (choice == "1") {
            string username, password;
            cout << "Username: ";
            cin >> username;
            cout << "Password: ";
            cin >> password;
            
            CSmessage request;
            createLoginMessage(request,username, password);
	    currentUsername = username;
            sendAndReceive(socket,request);
	    state = MAIN_MENU;
	    return;
        } 
        else if (choice == "99") {
            cout << "Exiting...\n";
            close(socket);
            exit(0);
        } 
        else {
            cout << "Invalid choice.\n";
        }
    }
}


void mainMenu(int socket, AppState &state) {
    string choice;
    if (!isLoggedIn)
    {
	state = TOP_MENU;
	return;
    }
    while (isLoggedIn && state == MAIN_MENU) {
        cout << "\nMain Menu:\n1. List Homes\n2. Access Home\n3. Logout\n4. Exit\nChoice: ";
        cin >> choice;

        if (choice == "1") {
            CSmessage request;
            createListHomesMessage(request);
            sendAndReceive(socket, request);
        } 
        else if (choice == "2") {
            cout << "Enter Home Name: ";
            cin >> currentHome;  

            CSmessage request;
            createAccessHomeMessage(request, currentHome);
            sendAndReceive(socket, request);
	    if (isAccess)
	    {
 		state = HOME_MENU;
		return;
	    }
	    else
            {
		cout << "Invalid Home" << endl;
             }
        } 
        else if (choice == "3") {
            CSmessage request;
            createLogoutMessage(request);
            sendAndReceive(socket, request);
            isLoggedIn = false;
	    state = TOP_MENU;
            return; 
        } 
        else if (choice == "4") {
            cout << "Exiting...\n";
            close(socket);
            exit(0);
        } 
        else {
            cout << "Invalid choice.\n";
        }
    }
}


void homeMenu(int socket, AppState &state){
    string choice;
    while (true) {
        cout << "\nHome Options:\n1. Check Room\n2. Check Alarm\n3. Check Lock\n4. Set Room\n5. Set Alarm\n6. Set Lock\n7. Back\n8. Logout\n9. Exit\nChoice: ";
        cin >> choice;

        CSmessage request;
	state = HOME_MENU;
        if (choice == "1") {
            //int roomId;
	    string roomId;
            cout << "Enter Room ID: ";
            //cin >> roomId;
	    std::cin.ignore();
            getline(std::cin, roomId);
	    //getline(std::cin, roomId);
            createGetRoomMessage(request,roomId);
        } 
        else if (choice == "2") {
            int alarmId;
            cout << "Enter Alarm ID: ";
            cin >> alarmId;
            createGetAlarmMessage(request,alarmId);
        } 
        else if (choice == "3") {
            int lockId;
            cout << "Enter Lock ID: ";
            cin >> lockId;
            createGetLockMessage(request,lockId);
        } 
        else if (choice == "4") {  
             //int roomId;
	     string roomId;
             cout << "Enter Room ID: ";
             //cin >> roomId;
             std::cin.ignore();
             getline(std::cin, roomId);
             CSmessage getRequest;
             createGetRoomMessage(getRequest,roomId);
             sendAndReceive(socket, getRequest);  

             if (lastReceivedRoom.getRoomId() != roomId) {
                  cout << "Error: Room retrieval failed. Try again." << endl;
                  return;
             }
	     bool newBlinds;
             cout << "Current Window Blinds State: " << (lastReceivedRoom.getWindowBlinds() ? "Closed" : "Open") << endl;
             cout << "Set Window Blinds? (1 for Closed, 0 for Open): ";
             cin >> newBlinds;
             lastReceivedRoom.SetWindowBlinds(newBlinds);
             for (LightGroup& light : lastReceivedRoom.GetAllLightGroups2()) {
        	  string color;
        	  bool on;
		  int id = light.getId();

        	  cout << "\nModify Light Group (ID: " << id << ")" << endl;
        	  cout << "Current Color: " << light.getColor() << endl;
        	  cout << "Enter New Light Color: ";
        	  cin >> color;
        	  cout << "Current State: " << (light.isOn() ? "On" : "Off") << endl;
        	  cout << "Turn Lights On? (1 for Yes, 0 for No): ";
        	  cin >> on;
        	  light.SetColor(color);
                  light.SetOn(on);
    	      }
	      request.reset();
              createSetRoomMessage(request, lastReceivedRoom);
              for (LightGroup& light : lastReceivedRoom.GetAllLightGroups()) {
                  string color;
                  bool on;

                  cout << "\nModify Light Group (ID: " << light.getId() << ")" << endl;
                  cout << "Current Color: " << light.getColor() << endl;
                  cout << "Current State: " << (light.isOn() ? "On" : "Off") << endl;
              }
              sendAndReceive(socket, request);
              
	}

        else if (choice == "5") {
            int alarmId, pin;
            bool on;
            cout << "Enter Alarm ID: ";
            cin >> alarmId;
            cout << "Enter PIN: ";
            cin >> pin;
            cout << "Activate Alarm? (1 for Yes, 0 for No): ";
            cin >> on;
            createSetAlarmMessage(request,alarmId, on, pin);
        } 
        else if (choice == "6") {
            int lockId, pin;
            bool open;
            cout << "Enter Lock ID: ";
            cin >> lockId;
            cout << "Enter PIN: ";
            cin >> pin;
            cout << "Open Lock? (1 for Yes, 0 for No): ";
            cin >> open;
            createSetLockMessage(request,lockId, open, pin);
        } 
	else if(choice == "7")
	{
		state = MAIN_MENU;
		return;
	}
        else if (choice == "8") {
            CSmessage request;
            createLogoutMessage(request);
            sendAndReceive(socket, request);
            state = TOP_MENU;
	    isLoggedIn = false;
            return;
        } 
        else if (choice == "9") {
            cout << "Exiting...\n";
            close(socket);
            exit(0);
        } 
        else {
            cout << "Invalid choice.\n";
            continue;
        }
        
	
        sendAndReceive(socket, request);

    }
}


void interactiveClient(int socket) {
    AppState state = TOP_MENU;  

    while (state != EXIT_APP){
        switch (state) {
            case TOP_MENU:
                topMenu(socket, state); 
                break;
            case MAIN_MENU:
                mainMenu(socket, state);
                break;
            case HOME_MENU:
                homeMenu(socket, state);
                break;
            case EXIT_APP:
                cout << "Exiting...\n";
                close(socket);
                exit(0);
                break;
        }
    }
}


// **Connect to Server**
int main() {
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        cerr << "Connection failed!\n";
        return 1;
    }

    cout << "Connected to the server. Type 'exit' to quit.\n";
    interactiveClient(clientSocket);

    close(clientSocket);
    return 0;
}


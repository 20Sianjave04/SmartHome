#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <cstring>
#include <thread>
#include <mutex>
#include "CSmessage.h"
#include "CSHome.h"
#include <unordered_set>
#include <chrono>
#include <nlohmann/json.hpp>
#include <set>

using namespace std;

std::mutex sessionMutex;
std::mutex homeMutex;
std::mutex userMutex;
std::mutex udpMutex;

// User database
// --- Homes (shared) ---
// --- Homes (shared) ---
Home smartHome("SmartHome",
    {
        Room("101", {LightGroup(1, "White"), LightGroup(2, "Blue")}, {Robot(1), Robot(2)}),
        Room("102", {LightGroup(3, "Red"), LightGroup(4, "Green")}, {Robot(3)})
    },
    {Alarm(1, 1234)},
    {Lock(1, {1111, 2222, 3333})}
);

Home vacationVilla("VacationVilla",
    {
        Room("201", {LightGroup(5, "Yellow"), LightGroup(6, "Purple")}, {Robot(4)}),
        Room("202", {LightGroup(7, "Cyan"), LightGroup(8, "Orange")}, {Robot(5), Robot(6)})
    },
    {Alarm(2, 5678)},
    {Lock(2, {4444, 5555, 6666})}
);

Home adminHouse("AdminHouse",
    {
        Room("301", {LightGroup(9, "Pink"), LightGroup(10, "Brown")}, {Robot(7)}),
        Room("302", {LightGroup(11, "Gray"), LightGroup(12, "Black")}, {Robot(8), Robot(9)})
    },
    {Alarm(3, 4321)},
    {Lock(3, {7777, 8888, 9999})}
);

Home penthouse("Penthouse",
    {
        Room("401", {LightGroup(13, "Magenta"), LightGroup(14, "Teal")}, {Robot(10)}),
        Room("402", {LightGroup(15, "Gold"), LightGroup(16, "Silver")}, {Robot(11), Robot(12)})
    },
    {Alarm(4, 8765)},
    {Lock(4, {1010, 2020, 3030})}
);

map<string, User> users = {
    {"user1", {"user1", "pass1", {&smartHome, &vacationVilla}}},
    {"admin", {"admin", "adminpass", {&adminHouse, &penthouse}}},
    {"Nigel", {"Nigel", "John", {&adminHouse, &penthouse}}},
    {"Jane", {"Jane", "janepass", {&smartHome, &penthouse}}},
    {"Bob", {"Bob", "bobpass", {&vacationVilla, &adminHouse}}}
};


unordered_map<int, string> clientCurrentHome;
// Track logged-in users
map<int, string> activeSessions;

unordered_map<string, string> userToRoom;
unordered_map<string, sockaddr_in> userUdpAddr;
unordered_map<string, set<string>>roomToUsers;
unordered_map<string, set<string>>dynamicRooms;

void createLoginResponse(CSmessage &request, CSmessage &response, int clientSocket) {

    string username = request.getParam("Username");
    string password = request.getParam("Password");
    response.setType("MESS");
    if (users.count(username) && users[username].CheckPassword(password)) {
	    std::lock_guard<std::mutex> lock(sessionMutex);
            activeSessions[clientSocket] = username;
            response.addParam("Response", "Ok");
    } else {
            response.addParam("Response", "Login Failure");
    }      
}

void createLogoutResponse(CSmessage &response) {
    response.setType("MESS");
    response.addParam("Response", "Ok");
}

void createListHomesResponse(CSmessage &responseR, int clientSocket)
{
    string user = activeSessions[clientSocket];
    responseR.setType("LRES");
    if (users.count(user)) {
            vector<Home*> userHomes = users[user].GetHomes(); // Fetch the user's homes
            vector<string> homeNames;
            for (const auto &home : userHomes) {
                homeNames.push_back(home->GetName());
            }
	    json response;
            response["Response"] = "Ok";
            response["Number"] = userHomes.size();
            response["List"] = homeNames;
            responseR.addParam("json", response.dump());
     }else{
          responseR.addParam("Response", "Invalid user");
     }
}

void createAccessHomeResponse(CSmessage &request, CSmessage &responseR, int clientSocket) {
    string user = activeSessions[clientSocket];  
    string homeName = request.getParam("HomeName");  

    responseR.setType("AHRES");

    if (users.count(user)) {  
        Home *home = users[user].GetHome(homeName); 

        if (home) {  
	    clientCurrentHome[clientSocket] = homeName;
            json response;
            response["Response"] = "Ok";
            json structure;
            structure["Rooms"] = home->GetRooms().size();
            structure["Locks"] = home->GetLocks().size();
            structure["Alarms"] = home->GetAlarms().size();

            //vector<int> roomIds;
	    vector<string> roomIds;
            for (const auto &room : home->GetRooms()) {
                roomIds.push_back(room.getRoomId());
            }
            structure["Rooms"] = roomIds;

            vector<int> lockIds;
            for (const auto &lock : home->GetLocks()) {
                lockIds.push_back(lock.GetLockId());
            }
            structure["Locks"] = lockIds;

            vector<int> alarmIds;
            for (const auto &alarm : home->GetAlarms()) {
                alarmIds.push_back(alarm.GetAlarmId());
            }
            structure["Alarms"] = alarmIds;

            response["Structure"] = structure;
            responseR.addParam("json", response.dump());
        } else {
            responseR.addParam("Response", "Home Name not found in user's homes");
        }
    } else {
        responseR.addParam("Response", "Invalid user");
    }
}

void createGetRoomResponse(CSmessage &request, CSmessage &response, int clientSocket) {
    string roomIdStr = request.getParam("RoomId");
    
    if (roomIdStr.empty()) {
        response.setType("GRRES");
        response.addParam("Response", "Invalid Request: Missing RoomId");
        return;
    }

    //int roomId = stoi(roomIdStr);
    string roomId = roomIdStr;
    cout << roomId << endl;
    string user = activeSessions[clientSocket];

   
    if (clientCurrentHome.count(clientSocket) == 0) {
        response.setType("GRRES");
        response.addParam("Response", "No Home Selected");
        return;
    }

    string homeName = clientCurrentHome[clientSocket];

    if (users.count(user) == 0) {
        response.setType("GRRES");
        response.addParam("Response", "Invalid User");
        return;
    }

    Home *home = users[user].GetHome(homeName);
    if (!home) {
        response.setType("GRRES");
        response.addParam("Response", "Invalid Home");
        return;
    }

    Room *room = home->GetRoom(roomId);
    if (room) {
	cout << "[Debug] Server sending Room with " << room->GetAllRobots().size() << " robots." << endl;
        response.setType("GRRES");
        response.addParam("Response", "Ok");
        response.addParam("Room", response.serializeRoom(*room).dump());
	cout << "hello" << endl;
	string roomKey = homeName + "_" + roomId;
	cout << "[Debug] roomKey to insert: " << roomKey << endl;  

	{
    		std::lock_guard<std::mutex> lock(udpMutex);
    		dynamicRooms[roomKey].insert(user);
    		userToRoom[user] = roomKey;
		roomToUsers[roomKey].insert(user);
    		cout << "[Debug] Inserted user into dynamicRooms" << endl; 
	        for (const auto& usr : dynamicRooms[roomKey]) {
                	cout << usr << " ";
                }

		cout << endl;	
		for (const auto& us : roomToUsers[roomKey]) {
                        cout << us << " ";
                }
	}
    } else {
        response.setType("GRRES");
        response.addParam("Response", "Invalid Room");
    }
}

void createGetAlarmResponse(CSmessage &request, CSmessage &response, int clientSocket) {
    int alarmId = stoi(request.getParam("AlarmId"));
    string user = activeSessions[clientSocket];
    string homeName = clientCurrentHome[clientSocket];

    response.setType("GARES");

   
    if (users.count(user)) {
        Home *home = users[user].GetHome(homeName);

        if (home) {
            Alarm *alarm = home->GetAlarm(alarmId);
            if (alarm) {
                response.addParam("Response", "Ok");
                response.addParam("Alarm", alarm->isOn() ? "Armed" : "Disarmed");
                return;
            }
        }
    }

    response.addParam("Response", "Invalid Alarm");
}

void createGetLockResponse(CSmessage &request, CSmessage &response, int clientSocket) {
    int lockId = stoi(request.getParam("LockId"));
    string user = activeSessions[clientSocket];
    string homeName = clientCurrentHome[clientSocket];

    response.setType("GLRES");

    if (users.count(user)) {
        Home *home = users[user].GetHome(homeName);

        if (home) {
            Lock *lock = home->GetLock(lockId);
            if (lock) {
                response.addParam("Response", "Ok");
                response.addParam("Lock", lock->isOpen() ? "Unlocked" : "Locked");
                return;
            }
        }
    }

    response.addParam("Response", "Invalid Lock");
}


void SetRoomResponse(CSmessage &request, CSmessage &response, int clientSocket) {
    string user = activeSessions[clientSocket];  
    string homeName = clientCurrentHome[clientSocket]; 
    cout << "uu" << endl;
    cout << homeName << endl;
    if (users.count(user)) { 
        Home* home = users[user].GetHome(homeName);  

        if (home) {
            Room roomInput = request.extractRoom();
            Room* room = home->GetRoom(roomInput.getRoomId());

            if (room) {
                room->SetWindowBlinds(roomInput.getWindowBlinds());

                for (const LightGroup &newLight : roomInput.GetAllLightGroups()) {
		    cout << newLight.getColor() << endl;
                    room->SetLightGroup(newLight.getId(), newLight.isOn(), newLight.getColor());
                }

                response.setType("SRRES");
                response.addParam("Response", "Ok");
                response.addParam("Room", response.serializeRoom(*room).dump());
            } else {
                response.setType("SRRES");
                response.addParam("Response", "Room Not Found");
            }
        } else {
            response.setType("SRRES");
            response.addParam("Response", "Home Not Found");
        }
    } else {
        response.setType("SRRES");
        response.addParam("Response", "Invalid User");
    }
}

void SetAlarmResponse(CSmessage &request, CSmessage & response, int clientSocket) {
    string user = activeSessions[clientSocket];
    string homeName = clientCurrentHome[clientSocket];
    cout << homeName << endl;
    if (users.count(user)) {
        Home* home = users[user].GetHome(homeName);
	if (home) {
		int alarmId = stoi(request.getParam("AlarmId"));
		bool on = request.getParam("On") == "1";
        	int pin = stoi(request.getParam("Pin"));
		Alarm* alarm = home->GetAlarm(alarmId);
   	 	if (alarm) {
        		bool success = alarm->SetAlarm(on, pin);
        		response.setType("SARES");
        		if (success) {
            			response.addParam("Response", "Ok");
            			response.addParam("Alarm", on ? "Armed" : "Disarmed");
        		} else {
            			response.addParam("Response", "Invalid PIN");
            			response.addParam("Alarm", "Failed");
        		}
	 	}else {
            		response.setType("SARES");
            		response.addParam("Response", "Alarm Not Found");
        	}
	}
	else {
            response.setType("SARES");
            response.addParam("Response", "Home Not Found");
        }
    } else {
        response.setType("SARES");
        response.addParam("Response", "Not Found");
    }
}

void SetLockResponse(CSmessage &request, CSmessage &response, int clientSocket) {
    //std::lock_guard<std::mutex> lock(sessionMutex);
    string user = activeSessions[clientSocket];
    string homeName = clientCurrentHome[clientSocket];
    cout << homeName << endl;
    
    if (users.count(user)) {
        Home* home = users[user].GetHome(homeName);
        if (home) {
		int lockId = stoi(request.getParam("LockId"));
        	bool open = request.getParam("Open") == "1";
        	int pin = stoi(request.getParam("Pin"));
		Lock* lock = home->GetLock(lockId);
    		if (lock) {
        		bool success = lock->SetLock(open, pin);
        		response.setType("SLRES");
        		if (success) {
            			response.addParam("Response", "Ok");
            			response.addParam("Lock", open ? "Unlocked": "Locked");
        		} else {
            			response.addParam("Response", "Invalid PIN");
            			response.addParam("Lock", "Failed");
        		}
		}else {
           	 	response.setType("SLRES");
            		response.addParam("Response", "Lock Not Found");
        	}
	}else {
            response.setType("SLRES");
            response.addParam("Response", "Home Not Found");
        }
    } else {
        response.setType("SLRES");
        response.addParam("Response", "Not Found");
    }
}

// Send response back to client
void sendResponse(int clientSocket, CSmessage &response) {
    string marshalled = response.marshal();
    send(clientSocket, marshalled.c_str(), marshalled.size(), 0);
}

// Process received message
void processMessage(int clientSocket, CSmessage &request) {
    CSmessage response;
    string type = request.getType();

    if (type == "LGIN") {
        createLoginResponse(request, response, clientSocket);
    } 
    else if (type == "LOUT") {
	{
	    std::lock_guard<std::mutex> lock(sessionMutex);
        	if (activeSessions.count(clientSocket)) {
            		activeSessions.erase(clientSocket);
            	createLogoutResponse(response);
        	}
	}
    } 
    else if (type == "LISTH") {
        createListHomesResponse(response, clientSocket);
    } 
    else if (type == "AHO") {
        createAccessHomeResponse(request, response, clientSocket);
    }
    
    else if (type == "GRO") {
	createGetRoomResponse(request, response, clientSocket);
    }
   
    else if (type == "GAL") {
        createGetAlarmResponse(request, response, clientSocket);
    }
    else if (type == "GLO"){
   	createGetLockResponse(request, response, clientSocket);
    } 
 
    else if (type == "SRO") {
	cout << "ohho" << endl;
	SetRoomResponse(request, response, clientSocket);
    }
    else if (type == "SAL") {
        SetAlarmResponse(request, response, clientSocket);    
    } 
    else if (type == "SLO") {
        SetLockResponse(request, response, clientSocket);
          
    }

    sendResponse(clientSocket, response);
}


void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            cout << "Client disconnected.\n";
            break;
        }

        buffer[bytesReceived] = '\0';
        CSmessage request;
        request.unmarshal(buffer);
        processMessage(clientSocket, request);
    }

    close(clientSocket);
}

void handleUDP() {
    int udpSocket = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in udpAddr{}, clientAddr{};
    socklen_t addrLen = sizeof(clientAddr);

    udpAddr.sin_family = AF_INET;
    udpAddr.sin_port = htons(9090);  // UDP Server on port 9090
    udpAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(udpSocket, (struct sockaddr*)&udpAddr, sizeof(udpAddr)) < 0) {
        cerr << "Failed to bind UDP socket.\n";
        return;
    }

    cout << "UDP server running on port 9090...\n";

    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytes = recvfrom(udpSocket, buffer, sizeof(buffer) - 1, 0,
                             (struct sockaddr*)&clientAddr, &addrLen);

        if (bytes > 0) {
            buffer[bytes] = '\0';

	    

            try {
                json robotUpdate = json::parse(buffer);
		string type = robotUpdate["Type"];
	        cout << type << endl;
	        cout << type << endl;	
                string username = robotUpdate["username"];
		if (robotUpdate["Type"] == "HELLO") {
    			string username = robotUpdate["username"];
			{
        			// Protect the shared map with a mutex
        			std::lock_guard<std::mutex> lock(udpMutex);
        			// Save the user's UDP address
        		userUdpAddr[username] = clientAddr;
    			}
			cout << "[Debug] Registered UDP address for user: " << username << endl;
    			continue; // Finished processing this HELLO message
		}
		else
		{
			 if (!robotUpdate.contains("username") ||
                   		!robotUpdate.contains("robotId") ||
                   		!robotUpdate.contains("room") ||
                   		!robotUpdate.contains("x") ||
                   		!robotUpdate.contains("y"))
                	{
                        	cerr << "UDP Error: Missing fields in message.\n";
                        	continue;
                	}
                	int robotId = robotUpdate["robotId"];
                	int x = robotUpdate["x"];
                	int y = robotUpdate["y"];
                	cout << "I love Networks" << endl;
                	string roomKey;
                	{
                    		std::lock_guard<std::mutex> lock(udpMutex);

                    		if (userToRoom.find(username) == userToRoom.end()) {
                        		cerr << "UDP Error: User not registered in a room.\n";
                        		continue;
                    		}

                    		roomKey = userToRoom[username];

                    		if (userUdpAddr.find(username) == userUdpAddr.end()) {
                        		userUdpAddr[username] = clientAddr;
                    		}
                	}
                	cout << "let her go" << endl;
                	size_t underscorePos = roomKey.find('_');
                	if (underscorePos == string::npos) {
                    		cerr << "Invalid roomKey format.\n";
                    		continue;
                	}

                	string homeName = roomKey.substr(0, underscorePos);
                	string roomId = roomKey.substr(underscorePos + 1);

                	Home* home = nullptr;
                	Room* room = nullptr;
			cout << "So get away " << endl;

                	{	
                    		std::lock_guard<std::mutex> lock(homeMutex);
                    		cout << "awwwww" << endl;
                    		for (auto& h : users[username].GetHomes()) {
					cout << "[Debug] Comparing home name: '" << h->GetName() << "' with '" << homeName << "'" << endl;
                        		if (h->GetName() == homeName) {
                            			home = h;
                            			break;
                        		}
                    		}
                    		cout << "Because I miss u all the time" << endl;
		    		cout << home->GetName() << endl;
                    		if (home) {
                        		cout << roomId << endl;
		        		cout << roomId.length() << endl;	
                        		room = home->GetRoom(roomId);
					cout << room->getRoomId();
                    		}
		    		cout << "SOOO get away, another way too feel" << endl;
                	}
                	cout << "is it here" << endl;
                	if (!room) {
                    		cerr << "Could not find room for user.\n";
                    		continue;
                	}

                	// (Here you would update robot position in the Room structure if you had robot objects!)

                	{
                    		std::lock_guard<std::mutex> lock(udpMutex);
                    		cout << " uwuuuuu" << endl;
                    		for (const string& targetUser : dynamicRooms[roomKey]) {
                        		cout << " ala " << endl;
					if (targetUser == username) continue;
                        		cout << "el diablo " << endl;
					cout << targetUser << endl;
                        		if (userUdpAddr.find(targetUser) != userUdpAddr.end()) {
			    			cout << "just let it be " << endl;
                            			sockaddr_in targetAddr = userUdpAddr[targetUser];
                            			json responseMessage;
                            			responseMessage["Type"] = "MRRES";
                            			responseMessage["Username"] = username;
                            			responseMessage["RobotId"] = robotId;
                            			responseMessage["X"] = x;
                            			responseMessage["Y"] = y;

                            			string outData = responseMessage.dump();
			    			cout << outData << endl;
                            			sendto(udpSocket, outData.c_str(), outData.size(), 0,(struct sockaddr*)&targetAddr, sizeof(targetAddr));

                        		}
                    		}
                	}
		
		}
            } catch (...) {
                cerr << "Invalid UDP message received.\n";
            }
        }
    }

    close(udpSocket);
}



int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    cout << "Server running on port 8080...\n";
     

    //Add UDP if it doesnt work delet the folliwung2 lines
    std::thread udpThread(handleUDP);
    udpThread.detach();    
    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket >= 0) {
	    std::thread clientThread(handleClient, clientSocket);
            clientThread.detach();	
        }
    }

    close(serverSocket);
    return 0;
}


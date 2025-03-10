#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <vector>
#include <map>
#include <cstring>
#include "CSmessage.h"
#include "CSHome.h"

using namespace std;


// User database
map<string, User> users = {
    {"user1", {"user1", "pass1", {
        Home("SmartHome",
            {Room("101", {LightGroup(1, "White"), LightGroup(2, "Blue")}),
             Room("102", {LightGroup(3, "Red"), LightGroup(4, "Green")})},
            {Alarm(1, 1234)},
            {Lock(1, {1111, 2222, 3333})}),

        Home("VacationVilla",
            {Room("201", {LightGroup(5, "Yellow"), LightGroup(6, "Purple")}),
             Room("202", {LightGroup(7, "Cyan"), LightGroup(8, "Orange")})},
            {Alarm(2, 5678)},
            {Lock(2, {4444, 5555, 6666})})
    }}},

    {"admin", {"admin", "adminpass", {
        Home("AdminHouse",
            {Room("301", {LightGroup(9, "Pink"), LightGroup(10, "Brown")}),
             Room("302", {LightGroup(11, "Gray"), LightGroup(12, "Black")})},
            {Alarm(3, 4321)},
            {Lock(3, {7777, 8888, 9999})}),

        Home("Penthouse",
            {Room("401", {LightGroup(13, "Magenta"), LightGroup(14, "Teal")}),
             Room("402", {LightGroup(15, "Gold"), LightGroup(16, "Silver")})},
            {Alarm(4, 8765)},
            {Lock(4, {1010, 2020, 3030})})
    }}},

    {"Nigel", {"Nigel", "John", {
        Home("NigelHouse",
            {Room("Living Room", {LightGroup(9, "Pink"), LightGroup(10, "Brown"), LightGroup(11, "Ginger")}),
             Room("Kitchen", {LightGroup(11, "Gray")}),
	     Room("Bedroom", {LightGroup(11, "Gray"), LightGroup(12, "Black")})
	     },
            {Alarm(3, 4321)},
            {Lock(3, {7777, 8888, 9999}),Lock(8, {1234, 5678, 9111}), }),

        Home("NigelPenthouse",
            {Room("401", {LightGroup(13, "Magenta"), LightGroup(14, "Teal")}),
             Room("402", {LightGroup(15, "Gold"), LightGroup(16, "Silver")})},
            {Alarm(4, 8765)},
            {Lock(4, {1010, 2020, 3030})})
    }}},
       
};

unordered_map<int, string> clientCurrentHome;
// Track logged-in users
map<int, string> activeSessions;

void createLoginResponse(CSmessage &request, CSmessage &response, int clientSocket) {

    string username = request.getParam("Username");
    string password = request.getParam("Password");
    response.setType("MESS");
    if (users.count(username) && users[username].CheckPassword(password)) {
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
            vector<Home> userHomes = users[user].GetHomes(); // Fetch the user's homes
            vector<string> homeNames;
            for (const auto &home : userHomes) {
                homeNames.push_back(home.GetName());
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
        response.setType("GRRES");
        response.addParam("Response", "Ok");
        response.addParam("Room", response.serializeRoom(*room).dump());
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
        if (activeSessions.count(clientSocket)) {
            activeSessions.erase(clientSocket);
            createLogoutResponse(response);
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


int main() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, 5);
    cout << "Server running on port 8080...\n";

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket >= 0) {
            cout << "Client connected.\n";
            handleClient(clientSocket);
        }
    }

    close(serverSocket);
    return 0;
}


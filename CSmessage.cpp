/*
 * CSmessage.cpp
 *
 *  Created on: Feb 13, 2025
 *      Author: nigel
 */

#include "CSmessage.h"
#include "CSHome.h"
#include <sstream>
#include <iostream>
#include <set>
#include "/usr/include/nlohmann/json.hpp"

using json = nlohmann::json;
using namespace std;

const set<string> MCMDS({
    "LGIN", "LOUT", "LISTH", "AHO", "GRO", "GAL", "GLO",
    "SRO", "SAL", "SLO"
});

const set<string> RESPONSES({
    "MESS", "LRES", "AHRES", "GRRES", "GARES", "GLRES",
    "SRRES", "SARES", "SLRES"
});

const string VJOIN = "=";
const string PJOIN = "|";

CSmessage::CSmessage() {
    reset();
}


CSmessage::~CSmessage() {}


// Reset kinda initialize
void CSmessage::reset() {
    messageData.clear();
    setType("LGIN");
}

// Get the value of a parametetr
string CSmessage::getParam(string k) {
    
    return messageData.contains(k) ? messageData[k].get<string>() : "";
}

// Set a type of messgea
void CSmessage::setType(string t) {
    messageData["type"] = t;
}

// Get the message type
string CSmessage::getType() {
    return messageData["type"];
}

// Add a key-value parameter
void CSmessage::addParam(string k, string v) {
    messageData[k] = v;
}

// pretty straightforward marshlling
string CSmessage::marshal() {
    return messageData.dump();
}

// deserializin messafge unmarshalling
void CSmessage::unmarshal(string str) {
    
    try {
        messageData = json::parse(str);  
    } catch (const json::parse_error& e) {
        std::cerr << "JSON Parsing Error: " << e.what() << std::endl;
        messageData.clear();  
    }
    
}

// Serialize a Room object into a string
json CSmessage::serializeRoom(const Room& room) {
    
    json j;
    j["RoomId"] = room.getRoomId();
    j["WindowBlinds"] = room.getWindowBlinds();
    for (const auto& light : room.GetAllLightGroups()) {
	 
        j["LightGroups"].push_back({
            {"Id", light.getId()},
            {"Color", light.getColor()},
            {"On", light.isOn()}
        });
    }
    
    return j;
}

// Deserialize a Room object from a string
Room CSmessage::deserializeRoom(const string& roomStr) {
    cout << "joa" << endl;
    json roomJson = json::parse(roomStr); 
    //int roomId = roomJson["RoomId"];
    string roomId = roomJson["RoomId"];
    cout << "roomid" << endl;
    bool windowBlinds = roomJson["WindowBlinds"];
    vector<LightGroup> lights;
    
    for (auto& lightJson : roomJson["LightGroups"]) {
        lights.push_back(LightGroup(lightJson["Id"], lightJson["Color"]));
        lights.back().SetOn(lightJson["On"]);
    }

    Room room(roomId, lights);
    room.SetWindowBlinds(windowBlinds);
    return room;
}

/*

void CSmessage::createGetRoomMessage(int roomId) {
    setType("GRO");
    addParam("RoomId", to_string(roomId));
}


void CSmessage::createSetRoomMessage(const Room& room) {
    setType("SRO");
    addParam("Room", serializeRoom(room).dump());
}
*/
Room CSmessage::extractRoom() {
    cout << "typeshi" << endl;
    return deserializeRoom(messageData["Room"]);
}

/*
// Create a login message
void CSmessage::createLoginMessage(string username, string password) {
    setType("LGIN");
    addParam("Username", username);
    addParam("Password", password);
}
*/
/*
// Create a logout message
void CSmessage::createLogoutMessage() {
    setType("LOUT");
}

// Create a list homes request
void CSmessage::createListHomesMessage() {
    setType("LISTH");
}

// Create an access home message
void CSmessage::createAccessHomeMessage(string homeName) {
    setType("AHO");
    addParam("HomeName", homeName);
}

// Create a get lock message
void CSmessage::createGetLockMessage(int lockId) {
    setType("GLO");
    addParam("LockId", to_string(lockId));
}

// Create a set lock message
void CSmessage::createSetLockMessage(int lockId, bool open, int pin) {
    setType("SLO");
    addParam("LockId", to_string(lockId));
     addParam("Open", open ? "1" : "0");
    addParam("Pin", to_string(pin));
}

// Create a get alarm message
void CSmessage::createGetAlarmMessage(int alarmId) {
    setType("GAL");
    addParam("AlarmId", to_string(alarmId));
}

// Create a set alarm message
void CSmessage::createSetAlarmMessage(int alarmId, bool on, int pin) {
    setType("SAL");
    addParam("AlarmId", to_string(alarmId));
    addParam("On", on ? "1" : "0");
    addParam("Pin", to_string(pin));
}
*/
/*
void CSmessage::SetRoomResponse(Home& home, Room roomInput) {

  
    Room* room = home.GetRoom(roomInput.getRoomId());
    if (room) {

        room->SetWindowBlinds(roomInput.getWindowBlinds());

        for (const LightGroup& newLight : roomInput.GetAllLightGroups()) {
            room->SetLightGroup(newLight.getId(), newLight.isOn(), newLight.getColor());
        }

        setType("SRRES");
        addParam("Response", "Ok");
        addParam("Room", serializeRoom(*room).dump());
    } else {
        setType("SRRES");
        addParam("Response", "Not Found");
    }
}
*/
/*
void CSmessage:: SetAlarmResponse(Home& home, int alarmId, bool on, int pin) {

    Alarm* alarm = home.GetAlarm(alarmId);
    if (alarm) {
        bool success = alarm->SetAlarm(on, pin);
        setType("SARES");
        if (success) {
            addParam("Response", "Ok");
            addParam("Alarm", on ? "Armed" : "Disarmed");
        } else {
            addParam("Response", "Invalid PIN");
            addParam("Alarm", "Failed");
        }
    } else {
        setType("SARES");
        addParam("Response", "Not Found");
    }
}

void CSmessage::SetLockResponse(Home& home, int lockId, bool open, int pin) {

    Lock* lock = home.GetLock(lockId);
    if (lock) {
        bool success = lock->SetLock(open, pin);
        setType("SLRES");
        if (success) {
            addParam("Response", "Ok");
            addParam("Lock", open ? "Unlocked": "Locked");
        } else {
            addParam("Response", "Invalid PIN");
            addParam("Lock", "Failed");
        }
    } else {
        setType("SLRES");
        addParam("Response", "Not Found");
    }
}
*/
/*
void CSmessage::createLoginResponse(bool success) {
    setType("MESS");
    if (success) {
        addParam("Response", "Ok");
    } else {
        addParam("Response", "Login Failure");
    }
}*/
/*
void CSmessage::createLogoutResponse() {
    setType("MESS");
    addParam("Response", "Ok");
}
*/

/*
void CSmessage::createListHomesResponse(const vector<string>& homeList) {
    setType("LRES");

    json response;
    response["Response"] = "Ok";
    response["Number"] = homeList.size();
    response["List"] = homeList;  

    addParam("json", response.dump());
}
*/
/*
void CSmessage::createAccessHomeResponse(const Home& home) {
    setType("AHRES");

    json response;
    response["Response"] = "Ok";

    json structure;
    structure["Rooms"] = home.GetRooms().size();
    structure["Locks"] = home.GetLocks().size();
    structure["Alarms"] = home.GetAlarms().size();

    response["Structure"] = structure; 

    addParam("json", response.dump()); 
}
*/
/*
void CSmessage::createGetRoomResponse(const Room* room) {
    setType("GRRES");
    if (room) {
        addParam("Response", "Ok");
        addParam("Room", serializeRoom(*room).dump());
    } else {
        addParam("Response", "Invalid");
    }
}*/
/*
void CSmessage::createGetAlarmResponse(const Alarm* alarm) {
    setType("GARES");
    if (alarm) {
        addParam("Response", "Ok");
        addParam("Alarm", alarm->isOn() ? "Armed" : "Disarmed");
    } else {
        addParam("Response", "Invalid");
    }
}

void CSmessage::createGetLockResponse(const Lock* lock) {
    setType("GLRES");
    if (lock) {
        addParam("Response", "Ok");
        addParam("Lock", lock->isOpen() ? "Unlocked" : "Locked");
    } else {
        addParam("Response", "Invalid");
    }
}
*/


/*
void CSmessage::processLoginResponse() {
    string response = getParam("Response");

    if (response == "Ok") {
        cout << " Login Successful!" << endl;
    } else if (response == "Login Failure") {
        cout << " Login Failed: Incorrect username or password." << endl;
    } else if (response == "Invalid Message") {
        cout << " Invalid Login Message Format." << endl;
    }
}*/

void CSmessage::processLogoutResponse() {
    string response = getParam("Response");

    if (response == "Ok") {
        cout << " Logout Successful!" << endl;
    } else {
        cout << " Logout Failed: Unknown issue." << endl;
    }
}
/*
void CSmessage::processGetRoomResponse() {

    if (getParam("Response") == "Ok") {
	cout << "plz" << endl;
	string k = getParam("Room");
	cout << k << endl;
        Room p = deserializeRoom(k);
        cout << "Room Data Received: " << p.getRoomId() << endl;
    } else {
        cout << "Room Not Found or Invalid Request." << endl;
    }
}
*/
/*
void CSmessage::processGetAlarmResponse() {
    string response = getParam("Response");

    if (response == "Ok") {
        string alarmStatus = getParam("Alarm");
        cout << " Alarm Status: " << alarmStatus << endl;
    } else {
        cout << " Alarm Not Found or Invalid Request." << endl;
    }
}

void CSmessage::processGetLockResponse() {
    string response = getParam("Response");

    if (response == "Ok") {
        string lockStatus = getParam("Lock");
        cout << " Lock Status: " << lockStatus << endl;
    } else {
        cout << " Lock Not Found or Invalid Request." << endl;
    }
}

*/
/*
void CSmessage::processGetListHomesResponse() {
    string jsonString = getParam("json");

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
}*/


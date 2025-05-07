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
#include <nlohmann/json.hpp>

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
    for (const auto& [id, robot] : room.GetAllRobots()) {
    	j["Robots"].push_back({
        	{"Id", robot.getId()},
        	{"X", robot.getX()},
        	{"Y", robot.getY()},
       		{"ControlledBy", robot.getControlledBy()}
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
    vector<Robot> robots;
    if (roomJson.contains("Robots")) {
        for (const auto& robotJson : roomJson["Robots"]) {
            Robot r(robotJson["Id"]);
            r.setX(robotJson["X"]);
            r.setY(robotJson["Y"]);
            r.setControlledBy(robotJson["ControlledBy"]);
	    robots.push_back(r);
        }
    }

    Room room(roomId, lights, robots);
    room.SetWindowBlinds(windowBlinds);
    return room;
}


Room CSmessage::extractRoom() {
    cout << "typeshi" << endl;
    return deserializeRoom(messageData["Room"]);
}

void CSmessage::processLogoutResponse() {
    string response = getParam("Response");

    if (response == "Ok") {
        cout << " Logout Successful!" << endl;
    } else {
        cout << " Logout Failed: Unknown issue." << endl;
    }
}



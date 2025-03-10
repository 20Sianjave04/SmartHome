/*
 * CSmessage.h
 *
 *  Created on: Feb 13, 2025
 *      Author: nigel
 */
#ifndef CSMESSAGE_H_
#define CSMESSAGE_H_

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <nlohmann/json.hpp>  // Include JSON library
#include "CSHome.h"

using json = nlohmann::json;
using namespace std;

class CSmessage {
private:
    json messageData; // Store the message as a JSON object

public:
    CSmessage();
    virtual ~CSmessage();

    void reset();
    string getType();
    string getParam(string key);
    void setType(string type);
    void addParam(string key, string value);

    string marshal();
    void unmarshal(string str);

    // Serialization & Deserialization
    json serializeRoom(const Room& room);
    Room deserializeRoom(const string& roomStr);

    // Requests
    //void createLoginMessage(string username, string password);
    //void createLogoutMessage();
    //void createListHomesMessage();
    //void createAccessHomeMessage(string homeName);
    //void createGetRoomMessage(int roomId);
    //void createSetRoomMessage(const Room& room);
    //void createGetLockMessage(int lockId);
    //void createSetLockMessage(int lockId, bool open, int pin);
    //void createGetAlarmMessage(int alarmId);
    //void createSetAlarmMessage(int alarmId, bool on, int pin);

    // Responses
    //void SetRoomResponse(Home& home, Room roomInput);
    //void SetAlarmResponse(Home& home, int alarmId, bool on, int pin);
    //void SetLockResponse(Home& home, int lockId, bool open, int pin);

    //void createLoginResponse(bool success);
    //void createLogoutResponse();
    //void createListHomesResponse(const vector<string>& homeList);
    //void createAccessHomeResponse(const Home& home);
    //void createGetRoomResponse(const Room* room);
    //void createGetAlarmResponse(const Alarm* alarm);
    //void createGetLockResponse(const Lock* lock);

    //void processLoginResponse();
    void processLogoutResponse();
    //void processGetRoomResponse();
    //void processGetAlarmResponse();
    //void processGetLockResponse();
    //void processGetListHomesResponse();
    Room extractRoom();
};

#endif /* CSMESSAGE_H_ */


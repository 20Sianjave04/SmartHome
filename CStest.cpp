#include <iostream>
#include "CSmessage.h"
#include "CSHome.h"
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

void testLoginLogout() {
    cout << "\n=== Login / Logout Test ===\n";

    CSmessage loginMsg;
    loginMsg.createLoginMessage("user1", "pass1");
    string loginRequest = loginMsg.marshal();
    cout << "Marshalled Login Request: " << loginRequest << endl;

    CSmessage loginRcv;
    loginRcv.unmarshal(loginRequest);
    cout << "Unmarshalled Login Request Type: " << loginRcv.getType() << endl;


    CSmessage loginResponse;
    loginResponse.createLoginResponse(true);
    string loginResponseStr = loginResponse.marshal();
    cout << "Marshalled Login Response: " << loginResponseStr << endl;

    CSmessage logoutMsg;
    logoutMsg.createLogoutMessage();
    string logoutRequest = logoutMsg.marshal();
    cout << "Marshalled Logout Request: " << logoutRequest << endl;

    CSmessage logoutRcv;
    logoutRcv.unmarshal(logoutRequest);
    cout << "Unmarshalled Logout Request Type: " << logoutRcv.getType() << endl;

    CSmessage logoutResponse;
    logoutResponse.createLogoutResponse();
    string logoutResponseStr = logoutResponse.marshal();
    cout << "Marshalled Logout Response: " << logoutResponseStr << endl;
}

void testListHomes() {
    cout << "\n=== List Homes Test ===\n";

    CSmessage listMsg;
    listMsg.createListHomesMessage();
    string request = listMsg.marshal();
    cout << "Marshalled Request: " << request << endl;

    CSmessage requestRcv;
    requestRcv.unmarshal(request);
    cout << "Unmarshalled Request Type: " << requestRcv.getType() << endl;

    vector<string> homes = {"Home1", "Home2", "Home3"};
    CSmessage response;
    response.createListHomesResponse(homes);
    string responseStr = response.marshal();
    cout << "Marshalled Response: " << responseStr << endl;
}

void testAccessHome() {
    cout << "\n=== Access Home Test ===\n";

    CSmessage accessMsg;
    accessMsg.createAccessHomeMessage("SmartHome");
    string request = accessMsg.marshal();
    cout << "Marshalled Request: " << request << endl;

    CSmessage requestRcv;
    requestRcv.unmarshal(request);
    cout << "Unmarshalled Request Type: " << requestRcv.getType() << endl;
    cout << "HomeName: " << requestRcv.getParam("HomeName") << endl;
    Home home("SmartHome",
        {Room(101, {LightGroup(1, "Red"), LightGroup(2, "Blue")})},
        {Alarm(201, true)},
        {Lock(301, {1234})}
    );

    CSmessage response;
    response.createAccessHomeResponse(home);
    string responseStr = response.marshal();
    cout << "Marshalled Response: " << responseStr << endl;
}

void testRoom() {
    cout << "\n--- Room Test ---\n";
    LightGroup num1 = LightGroup(1, "Red");
    LightGroup num2 = LightGroup(2, "Blue");
    vector<LightGroup> lighty = {num1, num2};

    Home myHome("SmartHome",
        {Room(101, lighty)},
        {}, {}
    );

    Room testRoom(101, {LightGroup(1, "Green")});
    testRoom.SetWindowBlinds(true);

    CSmessage setRoomMsg;
    setRoomMsg.createSetRoomMessage(testRoom);
    string setRequest = setRoomMsg.marshal();
    cout << "Marshalled Set Room Request: " << setRequest << endl;
    CSmessage setRoomRcv;
    setRoomRcv.unmarshal(setRequest);
    string k = setRoomRcv.getParam("RoomData");
    Room p = setRoomRcv.deserializeRoom(k);
    cout << "Unmarshalled Set Room Request: " << setRoomRcv.getType() << endl;
    cout << "Room Id: " << p.getRoomId() << endl;
    cout << "WindowBlinds: " << p.getWindowBlinds() << endl;

    CSmessage setResponse;
    setResponse.SetRoomResponse(myHome, p);
    string setResponseStr = setResponse.marshal();
    cout << "Marshalled Set Room Response: " << setResponseStr << endl;

    CSmessage setResponseRcv;
    setResponseRcv.unmarshal(setResponseStr);
    cout << "After unmarshalin and marshaling: " << setResponseRcv.marshal() << endl;

    CSmessage getRoomMsg;
    getRoomMsg.createGetRoomMessage(101);
    string getRequest = getRoomMsg.marshal();
    cout << "Marshalled Get Room Request: " << getRequest << endl;

    CSmessage getRoomRcv;
    getRoomRcv.unmarshal(getRequest);
    cout << "Unmarshalled Get Room Request: " << getRoomRcv.getType() << endl;
    cout << "Room id: " << getRoomRcv.getParam("RoomId") << endl;


    CSmessage getResponse;
    getResponse.createGetRoomResponse(&testRoom);
    string getResponseStr = getResponse.marshal();
    cout << "Marshalled Get Room Response: " << getResponseStr << endl;



}
void testLock() {
    cout << "\n=== Lock Test ===\n";

    CSmessage setLockMsg;
    setLockMsg.createSetLockMessage(202, true, 1234);
    string setRequest = setLockMsg.marshal();
    cout << "Marshalled Set Lock Request: " << setRequest << endl;

    CSmessage setLockRcv;
    setLockRcv.unmarshal(setRequest);
    cout << "Unmarshalled Set Lock Request Type: " << setLockRcv.getType() << endl;

    int lockId = stoi(setLockRcv.getParam("LockId"));
    bool open = setLockRcv.getParam("Open") == "1";
    int pin = stoi(setLockRcv.getParam("Pin"));
    Home myHome("SmartHome", {}, {}, {Lock(202, {1234})});

    CSmessage setResponse;
    setResponse.SetLockResponse(myHome, lockId, open, pin);
    string setResponseStr = setResponse.marshal();
    cout << "Marshalled Set Lock Response: " << setResponseStr << endl;

    CSmessage getLockMsg;
    getLockMsg.createGetLockMessage(202);
    string getRequest = getLockMsg.marshal();
    cout << "Marshalled Get Lock Request: " << getRequest << endl;

    CSmessage getLockRcv;
    getLockRcv.unmarshal(getRequest);
    cout << "Unmarshalled Get Lock Request Type: " << getLockRcv.getType() << endl;

    CSmessage getResponse;
    getResponse.createGetLockResponse(&myHome.GetLocks()[0]);
    string getResponseStr = getResponse.marshal();
    cout << "Marshalled Get Lock Response: " << getResponseStr << endl;
}

void testAlarm() {
    cout << "\n=== Alarm Test ===\n";

    CSmessage setAlarmMsg;
    setAlarmMsg.createSetAlarmMessage(303, true, 5678);
    string setRequest = setAlarmMsg.marshal();
    cout << "Marshalled Set Alarm Request: " << setRequest << endl;

    CSmessage setAlarmRcv;
    setAlarmRcv.unmarshal(setRequest);
    cout << "Unmarshalled Set Alarm Request Type: " << setAlarmRcv.getType() << endl;

    int alId = stoi(setAlarmRcv.getParam("AlarmId"));
    bool on = setAlarmRcv.getParam("On") == "1";
    int pin = stoi(setAlarmRcv.getParam("Pin"));

    Home myHome("SmartHome", {}, {Alarm(303, 5678)}, {});

    CSmessage setResponse;
    setResponse.SetAlarmResponse(myHome, alId, on, pin);
    string setResponseStr = setResponse.marshal();
    cout << "Marshalled Set Alarm Response: " << setResponseStr << endl;

    CSmessage getAlarmMsg;
    getAlarmMsg.createGetAlarmMessage(303);
    string getRequest = getAlarmMsg.marshal();
    cout << "Marshalled Get Alarm Request: " << getRequest << endl;

    CSmessage getAlarmRcv;
    getAlarmRcv.unmarshal(getRequest);
    cout << "Unmarshalled Get Alarm Request Type: " << getAlarmRcv.getType() << endl;

    CSmessage getResponse;
    getResponse.createGetAlarmResponse(&myHome.GetAlarms()[0]);
    string getResponseStr = getResponse.marshal();
    cout << "Marshalled Get Alarm Response: " << getResponseStr << endl;
}

int main() {
    testLoginLogout();
    testListHomes();
    testAccessHome();
    testRoom();
    testLock();
    testAlarm();
    return 0;
}


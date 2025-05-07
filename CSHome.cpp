#include <iostream>
#include "CSHome.h"


Alarm:: Alarm() :AlarmId(0), On(false), Code(1234) {}

Alarm:: Alarm(int id, int code) : AlarmId(id), On(false), Code(code) {}

// Alarm Serialization
json Alarm::to_json() const {
    json j;
    j["AlarmId"] = AlarmId;
    j["On"] = On;
    j["Code"] = Code;
    return j;
}

// Alarm Deserialization
Alarm Alarm::from_json(const json& j) {
    Alarm alarm;
    alarm.AlarmId = j["AlarmId"];
    alarm.On = j["On"];
    alarm.Code = j["Code"];
    return alarm;
}

int Alarm:: GetAlarmId() const
{
        return AlarmId;
}

bool Alarm:: isOn() const
{
        return On;
}

int Alarm::GetCode() const
{
        return Code;
}

void Alarm::SetOn(bool newOn)
{
        On = newOn;
}
bool Alarm:: SetAlarm(bool newState, int inputCode)
{
        if (inputCode == GetCode())
        {
                SetOn(newState);
                cout << " Alarm Updated Succesfully" << endl;
                return true;
        }
        else
        {
                cout << "Wrong Code" << endl;
                return false;
        }
}


Lock:: Lock() : LockId(0), Open(false){}
Lock:: Lock(int id, vector<int>codes) : LockId(id), Open(false), PinCodes(codes){}

json Lock::to_json() const {
    json j;
    j["LockId"] = LockId;
    j["Open"] = Open;
    j["PinCodes"] = PinCodes; // Assuming PinCodes is a vector
    return j;
}

// Lock Deserialization
Lock Lock::from_json(const json& j) {
    Lock lock;
    lock.LockId = j["LockId"];
    lock.Open = j["Open"];
    lock.PinCodes = j["PinCodes"].get<vector<int>>(); // Convert JSON array to vector
    return lock;
}
int Lock:: GetLockId() const
{
        return LockId;
}


bool Lock:: isOpen() const
{
        return Open;
}

const vector<int>& Lock ::GetPinCodes() const
{
        return PinCodes;
}

void Lock:: SetOpen(bool newOpen)
{
        Open = newOpen;
}

bool Lock:: SetLock(bool newState, int inputCode)
{
        const vector<int>& localCodes = GetPinCodes();
        for (int code : localCodes)
        {
                if (inputCode == code)
                {
                        SetOpen(newState);
                        cout << "New Lock State updated " << endl;
                        return true;
                }
        }

        cout << "Wrong Code " << endl;
        return false;

}

void Lock:: AddCode(int newPin)
{
        PinCodes.push_back(newPin);
}

bool Lock:: AddPinCode(int OldPin, int newPin)
{
        const vector<int>& localCodes = GetPinCodes();

        for (int code : localCodes)
        {
                if (OldPin == code)
                {
                        for (int i : localCodes)
                        {
                                if (i == newPin)
                                {
                                        cout << "Pin Already exists" << endl;
                                        return false;
                                }
                        }
                        AddCode(newPin);
                        cout << "Succesfully added Pin" << endl;
                        return true;
                }
        }
        cout << "Invalid Pin" << endl;
        return false;
}


LightGroup:: LightGroup() : LightGroupId(0), On(false), Color("White"){}
LightGroup:: LightGroup(int id, string color) : LightGroupId(id), On(false), Color(color){}

json LightGroup::to_json() const {
    json j;
    j["LightGroupId"] = LightGroupId;
    j["On"] = On;
    j["Color"] = Color;
    return j;
}

// LightGroup Deserialization
LightGroup LightGroup::from_json(const json& j) {
    LightGroup lightGroup;
    lightGroup.LightGroupId = j["LightGroupId"];
    lightGroup.On = j["On"];
    lightGroup.Color = j["Color"];
    return lightGroup;
}

int LightGroup :: getId() const
{
        return LightGroupId;
}
bool LightGroup :: isOn() const
{
        return On;
}
string LightGroup :: getColor() const
{
        return Color;
}

void LightGroup :: ShowState() const
{
        cout << "On : " << isOn() << endl;
        cout << "Color: " << getColor() << endl;
}

void LightGroup :: SetOn(bool newOn)
{
        On = newOn;
}

void LightGroup :: SetColor(string newColor)
{
        Color = newColor;
}
/*
Room :: Room() : RoomID(0), WindowBlinds(false){}
Room :: Room(int id, vector<LightGroup> lightGroups) : RoomID(id), LightGroups(lightGroups), WindowBlinds(false){}
*/


// Robot Constructors
Robot::Robot() : id(0), x(0), y(0), controlledBy("") {
    lastMoved = std::chrono::steady_clock::now();
}

Robot::Robot(int id_) : id(id_), x(0), y(0), controlledBy("") {
    lastMoved = std::chrono::steady_clock::now();
}

json Robot::to_json() const {
    json j;
    j["id"] = id;
    j["x"] = x;
    j["y"] = y;
    j["controlledBy"] = controlledBy;
    return j;
}

// Robot Deserialization
Robot Robot::from_json(const json& j) {
    Robot robot;
    robot.id = j["id"];
    robot.x = j["x"];
    robot.y = j["y"];
    robot.controlledBy = j["controlledBy"];
    return robot;
}
// Accessors (getters)
int Robot::getId() const {
    return id;
}

int Robot::getX() const {
    return x;
}

int Robot::getY() const {
    return y;
}

string Robot::getControlledBy() const {
    return controlledBy;
}

// Mutators (setters)
void Robot::setX(int x_) {
    x = x_;
}

void Robot::setY(int y_) {
    y = y_;
}

void Robot::setControlledBy(const string& name) {
    controlledBy = name;
}

//NEW STUFF
void Room::AddRobot(const Robot& r) {
    robots[r.getId()] = r;
}

Robot* Room::GetRobot(int id) {
    if (robots.count(id)) return &robots[id];
    return nullptr;
}

const unordered_map<int, Robot>& Room::GetAllRobots() const {
    return robots;
}

void Room::AddUser(const string& username) {
    usersInRoom.insert(username);
}

void Room::RemoveUser(const string& username) {
    usersInRoom.erase(username);
}

bool Room::HasUser(const string& username) const {
    return usersInRoom.count(username) > 0;
}

unordered_set<string> Room::GetUsers() const {
    return usersInRoom;
}

Room :: Room() : RoomID("NA"), WindowBlinds(false){}
Room::Room(string id, vector<LightGroup> lightGroups, vector<Robot> robots)
    : RoomID(id), LightGroups(lightGroups), WindowBlinds(false)
    {
        for (const auto& robot : robots) {
           AddRobot(robot);
    }
}

json Room::to_json() const {
    json j;
    j["RoomID"] = RoomID;
    j["WindowBlinds"] = WindowBlinds;

    // Serialize LightGroups
    json lightGroupsArray = json::array();
    for (const auto& lightGroup : LightGroups) {
        lightGroupsArray.push_back(lightGroup.to_json());
    }
    j["LightGroups"] = lightGroupsArray;

    // Serialize Robots
    json robotsArray = json::array();
    for (const auto& [id, robot] : robots) {
        robotsArray.push_back(robot.to_json());
    }
    j["Robots"] = robotsArray;

    return j;
}

// Room Deserialization
Room Room::from_json(const json& j) {
    Room room;
    room.RoomID = j["RoomID"];
    room.WindowBlinds = j["WindowBlinds"];

    // Deserialize LightGroups
    for (const auto& lightGroupJson : j["LightGroups"]) {
        room.LightGroups.push_back(LightGroup::from_json(lightGroupJson));
    }

    // Deserialize Robots
    for (const auto& robotJson : j["Robots"]) {
        room.robots[robotJson["id"]] = Robot::from_json(robotJson);
    }

    return room;
}

LightGroup * Room :: GetLightGroup(int id)
{
        for (LightGroup& group : LightGroups)
        {
                if (group.getId() == id)
                {
                        return &group;
                }
        }
        return nullptr;
}

vector<LightGroup> Room::GetAllLightGroups()  const {
    return LightGroups;
}
vector<LightGroup> &Room::GetAllLightGroups2() {
    return LightGroups;
}


void Room :: AddLightGroup(const LightGroup& newLight)
{
        if (GetLightGroup(newLight.getId()) == nullptr)
        {
                LightGroups.push_back(newLight);
                cout << "New Light Group added " << endl;
        }
        else
        {
                cout << "Light with such id already exists" << endl;
        }
}

void Room::SetWindowBlinds(bool newBlinds)
{
        WindowBlinds = newBlinds;
}

void  Room :: SetLightGroup(int id, bool newOn, string newColor)
{
        LightGroup * local = GetLightGroup(id);
        if (local)
        {
                local->SetOn(newOn);
                local->SetColor(newColor);
        }
        else
        {
                cout << "Light Group Not found" << endl;
        }
}
/*
int Room :: getRoomId() const
{
        return RoomID;
}
*/
string Room :: getRoomId() const
{
        return RoomID;
}
bool Room :: getWindowBlinds() const
{
        return WindowBlinds;
}

Home :: Home() : HomeName("NA"){}
Home :: Home(string name, vector<Room> rooms, vector<Alarm> alarms, vector<Lock> locks) : HomeName(name), Rooms(rooms), Alarms(alarms), Locks(locks){}
/*
Room * Home :: GetRoom(int id)
{
        for (Room& room : Rooms)
        {
                if (room.getRoomId() == id)
                {
                        return &room;
                }
        }
        return nullptr;
}
*/
Room * Home :: GetRoom(string id)
{
	cout << "wtf" << endl;
        for (Room& room : Rooms)
        {
		cout << "1" << endl;
		cout << room.getRoomId();
		cout << "2" << endl;
                if (room.getRoomId() == id)
                {
                        return &room;
                }
        }
	cout << "no way" << endl;
        return nullptr;
}
Alarm * Home :: GetAlarm(int id)
{
        for (Alarm& alarm : Alarms)
        {
                if (alarm.GetAlarmId() == id)
                {
                        return &alarm;
                }
        }
        return nullptr;
}

Lock * Home :: GetLock(int id)
{
        for (Lock & lock : Locks)
        {
                if (lock.GetLockId() == id)
                {
                        return &lock;
                }
        }
        return nullptr;
}

vector<Room> Home:: GetRooms() const
{
        return Rooms;
}

vector<Alarm> Home:: GetAlarms() const
{
        return Alarms;
}
vector<Lock> Home:: GetLocks() const
{
        return Locks;
}



void Home :: AddRoom(const Room& newRoom)
{
        if (GetRoom(newRoom.getRoomId()) == nullptr)
        {
                Rooms.push_back(newRoom);
                cout << "Room Added Succesfully" << endl;
        }
        else
        {
                cout << "Room with such id, already in the room" << endl;
        }
}
void Home :: AddAlarm(const Alarm& newAlarm)
{
        if (GetAlarm(newAlarm.GetAlarmId()) == nullptr)
        {
                Alarms.push_back(newAlarm);
                cout << "Alarm Added Succesfully" << endl;
        }
        else
        {
                cout << "Alarm with such id already in the room" << endl;
        }
}

void Home :: AddLock(const Lock& newLock)
{
        if(GetLock(newLock.GetLockId()) == nullptr)
        {
                Locks.push_back(newLock);
                cout << "Lock Added Succesfully" << endl;
        }
        else
        {
                cout << "Lock with such id, alredy exists" << endl;
        }
}

string Home :: GetName() const
{
        return HomeName;
}

json Home::to_json() const {
    json j;
    j["HomeName"] = HomeName;

    // Serialize Rooms
    json roomsArray = json::array();
    for (const auto& room : Rooms) {
        roomsArray.push_back(room.to_json());
    }
    j["Rooms"] = roomsArray;

    // Serialize Alarms
    json alarmsArray = json::array();
    for (const auto& alarm : Alarms) {
        alarmsArray.push_back(alarm.to_json());
    }
    j["Alarms"] = alarmsArray;

    // Serialize Locks
    json locksArray = json::array();
    for (const auto& lock : Locks) {
        locksArray.push_back(lock.to_json());
    }
    j["Locks"] = locksArray;

    return j;
}

// Home Deserialization
Home Home::from_json(const json& j) {
    Home home;
    home.HomeName = j["HomeName"];

    // Deserialize Rooms
    for (const auto& roomJson : j["Rooms"]) {
        home.Rooms.push_back(Room::from_json(roomJson));
    }

    // Deserialize Alarms
    for (const auto& alarmJson : j["Alarms"]) {
        home.Alarms.push_back(Alarm::from_json(alarmJson));
    }

    // Deserialize Locks
    for (const auto& lockJson : j["Locks"]) {
        home.Locks.push_back(Lock::from_json(lockJson));
    }

    return home;
}

User :: User() : Username("NA"), Password("NA"){}
User :: User(string name, string password, vector<Home*>net) : Username(name), Password(password), Networth(net){}

json User::to_json() const {
    json j;
    j["Username"] = Username;
    j["Password"] = Password;

    // Serialize Homes
    json homesArray = json::array();
    for (const auto& home : Networth) {
        homesArray.push_back(home->to_json());
    }
    j["Homes"] = homesArray;

    return j;
}

// User Deserialization
User User::from_json(const json& j) {
    User user;
    user.Username = j["Username"];
    user.Password = j["Password"];

    // Deserialize Homes
    for (const auto& homeJson : j["Homes"]) {
        // Here, we assume that you have to manage the pointers to Home objects.
        // Be careful with pointer handling when deserializing.
        Home* home = new Home(Home::from_json(homeJson));  // Allocate memory for the new Home
        user.Networth.push_back(home);  // Add to user's homes
    }

    return user;
}

string User:: GetUsername()
{
        return Username;
}

string User:: GetPassword()
{
        return Password;
}

vector<Home*> User :: GetHomes()
{
	return Networth;
}

Home * User :: GetHome(string homeName)
{
        for (Home * home : Networth)
        {
                if (home->GetName() == homeName)
                {
                        return home;
                }
        }
        return nullptr;
}

bool User :: CheckUsername(string us)
{	
	if (us != GetUsername())
	{
		return false;
	}
	return true;
	
}

bool User :: CheckPassword(string pw)
{
	if(pw != GetPassword())
	{
		return false;
	}
	return true;
}
void User::AddHome(const Home& home) {
    Home* newHome = new Home(home);  // Allocate memory for a new Home
    Networth.push_back(newHome);
}


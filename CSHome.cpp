#include <iostream>
#include "CSHome.h"


Alarm:: Alarm() :AlarmId(0), On(false), Code(1234) {}

Alarm:: Alarm(int id, int code) : AlarmId(id), On(false), Code(code) {}

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
Room :: Room() : RoomID("NA"), WindowBlinds(false){}
Room :: Room(string id, vector<LightGroup> lightGroups) : RoomID(id), LightGroups(lightGroups), WindowBlinds(false){}

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
        for (Room& room : Rooms)
        {
                if (room.getRoomId() == id)
                {
                        return &room;
                }
        }
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

User :: User() : Username("NA"), Password("NA"){}
User :: User(string name, string password, vector<Home>net) : Username(name), Password(password), Networth(net){}

string User:: GetUsername()
{
        return Username;
}

string User:: GetPassword()
{
        return Password;
}

vector<Home> User :: GetHomes()
{
	return Networth;
}

Home * User :: GetHome(string homeName)
{
        for (Home & home : Networth)
        {
                if (home.GetName() == homeName)
                {
                        return &home;
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



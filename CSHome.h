#ifndef CSHOME_H_
#define CSHOME_H_
#include <iostream>
#include <vector>
#include <string>

using namespace std;

//Alarm class
class Alarm {
        private:
                int AlarmId;
                bool On;
                int Code;
                int GetCode() const;
                void SetOn(bool);
        public:
                Alarm();
                Alarm(int id, int code);
                int GetAlarmId() const;
                bool isOn() const;
                bool SetAlarm(bool, int);
};

//Lock CLass
class Lock {
        private:
                int LockId;
                bool Open;
                vector<int> PinCodes;
                const vector<int>& GetPinCodes() const;
                void SetOpen(bool);
                void AddCode(int);
        public:
                Lock();
                Lock(int id, vector<int> codes);
                int GetLockId() const;
                bool isOpen() const;
                bool SetLock(bool, int);
                bool AddPinCode(int OldPin, int newPin);
};

//LightGorup Class
class LightGroup {
        private:
                int LightGroupId;
                bool On;
                string Color;
        public:
                LightGroup();
                LightGroup(int id, string color);
                void ShowState() const;
                void SetOn(bool);
                void SetColor(string);

                int getId() const;
                bool isOn() const;
                string getColor() const;
};

//Room CLass
class Room {
        private:
                //int RoomID;
		string RoomID;
                vector<LightGroup> LightGroups;
                bool WindowBlinds;
        public:
                Room();
                //Room(int, vector<LightGroup>);
                Room(string, vector<LightGroup>);
                LightGroup * GetLightGroup(int);
                vector<LightGroup> GetAllLightGroups() const;
		vector<LightGroup> &GetAllLightGroups2();
                void AddLightGroup(const LightGroup&);
                void SetWindowBlinds(bool);
                void SetLightGroup(int, bool, string);
                //int getRoomId() const;
		string getRoomId() const;
                bool getWindowBlinds() const;

};

//Home Class
class Home {
        private:
                string HomeName;
                vector<Room> Rooms;
                vector<Alarm> Alarms;
                vector<Lock> Locks;
        public:
                Home();
                Home(string, vector<Room>, vector<Alarm>, vector<Lock>);
                vector<Room> GetRooms() const;
                vector<Alarm> GetAlarms() const;
                vector<Lock> GetLocks() const;

                //Room * GetRoom(int);
		Room * GetRoom(string);
                Alarm * GetAlarm(int);
                Lock * GetLock(int);
                void AddRoom(const Room&);
                void AddAlarm(const Alarm&);
                void AddLock(const Lock&);
                string GetName() const;
};
// User Class
class User {
        private:
                string Username;
                string Password;
                vector<Home> Networth;
                string GetUsername();
                string GetPassword();
        public:
                User();
                User(string name, string password, vector<Home>net);
		vector<Home> GetHomes();
                Home * GetHome(string);
                void AddHome(const Home&);
                bool CheckUsername(string);
                bool CheckPassword(string);


};
#endif /* CSHOME_H_ */

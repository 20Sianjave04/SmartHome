#ifndef CSHOME_H_
#define CSHOME_H_
#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <unordered_map>
#include <unordered_set>
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

// --- Robot Class ---
class Robot {
private:
    int id;
    int x, y;
    string controlledBy;
    std::chrono::steady_clock::time_point lastMoved;

public:
    Robot();
    Robot(int id_);

    // Accessors (getters)
    int getId() const;
    int getX() const;
    int getY() const;
    string getControlledBy() const;

    // Mutators (setters)
    void setX(int x_);
    void setY(int y_);
    void setControlledBy(const string& name);
};


//Room CLass
class Room {
        private:
                //int RoomID;
		string RoomID;
                vector<LightGroup> LightGroups;
                bool WindowBlinds;
		unordered_map<int, Robot> robots;       
                unordered_set<string> usersInRoom; 
        public:
                Room();
                //Room(int, vector<LightGroup>);
                Room(string id, vector<LightGroup> lightGroups, vector<Robot> robots = {});
                LightGroup * GetLightGroup(int);
                vector<LightGroup> GetAllLightGroups() const;
		vector<LightGroup> &GetAllLightGroups2();
                void AddLightGroup(const LightGroup&);
                void SetWindowBlinds(bool);
                void SetLightGroup(int, bool, string);
                //int getRoomId() const;
		string getRoomId() const;
                bool getWindowBlinds() const;
                
		// NEW methods for robots
        	void AddRobot(const Robot& r);
        	Robot* GetRobot(int id);
        	const unordered_map<int, Robot>& GetAllRobots() const;

        	// NEW methods for user presence
        	void AddUser(const string& username);
        	void RemoveUser(const string& username);
        	bool HasUser(const string& username) const;
        	unordered_set<string> GetUsers() const;
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
                vector<Home*> Networth;
                string GetUsername();
                string GetPassword();
        public:
                User();
                User(string name, string password, vector<Home*>net);
		vector<Home*> GetHomes();
                Home * GetHome(string);
                void AddHome(const Home&);
                bool CheckUsername(string);
                bool CheckPassword(string);


};
#endif /* CSHOME_H_ */

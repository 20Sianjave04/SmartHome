#pragma once
#include "CSHome.h"




// User Class
class User {
        private:
                string Username;
                string Password;
                vector<Home*> Networth;

                string GetPassword();
        public:
                User();
                User(string name, string password, vector<Home*>net);
		vector<Home*> GetHomes();
		vector<Home*> GetHomes() const;
                Home * GetHome(string);
                void AddHome(const Home&);
		void AddHome(Home* homePtr);
                bool CheckUsername(string);
                bool CheckPassword(string);
		json to_json() const; // Add this for serialization
                static User from_json(const json& j);
		string GetUsername();

};

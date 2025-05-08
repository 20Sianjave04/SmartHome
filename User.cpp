#include "User.h"
#include "CSdatabase.h" // for getHomeByName
#include <iostream>

User :: User() : Username("NA"), Password("NA"){}
User :: User(string name, string password, vector<Home*>net) : Username(name), Password(password), Networth(net){}

json User::to_json() const {
    json j;
    j["Username"] = Username;
    j["Password"] = Password;

    // Just store names of homes this user references
    json homeNames = json::array();
    for (const auto& home : Networth) {
        homeNames.push_back(home->GetName());
    }
    j["Homes"] = homeNames;

    return j;
}

User User::from_json(const json& j) {
    User user;
    user.Username = j["Username"];
    user.Password = j["Password"];

    for (const auto& name : j["Homes"]) {
    	Home* h = getHomeByName(name.get<std::string>());
   	if (h) {
        	user.Networth.push_back(h);  // ✅ shared pointer to allHomes
    	} else {
        	std::cerr << "[Warning] Home not found: " << name << std::endl;
    	}
     }

    return user;
}

void User::AddHome(Home* homePtr) {
    Networth.push_back(homePtr);
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
vector<Home*> User::GetHomes() const {
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


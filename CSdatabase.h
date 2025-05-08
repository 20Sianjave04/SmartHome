#pragma once
#include <map>
#include <vector>
#include <fstream>
#include <nlohmann/json.hpp>
#include "User.h"

extern std::map<std::string, User> users;
extern std::vector<Home*> allHomes;

Home* getHomeByName(const std::string& name);
bool addUser(const std::string& username, const std::string& password);
bool assignHomeToUser(const std::string& username, const std::string& homeName);
void loadUsers(const std::string& filename);
void loadHomes(const std::string& filename);
void saveUsers(const std::string& filename);
void saveHomes(const std::string& filename);

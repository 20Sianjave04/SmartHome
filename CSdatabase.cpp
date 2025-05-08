#include "CSdatabase.h"

using json = nlohmann::json;

// Global user and home storage
std::map<std::string, User> users;
std::vector<Home*> allHomes;


Home* getHomeByName(const std::string& name) {
    for (Home* h : allHomes) {
        if (h->GetName() == name) return h;
    }
    return nullptr;
}
bool addUser(const std::string& username, const std::string& password) {
    if (users.count(username)) {
        std::cerr << "[Error] User already exists: " << username << std::endl;
        return false;
    }
    users[username] = User(username, password, {});
    return true;
}

bool assignHomeToUser(const std::string& username, const std::string& homeName) {
    if (!users.count(username)) {
        std::cerr << "[Error] User not found: " << username << std::endl;
        return false;
    }

    Home* home = getHomeByName(homeName);
    if (!home) {
        std::cerr << "[Error] Home not found: " << homeName << std::endl;
        return false;
    }

    users[username].AddHome(home);  // assumes AddHome(Home*) exists
    return true;
}
void loadUsers(const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile) {
        json j;
        inFile >> j;
        for (const auto& [username, userData] : j.items()) {
            User u = User::from_json(userData);
            users[username] = u;
        }
        std::cout << "Loaded users from file.\n";
	for (const auto& [username, user] : users) {
    		std::cout << "[Debug] User: " << username << ", homes: ";
    		for (const auto& home : user.GetHomes()) {
        		std::cout << home->GetName() << " ";
    		}
    		std::cout << std::endl;
	}
    } else {
        std::cout << "No users.json file found. Initializing default users...\n";
        // Add your hardcoded users
        users = {
            {"user1", {"user1", "pass1", {}}},
            {"admin", {"admin", "adminpass", {}}},
            {"Nigel", {"Nigel", "John", {}}},
            {"Jane", {"Jane", "janepass", {}}},
            {"Bob", {"Bob", "bobpass", {}}}
        };
	for (auto& [username, user] : users) {
    		for (Home* h : allHomes) {
        		if (username == "user1" && (h->GetName() == "SmartHome" || h->GetName() == "VacationVilla")) {
            			user.AddHome(*h);
        		}
        		else if ((username == "admin" || username == "Nigel") && (h->GetName() == "AdminHouse" || h->GetName() == "Penthouse")) {
            			user.AddHome(*h);
        		}
        		else if (username == "Jane" && (h->GetName() == "SmartHome" || h->GetName() == "Penthouse")) {
            			user.AddHome(*h);
        		}
        		else if (username == "Bob" && (h->GetName() == "VacationVilla" || h->GetName() == "AdminHouse")) {
            			user.AddHome(*h);
        		}		
    		}
	}
    }
}

void loadHomes(const std::string& filename) {
    std::ifstream inFile(filename);
    if (inFile) {
        json j;
        inFile >> j;
        for (const auto& homeData : j) {
            Home* h = new Home(Home::from_json(homeData));
            allHomes.push_back(h);
        }
        std::cout << "Loaded homes from file.\n";
    } else {
        std::cout << "No homes.json file found. Initializing default homes...\n";
        allHomes = {
            new Home("SmartHome", {
                Room("101", {LightGroup(1, "White"), LightGroup(2, "Blue")}, {Robot(1), Robot(2)}),
                Room("102", {LightGroup(3, "Red"), LightGroup(4, "Green")}, {Robot(3)})
            }, {Alarm(1, 1234)}, {Lock(1, {1111, 2222, 3333})}),
            new Home("VacationVilla", {
                Room("201", {LightGroup(5, "Yellow"), LightGroup(6, "Purple")}, {Robot(4)}),
                Room("202", {LightGroup(7, "Cyan"), LightGroup(8, "Orange")}, {Robot(5), Robot(6)})
            }, {Alarm(2, 5678)}, {Lock(2, {4444, 5555, 6666})}),
            new Home("AdminHouse", {
                Room("301", {LightGroup(9, "Pink"), LightGroup(10, "Brown")}, {Robot(7)}),
                Room("302", {LightGroup(11, "Gray"), LightGroup(12, "Black")}, {Robot(8), Robot(9)})
            }, {Alarm(3, 4321)}, {Lock(3, {7777, 8888, 9999})}),
            new Home("Penthouse", {
                Room("401", {LightGroup(13, "Magenta"), LightGroup(14, "Teal")}, {Robot(10)}),
                Room("402", {LightGroup(15, "Gold"), LightGroup(16, "Silver")}, {Robot(11), Robot(12)})
            }, {Alarm(4, 8765)}, {Lock(4, {1010, 2020, 3030})})
        };
	/*
        // Assign homes to users
        users["user1"].AddHome(*allHomes[0]);
        users["user1"].AddHome(*allHomes[1]);
        users["admin"].AddHome(*allHomes[2]);
        users["admin"].AddHome(*allHomes[3]);
        users["Nigel"].AddHome(*allHomes[2]);
        users["Nigel"].AddHome(*allHomes[3]);
        users["Jane"].AddHome(*allHomes[0]);
        users["Jane"].AddHome(*allHomes[3]);
        users["Bob"].AddHome(*allHomes[1]);
        users["Bob"].AddHome(*allHomes[2]);
	*/
	std::cout << "[Debug] allHomes contains: ";
    }
    std::cout << "[Debug] allHomes contains: ";
    for (Home* h : allHomes) {
    	std::cout << h->GetName() << " ";
    }
    std::cout << std::endl;
}

void saveUsers(const std::string& filename) {
    json j;
    for (const auto& [username, user] : users) {
        j[username] = user.to_json();
    }
    std::ofstream outFile(filename);
    outFile << j.dump(4);
    std::cout << "Users saved to " << filename << '\n';
}

void saveHomes(const std::string& filename) {
    json j = json::array();
    for (const auto& home : allHomes) {
        j.push_back(home->to_json());
    }
    std::ofstream outFile(filename);
    outFile << j.dump(4);
    std::cout << "Homes saved to " << filename << '\n';
}

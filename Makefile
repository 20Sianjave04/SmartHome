# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I$(PWD)/third_party/nlohmann -I$(PWD)/shared

# Source files for client and server
CLIENT_SRC = client/TCPClient.cpp shared/CSmessage.cpp shared/CSHome.cpp
SERVER_SRC = server/TCPServer.cpp shared/CSmessage.cpp shared/CSHome.cpp

# Object files for client and server
CLIENT_OBJ = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJ = $(SERVER_SRC:.cpp=.o)

# Output executables
CLIENT_EXEC = client/client
SERVER_EXEC = server/server

# Build rules
all: $(CLIENT_EXEC) $(SERVER_EXEC)

# Rule for client
$(CLIENT_EXEC): $(CLIENT_OBJ)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_EXEC) $(CLIENT_OBJ) -pthread

# Rule for server
$(SERVER_EXEC): $(SERVER_OBJ)
	$(CXX) $(CXXFLAGS) -o $(SERVER_EXEC) $(SERVER_OBJ) -pthread

# Generic rule for compiling .cpp files to .o object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule to remove generated files
clean:
	rm -f $(CLIENT_OBJ) $(SERVER_OBJ) $(CLIENT_EXEC) $(SERVER_EXEC)

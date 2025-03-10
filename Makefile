CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++17 -I/usr/include/nlohmann

TARGET = CStest

SOURCES = CSmessage.cpp CStest.cpp CSHome.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

CSmessage.o: CSmessage.cpp CSmessage.h CSHome.h
	$(CXX) $(CXXFLAGS) -c CSmessage.cpp -o CSmessage.o

CSHome.o: CSHome.cpp CSHome.h
	$(CXX) $(CXXFLAGS) -c CSHome.cpp -o CSHome.o

CStest.o: CStest.cpp CSmessage.h
	$(CXX) $(CXXFLAGS) -c CStest.cpp -o CStest.o

clean:
	rm -f $(OBJECTS) $(TARGET)


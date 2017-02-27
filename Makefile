SHELL = /bin/sh

CXX = g++
CXXFLAGS = -Wall -march=native
LDFLAGS = -lwiringPi -lao -lpthread
DEXEC = mavi-debug
FEXEC = mavi-final

SOURCES = \
	mavi-analysis.cpp \
	mavi-buttons.cpp  \
	mavi-control.cpp  \
	mavi-feedback.cpp \
	mavi-main.cpp     \
	mavi-mcp3008.cpp  \
	mavi-sensors.cpp  \
	mavi-vibration.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)

.PHONY: all objects final clean

all: $(DEXEC)

objects: $(OBJECTS)

$(DEXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

final:
	$(CXX) $(CXXFLAGS) -O2 $(SOURCES) $(LDFLAGS) -o $(FEXEC)

clean:
	rm -f $(OBJECTS) $(DEXEC)

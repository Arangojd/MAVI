SHELL = /bin/sh

CXX = g++
CXXFLAGS = -Wall -std=c++11
LDFLAGS = -lm -lwiringPi -lpthread
DEXEC = mavi-debug
FEXEC = mavi-final

SOURCES = \
	mavi-analysis.cpp \
	mavi-audio.cpp    \
	mavi-buttons.cpp  \
	mavi-calib.cpp    \
	mavi-feedback.cpp \
	mavi-init.cpp     \
	mavi-main.cpp     \
	mavi-mcp3008.cpp  \
	mavi-sensors.cpp  \
	mavi-signals.cpp  \
	mavi-state.cpp    \
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

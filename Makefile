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
	mavi-math.cpp     \
	mavi-mcp3008.cpp  \
	mavi-sensors.cpp  \
	mavi-signals.cpp  \
	mavi-state.cpp    \
	mavi-vibration.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)
DEPENDS = $(SOURCES:%.cpp=%.d)

.PHONY: all objects depends final clean

all: $(DEXEC)
objects: $(OBJECTS)
depends: $(DEPENDS)

include $(DEPENDS)

$(DEXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

%.d: %.cpp
	$(CXX) $(CXXFLAGS) -MM -MT '$@ $(@:%.d=%.o)' $< > $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

final:
	$(CXX) $(CXXFLAGS) -O2 $(SOURCES) $(LDFLAGS) -o $(FEXEC)

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(DEXEC)

#All the dirs containing source files
DIRS := . src src/PM_2 src/PM_2/MinCost
#Gets all of the .cpp files in all of the directories
SOURCES := $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
#Converts .cpp extension to .o
OBJS := $(patsubst %.cpp, %.o, $(SOURCES))

CFLAGS := -Wall
CXX ?= g++

SRCDIR := src

all: main.exe

main.exe: ${OBJS}
	$(CXX) $(CFLAGS) -o $@ ${OBJS}

.cpp.o:
	$(CXX) $(CFLAGS) $< -c -o $@

clean:
	rm -f ${OBJS} main.exe
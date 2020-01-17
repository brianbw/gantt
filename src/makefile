# Brian Wong
# CS 302 - 1004
# Assignment #11B Makefile

OBJS	= projectInfo.o ganttUtils.o
CC		= g++ -g -Wall -pedantic -std=c++11 -pthread -O3 

all: projectInfo

projectInfo: $(OBJS) ganttUtils.h
	$(CC) -o projectInfo $(OBJS)

projectInfo.o: projectInfo.cpp ganttUtils.h
	$(CC) -c projectInfo.cpp

ganttUtils.o: ganttUtils.cpp ganttUtils.h linkedQueue.h
	$(CC) -c ganttUtils.cpp

# -----
# clean by removing object files.

clean:
	rm	$(OBJS)


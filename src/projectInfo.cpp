
// Find project gantt information.

#include <cstdlib>
#include <iostream>
#include <thread>

using namespace std;

#include "ganttUtils.h"

int main(int argc, char *argv[])
{

// *****************************************************************
//  Headers...

	const int	LIMIT = 10;
	const char	*bold = "\033[1m";
	const char	*unbold = "\033[0m";
	string		stars = "";
	string		tasksFile = "";
	string		dotFile = "";
	bool		printFlag = false;
	ganttUtils	myProject;

	stars.append(65, '*');

// ------------------------------------------------------------------
//  Perform initial checks.
//	command line arguments
//	read project tasks file
//	basic validation of tasks file

	if (!myProject.getArguments(argc, argv, tasksFile, printFlag))
		exit(1);

	if (!myProject.readGraph(tasksFile)) {
		cout << "Error, can not find project file." << endl;
		exit(1);
	}

	if(!myProject.isValidProject()) {
		cout << "Invalid task list." << endl;
		exit(1);
	}

// ------------------------------------------------------------------
//  Display headers, verify enough tasks...

	cout << stars << endl << bold << "CS 302 - Assignment #11" << endl;
	cout << "Gantt Analysis Project" << unbold << endl;
	cout << endl;

	if (myProject.getTaskCount() < LIMIT) {
		cout << "Error, too few tasks." << endl;
		exit(1);
	}

// ------------------------------------------------------------------
//  Find project information
//	Note, some of these function can be threaded...

// ----------
//  To initiate a thread for an object function:
//	thread thd1(&<objectName>::<functionName>);
//  Note, as usual, must join to ensure completed:
//	thd1.join()

	thread t1(&ganttUtils::criticalPath, &myProject);
	thread t2(&ganttUtils::findAPs, &myProject);
	thread t3(&ganttUtils::topoSort, &myProject);	
	thread t4(&ganttUtils::findKeyMilestone, &myProject);
	thread t5(&ganttUtils::findNodePoint, &myProject);
	thread t6(&ganttUtils::findIndepedentMilestones, &myProject);

	t1.join();
	t2.join();
	t3.join();
	t4.join();
	t5.join();
	t6.join();

	myProject.findGraphInformation();
	myProject.findSlackTimes();
	myProject.findDependencyStats();

// -----
//  Display project information.
//	Note, all calculations done, just display results.

	myProject.printGraphInformation();

	if (printFlag)
		myProject.printGraph();

	myProject.printDependencyStats();
	myProject.printTopoSort();
	myProject.printAPs();
	myProject.printCriticalPath();
	myProject.printSlackTimes();

// *****************************************************************
//  All done.

	cout << stars << endl;
	cout << "Game Over, thank you for playing." << endl;

	return 0;
}


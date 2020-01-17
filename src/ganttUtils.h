
#include <iostream>
#include "linkedQueue.h"

using namespace std;

struct lNode
{
	int weight = 0;											// 1st vertex is base --> 1st 
	int vertex = -1;										// weight exists starting here
	lNode *next = NULL;										// points to next node
};

struct idxNode
{
	int inDegree = 0;										// increment everytime read in (2nd)
	int outDegree = 0; 										// increment everytime read in (1st)
	int topoInDeg = 0;										// copy of indegree
	int crInDeg = 0;										// copy of indegree
	lNode *head = NULL;										// points to start of linked list
};

class ganttUtils
{
	public:
		ganttUtils();										// constructor (initialize variables)
		~ganttUtils();										// destructor (cleared allocated data)

		bool getArguments(int, char*[], string&, bool&);	// parse and process arguments
		bool readGraph(string);								// reads in graph from passed file name
		bool isValidProject();								// determines if graph is D.A.G.

		int getTaskCount();									// get total task count

		void initializeValues();							// initialize arrays of class vars
		void findGraphInformation();						// calculate & print graph info
		void findKeyMilestone();							// find last milestone w/ max in-degree
		void findNodePoint();								// find first milestone w/ max out-degree
		void findIndepedentMilestones();					// find independent milestones (out-degree of 0)

		void findAPs();										// find articulation points
		void topoSort();									// do kahn's topological sort

		void criticalPath();								// find critical path in D.A.G.								
		void findSlackTimes();								// find slack time (delay time)
		void findDependencyStats();							// value & vertices of highest & lowest in-degree (excluding source)

		void printGraphInformation();						// print graph info		
		void printGraph();									// print adjMatrix if flag
		void printDependencyStats();						// print dependencies
		void printTopoSort();								// print topological sort
		void printAPs();									// print articulation points
		void printCriticalPath();							// print critical path in D.A.G.
		void printSlackTimes();								// print intervals where slack times exist

	private:
		idxNode	*adjList;									// adjacency list

		bool *aps;											// findAPs() && findAPsHelper()

		int *topoNodes;										// holds path of topological sort
		int *dist; 											// shortest distance for critical path
		int *crPath;										// holds critical path
		int *slackTimes;									// slack time of each eadge
		int *iMilestones;									// independent milestones

		int apsCount;										// count of articulation points
		int topoCount;										// count of topo nodes
		int crPathCount;									// count of critical path
		int slackCount;										// count of tasks w/ slack time
		int iMileCount;										// count of independent milestones

		int vCount;											// vertex count
		int tCount;											// task count

		int srcMile;										// source milestone
		int dstMile;										// destination milestone
		int totalDur;										// total duration (sum of critical path)

		int keyMile;										// key milestone (highest in. vertex)
		int nodePoint; 										// node point (highest out. vertex)
		
		int LIVertex;										// lowest in. vertex
		int LOVertex;										// lowest out. vertex
		int HIVertex;										// highest in. vertex
		int HOVertex;										// highest out. vertex

		int LICount;										// lowest in count
		int LOCount;										// lowest out count
		int HICount;										// highest in count
		int HOCount; 										// highest out count

		string title;										// title of graph

		double tmRatio;										// tasks / milestone ratio
		double tDensity;									// task density

		bool isCycle(int, bool *, bool *);					// checks if cycle exists
		void findAPsHelper(int, bool *, int *, int *, int*);// checks articulation point for non-visited

};
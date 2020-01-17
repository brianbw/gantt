
#include "ganttUtils.h"
#include "linkedQueue.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace std;

/*------------------------------------------------------------
BASIC FUNCTIONS
------------------------------------------------------------*/


// ganttUtils() constructor should initialize class variables to empty state.
ganttUtils::ganttUtils()
{
	topoNodes = dist = crPath = slackTimes = iMilestones = NULL;
	aps = NULL;
	adjList = NULL;
	apsCount = topoCount = crPathCount = slackCount = iMileCount = 0;
	vCount = tCount = srcMile = dstMile = totalDur = keyMile = nodePoint = 0;
	LIVertex = LOVertex = HIVertex = HOVertex = 0;
	LICount = LOCount = HICount = HOCount = 0;
	tmRatio = tDensity = 0.0;
	title = "";
}


// ~ganttUtils() destructor should deallocate all dynamically allocated memory.
ganttUtils::~ganttUtils()
{
	if (adjList != NULL)
	{
		for(int i = 0; i < vCount; i ++)			// for every idx
		{
			lNode *temp = adjList[i].head;			// copy head of LL

			while (temp != NULL)					// traverse LL
			{		
				lNode *removed = temp;				// copy node
				temp = temp->next;					// move to next node
				delete removed;						// delete this node
			}
		}

		delete[] adjList;							// delete whole array
	}

	if (aps != NULL)
		delete[] aps;

	if (topoNodes != NULL)
		delete[] topoNodes;

	if (dist != NULL)
		delete[] dist;

	if (crPath != NULL)
		delete[] crPath;

	if (slackTimes != NULL)
		delete[] slackTimes;

	if (iMilestones != NULL)
		delete[] iMilestones;
}

/*------------------------------------------------------------
INPUT FUNCTIONS
------------------------------------------------------------*/


// getArguments() parses arguments. If no arguments, show usage of “./projectInfo -f <filename>. 
// Ensure file can be opened and read. 1st argument is argument count, 2nd argument is argument vector, 
// 3rd argument is filename, 4th argument is print flag. (true if all arguments are correct, false otherwise)
bool ganttUtils::getArguments(int c, char *v[], string& fileName, bool& printFlag)
{
	if (c == 1)
	{
		cout << " Usage: ./projectInfo -f <filename>." << endl;
		return false;
	}

	if (c != 3 && c !=4)
	{
		cout << "Error, invalid command line arguments." << endl;
		return false;
	}

	bool fArg = false;
	bool pArg = false;

	ifstream in;
	stringstream mySS;

	if (c == 3)		// no printFlag
	{
		for (int i = 1; i < c; i ++)
		{
			if (string(v[i]) == "-f")
			{
				if (fArg)
				{
					cout << "Error, invalid project file name specifier." << endl;
					return false;
				}

				else 
				{
					fArg = true;
					mySS << v[i + 1];
					mySS >> fileName;

					if (mySS.fail() || !mySS.eof())
					{
						cout << "Error, invalid project file name." << endl;
						return false;
					}

					in.open(fileName.c_str());

					if (!in.is_open())
					{
						cout << "Error, can not find project file." << endl;
						return false;
					}

					mySS.clear();
					in.close();
				}
			}
		}

		if (!fArg)
		{
			cout << "Error, invalid project file name specifier." << endl;
			return false;
		}
	}

	else if (c == 4)	// printFlag
	{
		for (int i = 1; i < c; i ++)
		{
			if (string(v[i]) == "-f")
			{
				if (fArg)
				{
					cout << "Error, invalid project file name specifier." << endl;
					return false;
				}

				else 
				{
					fArg = true;
					mySS << v[i + 1];
					mySS >> fileName;

					if (mySS.fail() || !mySS.eof())
					{
						cout << "Error, invalid project file name." << endl;
						return false;
					}

					in.open(fileName.c_str());

					if (!in.is_open())
					{
						cout << "Error, can not find project file." << endl;
						return false;
					}

					mySS.clear();
					in.close();
				}
			}

			if (string(v[i]) == "-p")
			{
				if (pArg)
				{
					cout << "Error, invalid print specifier." << endl;
				}

				else
				{
					pArg = true;
					printFlag = true;
				}
			}
		}

		if (!fArg || !pArg)
		{
			if (!fArg)
			{
				cout << "Error, invalid project file name specifier." << endl;
				return false;
			}

			else if (!pArg)
			{
				cout << "Error, invalid print specifier." << endl;
				return false;
			}
		}
	}

	return true;
}


// readGraph() opens and reads file from passed argument, stores read data into class variables. 
// (true if file can be opened and read, false otherwise)
bool ganttUtils::readGraph(string fileName)
{
	ifstream in;
	int from, to, val, tempCount;
	string trash;

	in.open(fileName);

	if (!in.is_open())
	{
		cout << "Error, can not find project file." << endl;
		return false;
	}

	getline(in, title);			

	if (title[0] == 't')
		title.erase(0, 6);

	if (in.eof())										// file is empty
	{
		cout << "Error, invalid task list." << endl;
		return false;
	}

	in >> trash;										// remove string before vCount
	in >> tempCount;

	in >> trash;										// remove string before source
	in >> srcMile;

	vCount = tempCount;
	initializeValues();

	while (in >> from)									// from A
	{
		in >> to;										// to B
		in >> val;										// value C

		tCount ++;										// increase task count
		adjList[from].outDegree ++;						// increase out degree of A
		adjList[to].inDegree ++;						// increase in degree of B
		adjList[to].topoInDeg ++;						// copy of indegree
		adjList[to].crInDeg ++;							// copy of indegree
		
		lNode *curr = new lNode;						// allocate node

		curr->vertex = to;								// store vertex (neighbor = to)
		curr->weight = val;								// store weight
		curr->next = adjList[from].head;				// set this node's link to next node
		adjList[from].head = curr;						// new head
	}

	vCount = tempCount;									// update vCount
	in.close();
	return true;	
}


// isValidProject() checks the graph data to ensure that it does not contain a path and is directed. 
// (true if graph is directed and acyclic, false otherwise)
// calls isCycle() to look for cycles in the graph.
// ALGORITHM: 
/*
	create bool visited[] & initialize to false
	create bool marked[] & initialize to false
	for every vertex v
		if v is not visited before
			if isCycle(v, visited[], marked[])
				return false
		return true
	delete[] visited
	delete[] marked
*/
bool ganttUtils::isValidProject()
{
	if (adjList == NULL)
		return false; 

	bool *visited = new bool[vCount];			// allocate visited
	bool *marked = new bool[vCount]; 			// allocated marked

	for (int i = 0; i < vCount; i ++)			// initialize to false
	{
		visited[i] = false;
		marked[i] = false;
	}

	for (int v = 0; v < vCount; v ++)			// for every vertex
	{
		if (!visited[v])						// if it is not visited
			if (isCycle(v, visited, marked))	// 	if there is a cycle
			{
				delete[] visited;
				delete[] marked;
				return false;					//	 invalid graph
			}

	}

	delete[] visited;							// deallocate visited
	delete[] marked;							// deallocate marked
	return true;								// invalid graph
}


// isCycle() is a function used in isValidProject() to determine if cycles exists in the graph.
// ALGORITHM: 
/*
	if !visited[v]
		set v as visited[] & set v as marked[]
		for every adjacent u of v
			if visited[u] & isCycle(u, visited[], marked[])
				return true
			else if marked[u]
				return true
	marked[v] = false
	return false
*/
bool ganttUtils::isCycle(int v, bool *visited, bool *marked)	// private
{
	if (!visited[v])											// if v not visited
	{
		visited[v] = true;										// visit now
		marked[v] =  true;										// marked

		lNode *curr = adjList[v].head;							// point to v's 1st neighbor

		while (curr != NULL)			
		{
			int u = curr->vertex;								// u = neighbor

			if (u != -1)
			{
				if (marked[u])									// if visited & has cycle => has cycle
					return true;

				else if (isCycle(u, visited, marked))			// if it was marked previously => has cycle
					return true;
			}

			curr = curr->next;									// update traversal
		}
	}

	marked[v] = false;											// if passed everything, not cycle
	return false;						
}

/*------------------------------------------------------------
COMPUTATIONAL FUNCTIONS (Threadable)
------------------------------------------------------------*/


// getTaskCount() returns the total amount of tasks in the graph.
int ganttUtils::getTaskCount()
{
	return tCount;
}


// initialize arrays of class variables
void ganttUtils::initializeValues()
{
	adjList = new idxNode[vCount];
	aps = new bool[vCount];
	topoNodes = new int[vCount];
	dist = new int[vCount]; 
	crPath = new int[vCount]; 
	slackTimes = new int[vCount];
	iMilestones = new int[vCount];

	for (int i = 0; i < vCount; i ++)
	{
		aps[i] = false;
		topoNodes[i] = 0;
		dist[i] = 0;
		crPath[i] = -1;
		slackTimes[i] = -1;
		iMilestones[i] = -1;
	}
}


// findGraphInformation() calculates the tasks / milestones ratio, and task density ratio.
void ganttUtils::findGraphInformation()
{
	tmRatio = double(tCount) / vCount;
	tDensity = double(2 * tCount) / (vCount * (vCount -1));
}


// findKeyMilestone() finds key milestone. (max in-degree)
void ganttUtils::findKeyMilestone()
{	
	if (adjList == NULL)
		return; 

	int max = 0;											// temp max vertex = 0
	for (int i = 1; i < vCount; i ++)						// start from 1 ~ v
	{
		if (adjList[max].inDegree <= adjList[i].inDegree)	// if indegree is same / more new max (latest)
			max = i;
	}

	keyMile = max;											// vertex 
}


// findNodePoint() finds node point. (max out-degree)
void ganttUtils::findNodePoint()
{	
	if (adjList == NULL)
		return; 

	int max = 0;											// temp max vertex = 0
	for (int i = 1; i < vCount; i ++)						// start from 1 ~ v
	{
		if (adjList[max].outDegree < adjList[i].outDegree)	// if outdegree is more, new max (first)
			max = i;			
	}

	nodePoint = max;										// vertex
}


// findIndependentMilestones() finds independent milestones. (out-degree of 0)
void ganttUtils::findIndepedentMilestones()
{
	if (adjList == NULL)
		return; 

	for (int i = 0; i < vCount; i ++)
	{
		if (adjList[i].outDegree == 0)		// out degree of 0
		{
			iMilestones[iMileCount] = i;	// insert vertex
			iMileCount ++;						
		}
	} 
}


// findAPs() finds the articulation points of the graph. (if articulation point removed, graph disconnects)
// calls findAPsHelper() to find articulation points for non-visited vertices. 
// ALGORITHM: 
/*
	initialize bool aps[] to false
	create bool visited[] & initialize to false
	create int parent[] & initialize to -1
	create int low[] & initialize to 0
	create int disc[] & initialize to 0
	for each vertex v
		if v is not visited
			findAPsHelper(v, visited[], disc[], low[], parent[])

	delete[] visited
	delete[] parent
	delete[] low
	delete[] disc
*/
void ganttUtils::findAPs()
{
	if (adjList == NULL)
		return; 

	bool *visited = new bool[vCount];						// allocation				
	int *parent = new int[vCount];
	int *low = new int[vCount];
	int *disc = new int[vCount];

	for (int i = 0; i < vCount; i ++)						// initialization
	{
		visited[i] = false;
		parent[i] = -1;
		low[i] = 0;
		disc[i] = 0;
	}

	for (int v = 0; v < vCount; v ++)						// if non visited
	{
		if (!visited[v])
			findAPsHelper(v, visited, disc, low, parent);
	}

	delete[] visited;										// deallocation
	delete[] parent;
	delete[] low;
	delete[] disc;
}


// findAPsHelper() is a function used in findAPs() to determine articulation points 
// for unvisited vertices in the graph.
// ALGORITHM: 
/*
	create int children, tempTime & initialize to 0 
	mark current vertex, v, as visited
	initialize discovery order
		disc[v] = low[v] = ++tempTime;
	for every adjacent u of v
		if u is not visited
			children ++
			parent[u] = v
			findAPsHelper(u, visited[], disc[], low[], parent[])
			low[v] = min(low[v], low[u])
			if parent[v] == -1 && children > 1
				aps[v] = true
			if parent[v] != -1 && low[u] >= disc[v]
				aps[v] = true
		else if (v != parent[v])
			low[v] = min(low[v], disc[u])
*/
void ganttUtils::findAPsHelper(int v, bool *visited, int *disc, int *low, int *parent)	// private
{
	int children = 0;											// intialization
	int tTime = 0;

	visited[v] = true;											// visit is true
	disc[v] = low[v] = ++tTime;									// disc & low of v is 1 + time

	lNode *curr = adjList[v].head;								// v's head of LL

	while (curr != NULL)
	{
		int u = curr->vertex;									// v's neighbor

		if (u != -1)
		{
			if (!visited[u])									// if unvisited neighbor
			{	
				children ++;									
				parent[u] = v;
				findAPsHelper(u, visited, disc, low, parent);	// recursion
				low[v] = min(low[v], low[u]);					

				if (parent[v] == -1 && children > 1)
				{
					aps[v] = true;
					apsCount ++;
				}

				if (parent[v] != -1 && low[u] >= disc[v])
				{
					aps[v] = true;
					apsCount ++;
				}
			}

			else if (v != parent[v])
				low[v] = min(low[v], disc[u]);
		}

		curr = curr->next;										// update traversal
	}
}


// topoSort() does topological sorting with the graph data based on in-degrees. 
// ALGORITHM: (USES LINKEDQUEUE)
/*
	initialize inDegree[] to 0
	initialize int topoNodes[] to 0
	initialize topoCount to 0

	compute in-degree for each vertex in graph (store in inDegree[])
	enqueue each vertex with in-degree of 0
	while queue is not empty
		topoNodes[topoCount] = remove vertex from queue
			decrease in-degree by 1 for all of its neighbors
			if in-degree of neighbor is reduced to 0, add to queue
*/
void ganttUtils::topoSort()
{
	if (adjList == NULL)
		return; 

	linkedQueue<int> topoQueue;								// empty queue

	for (int i = 0; i < vCount; i ++)						// enqueue in degree 0
	{
		if (adjList[i].topoInDeg == 0)
			topoQueue.addItem(i);
	}

	while (!topoQueue.isEmptyQueue())						// while not empty queue
	{
		topoNodes[topoCount] = topoQueue.front();			// dequeue
		topoQueue.deleteItem();

		lNode *curr = adjList[topoNodes[topoCount]].head;	// get LL at that dequeued value

		while (curr != NULL)								// traverse LL
		{
			int v = curr->vertex;

			if (v != -1)									// neighbor
			{
				adjList[v].topoInDeg --;					// indegree - 1

				if (adjList[v].topoInDeg == 0)				// if reach 0 
					topoQueue.addItem(v);					// 	enqueue
			}

			curr = curr->next;								// update traverser
		}

		topoCount ++;
	}
}


// criticalPath() finds the critical path of the graph to use as minimum starting time of activities. 
// ALGORITHM: (USES LINKEDQUEUE)
/*
	initialize dist[] to 0
	compute in-degree of each vertex (store in inDegree[])

	//
	determine vertex that has in-degree of 0
	//
		enqueue all vertices w/ in-degree of 0
		create empty queue
		for each vertex v
			if inDegree[v] = 0
				enqueue v 

	dequeue first vertex v from queue
	for each neighbor u of v
		dist[u] = max(dist[u], dist[v] + time(u, v))	

	//
	time(u, v) is time necessary to perform task (u, v) in adjList. 
	Remove v from consideration by decreases in-degree of each neighbor.
	Enqueue any new vertex w/ 0 in-degree.
	Repeat this process until all vertices are processed
	//
		while queue is not empty
			v = dequeued front element
			for each neighbor u of v
				dist[u] = max(dist[u], dist[v] + time(u, v))
				inDegree[u] -= 1
				if inDegree[u] = 0
					enqueue u

	//
	total project duration found from vertex x w/ largest distance is project duration & final task
	re-construct critical path.
	Start w/ vertex x & search for path to starting vertex
	If you're in vertex a, you can only go to vertex b, there's edge(b, a) s.t.
	//
		dist[a] = dist[a] + time(b, a)

	//
	start w/ vertex x (final task) & search for path to starting vertex
	//
		initialize crPath[] to -1
		set crPathCount to 0
		crPath[crPathCount] = x 
		crPathCount ++ 
		while x is not source node (not -1)
			for each vertex v from graph
				for each neighbor u of v
					if x == u
						if dist[x] == dist[v] + time(u, v)
							crPath[crPathCount] = v
							crPathCount ++
							x = v
*/
void ganttUtils::criticalPath()
{
	if (adjList == NULL)
		return;

	linkedQueue<int> crQueue;										// empty queue

	int x = 0;														// destination node

	for (int i = 0; i < vCount; i ++)								// enqueue in degree of 0
	{	
		if (adjList[i].crInDeg == 0)
			crQueue.addItem(i);
	}

	while (!crQueue.isEmptyQueue())									// while queue not empty
	{
		int v = crQueue.front();									// v = index
		crQueue.deleteItem();

		lNode *curr = adjList[v].head;								// get LL

		while (curr != NULL)										// traverse LL
		{
			int u = curr->vertex;									// neighbor

			if (u != -1)											// valid neighbor
			{		
				dist[u] = max(dist[u], dist[v] + curr->weight);		// get max dist

				if (totalDur <= dist[u])
				{
					totalDur = dist[u];
					x = u;
				}

				adjList[u].crInDeg --;								// lower in degree

				if (adjList[u].crInDeg == 0)						// in degree reach 0
					crQueue.addItem(u);								//	enqueue 
			}

			curr = curr->next; 										// update traverser
		}
	}

	dstMile = x;													// store final task
	crPath[crPathCount] = x;										// 1st critical path point
	crPathCount ++;													// increase cr count

	while (x != srcMile)											// when x is not source node
	{
		for (int v = 0; v < vCount; v ++)							// for every vertex
		{
			lNode *curr = adjList[v].head;							// get LL

			while (curr != NULL)									// traverse LL
			{
				int u = curr->vertex;								// neighbor

				if (u != -1)										// valid neighbor
				{
					if (x == u)										// if equal
					{
						if (dist[x] == (dist[v] + curr->weight))	// and distance
						{
							crPath[crPathCount] = v;				// new critical node
							crPathCount ++;							// update count
							x = v;									// x is now that node
						}											// stops when x is source node
					}
				}

				curr = curr->next;									// update traverser
			}
		}
	}
}


// findSlackTimes() finds slack time for each edge / vertex to determine possible delay time 
// based on critical path. 
// ALGORITHM: 
/*
	initialize int slackTimes[] to -1
	set slackTimes[i] to 0 for each i on critical path
	for each (u, v) not on critical path
		slackTimes[u] = dist[v] - (dist[u] + time(u, v))
*/
void ganttUtils::findSlackTimes()
{
	if (adjList	== NULL)					
		return;

	slackCount = vCount - crPathCount;									// non critical path

	for (int i = 0; i < vCount; i ++)									// criticalPath = 0
		if (crPath[i] != -1)
			slackTimes[crPath[i]] = 0;

	for (int v = 0; v < vCount; v ++)									// for every vertex
	{
		if (slackTimes[v] == - 1)										// if that v is not in critical path
		{
			lNode *curr = adjList[v].head;								// get LL

			while (curr != NULL)							
			{
				int u = curr->vertex;									// neighbor

				if (u != -1)											// valid
					slackTimes[v] = dist[u] - (dist[v] + curr->weight);	// add

				curr = curr->next;										// update traverser
			}
		}
	}
}


// findDependencyStats() finds values and vertices of the highest and lowest in-degree nodes 
// excluding source node. 
void ganttUtils::findDependencyStats()
{	
	if (adjList == NULL)
		return;

	LOCount = LICount = HOCount = HICount = 0;

	if (0 != srcMile)
	{
		HIVertex = 0;
		HOVertex = 0;
		LIVertex = 0;
		LOVertex = 0;
	}

	else
	{
		HIVertex = 1;
		HOVertex = 1;
		LIVertex = 1;
		LOVertex = 1;
	}

	HIVertex = keyMile;
	HOVertex = nodePoint;


	for (int i = 0; i < vCount; i ++)
	{
		if (i != srcMile && i != dstMile)
		{
			if (adjList[HIVertex].inDegree < adjList[i].inDegree)
					HIVertex = i;
		
				if (adjList[HOVertex].outDegree < adjList[i].outDegree)
					HOVertex = i;

			if (adjList[LIVertex].inDegree > adjList[i].inDegree)
				LIVertex = i;

			if (adjList[LOVertex].outDegree > adjList[i].outDegree)
				LOVertex = i;
		}
	}

	for (int i = 0; i < vCount; i ++)
	{
		if (adjList[HIVertex].inDegree == adjList[i].inDegree)
			HICount ++;

		if (adjList[HOVertex].outDegree == adjList[i].outDegree)
			HOCount ++;

		if (i != srcMile)
			if (adjList[LIVertex].inDegree == adjList[i].inDegree)
				LICount ++;

		if (i != dstMile)	
			if (adjList[LOVertex].outDegree == adjList[i].outDegree)
				LOCount ++;
	}

}


/*------------------------------------------------------------
OUTPUT FUNCTIONS
------------------------------------------------------------*/


// printGraphInformation() prints graph information with formatted output.
void ganttUtils::printGraphInformation()
{
	string bars = "";
	bars.append(60, '-');
	cout << bars << "\n";
	cout << "Graph Information" << "\n";
	cout << "   " << "Project title: " << title << "\n";
	cout << "   " << "Milestone Count: " << vCount << "\n";
	cout << "   " << "Task Count: " << tCount << "\n";
	cout << "   " << "Source Milestone: " << srcMile << "\n";
	cout << setprecision(6) << fixed;
	cout << "   " << "Tasks/Milestones Ratio: " << tmRatio << "\n";
	cout << "   " << "Project Tasks Density: " << tDensity << "\n";
	cout << "\n";

	cout << "   " << "Key Milestone: " << keyMile <<", in-degree: " << adjList[keyMile].inDegree << " tasks" << "\n";
 	cout << "   " << "Node Point: " << nodePoint <<", out-degree: " << adjList[nodePoint].outDegree << " tasks" << "\n";
 	cout << "   " << "Independent Milestones" << "\n";

 	int tempCount = 0;

 	for (int i = 0; i < vCount; i ++)
 	{
 		if (iMilestones[i] != -1)
 		{
 			cout << " " << iMilestones[i];
 			tempCount ++;

 			if (tempCount == iMileCount)
 				break;
 		}
 	}

 	cout << "\n" << "\n";
}


// printGraph() prints adjacency list with formatted output.
void ganttUtils::printGraph()
{
	string bars = "";
	bars.append(60, '-');	
	cout << bars << "\n";
	cout << "Graph Adjacency List:" << "\n";
	cout << "   " << "Title: " << title << "\n" << "\n";
	cout << "Vertex" << "    " << "vrt /weight | vrt /weight | vrt /weight | ..." << "\n";
	cout << "------" << "    " << "----------------------------------------------" << "\n";

	for (int i = 0; i < vCount; i ++)
	{
		lNode *curr = adjList[i].head;

		cout << setw(6) << right << i << " " << "->"; 

		if (curr != NULL)
		{
			while (curr != NULL)
			{
				int v = curr->vertex;
				int w = curr->weight;

				if (v != -1)
				{
					cout << setw(5) << right << v << "/";
					cout << setw(6) << right << w << " " << "|";
				}

				curr = curr->next;
			}
			cout << " ";
		}

		else
			cout << setw(8) << right << "None";

		cout << "\n";
	}

	cout << "\n";
}


// printDependencyStats() prints dependency statistics with formatted output.
void ganttUtils::printDependencyStats()
{
	string bars = "";
	bars.append(60, '-');	
	cout << bars << "\n";
	cout << "Dependency Statistics (in-degree):" << "\n";
	cout << "   " << "Highest In-Degree: " << adjList[HIVertex].inDegree << "\n";
	cout << "   " << "Lowest In-Degree: " << adjList[LIVertex].inDegree << "\n";
	cout << "   " << "Count of Highest Degree: " << HICount << "\n";
	cout << "   " << "Count of Lowest Degree: " << LICount << "\n";
	cout << "\n";

	cout << "Dependency Statistics (out-degree):" << "\n";
	cout << "   " << "Highest Out-Degree: " << adjList[HOVertex].outDegree << "\n";
	cout << "   " << "Lowest Out-Degree: " << adjList[LOVertex].outDegree << "\n";
	cout << "   " << "Count of Highest Degree: " << HOCount << "\n";
	cout << "   " << "Count of Lowest Degree: " << LOCount << "\n";
	cout << "\n";
}


// printTopoSort() prints topological sort with formatted output.
void ganttUtils::printTopoSort()
{
	string bars = "";
	bars.append(60, '-');	
	cout << bars << "\n";
	cout << "Topological Sort: " << "\n";

	for (int i = 0; i < topoCount; i ++)
		cout << " " << topoNodes[i];

	cout << "\n" << "\n" << "\n";
}


// printAPs() prints graph articulation points with formatted output.
void ganttUtils::printAPs()
{
	string bars = "";
	bars.append(60, '-');	
	cout << bars << "\n";
	cout << "Articulation Points:" << "\n";

	int tempCount = 0;

	for (int i = 0; i < vCount; i ++)
	{
		if (aps[i])
		{
			cout << " " << i;
			tempCount ++;

			if (tempCount == apsCount)
				break;
		}
	}

	cout << "\n" << "\n"; 
}


// printCriticalPath() prints critical path with formatted output.
void ganttUtils::printCriticalPath()
{
	string bars = "";
	bars.append(60, '-');	
	cout << bars << "\n";
	cout << "Critical Path:" << "\n";
	cout << "   " << "Source Node: " << srcMile << "\n";
	cout << "   " << "Final Task: " << dstMile << "\n";
	cout << "   " << "Total Duration: " << totalDur << "\n" << "\n";
	
	cout << "Critical Path: " << "\n";

	for (int i = crPathCount - 1; i >= 0; i --)
		cout << " " << crPath[i];
	 
	cout << "\n" << "\n";
}


// printSlackTimes() prints slack times with formatted output.
void ganttUtils::printSlackTimes()
{
	string bars = "";
	bars.append(60, '-');	
	cout << bars << "\n";
	cout << "Slack Times (task-slacktime):" << "\n";

	int tempCount = 0;

	for (int i = 0; i < vCount; i ++)
	{
		if (slackTimes[i] != 0 && slackTimes[i] != -1)
		{
			cout << " " << i << "-" << slackTimes[i];
			tempCount ++;

			if (tempCount == slackCount)
				break;
		}
	}

	cout << "\n" << "\n";
}

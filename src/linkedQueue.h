
#ifndef H_LINKEDQUEUE
#define H_LINKEDQUEUE

#include <iostream>
#include <iomanip>
#define SIZE 32000

using namespace std;

// node declaration
template <class myType>
struct queueNode
{
	myType dataSet[SIZE];		// array up to 32000 
	int front, back;			// front and back of queue
	queueNode<myType> *link;	// next node
};

// class declaration
template <class myType>
class linkedQueue
{
	public: 
		linkedQueue();							// constructor
		~linkedQueue();							// destructor
		bool isEmptyQueue() const;				// checks if queue is empty
		void initializeQueue();					// initialize new empty queue
		void addItem(const myType& newItem);	// add item to back of queue
		void deleteItem();						// remove item from front of queue (don't ret)
		void printQueue();						// print current elements of queue
		int queueCount();						// return current count of elements in queue
		myType front() const;					// return item from front of queue
		myType back() const;					// return item from back of queue

	private:
		queueNode<myType> *queueFront;			// front pointer
		queueNode<myType> *queueRear;			// rear pointer
		int count;								// number of items on queue (array)
};

// constructor, sets pointers to NULL, values to 0
template <class myType>
linkedQueue<myType>::linkedQueue()
{
	queueFront = NULL;	
	queueRear = NULL;	
	count = 0; 			
}

// destructor, deallocates whole queue, set pointers to NULL
template <class myType>
linkedQueue<myType>::~linkedQueue()
{
	if (!isEmptyQueue())							// if queue has nodes
	{
		queueNode<myType> *curr = queueFront;		// start from front
		while (curr != NULL)						// goes to the end 
		{
			queueNode<myType> *next = curr->link;	// next node 
			delete curr;							// delete this node
			curr = next;							// this node = next node
		}

		queueFront = NULL;							// queue is empty							
		queueRear = NULL; 	
	}							
}

// true if queue is empty, false if not empty
template <class myType>
bool linkedQueue<myType>::isEmptyQueue() const
{
	if (count == 0)		// if there are no items
		return true;	// it is empty

	else
		return false;
}

// create new, empty queue
template <class myType>
void linkedQueue<myType>::initializeQueue()
{
	if (!isEmptyQueue())								// if queue has nodes
	{
		queueNode<myType> *curr = queueFront;			// start from front
		if (queueFront != NULL)
			{
			while (curr != NULL)						// goes to the end 
			{
				queueNode<myType> *next = curr->link;	// next node 
				delete curr;							// delete this node
				curr = next;							// this node = next node
			}
		}

		queueFront = NULL;								// queue is empty							
		queueRear = NULL; 	
	}	

	queueNode<myType> *curr = new queueNode<myType>;	// create new node pointer
	curr->front = 0;									// front is first 
	curr->back = 0;										// rear is first
	curr->link = NULL;									// link is empty
	queueFront = curr;									// only node, front node
	queueRear = curr; 									// only node, back node as well			
	count = 0;											// elements removed
}

// add item to back of queue, create new if empty, go to next node if full
template <class myType>
void linkedQueue<myType>::addItem(const myType& newItem)
{
	if (isEmptyQueue())										// if queue is empty, create a new one, insert item
	{
		queueNode<myType> *curr = new queueNode<myType>;	// create new node pointer 
		curr->front = 0;									// front is first 
		curr->back = 0;										// rear is first
		curr->link = NULL;									// link is empty
		
		queueFront = curr; 									// only node, front node
		queueRear = curr;									// only node, back node as well
		queueFront->dataSet[queueFront->front] = newItem;	// put new item in front (in array)
	}


	else if (count % SIZE == 0)								// if array is full, create a new node
	{
		queueNode<myType> *curr = new queueNode<myType>;	// create new node pointer
		curr->front = 0;									// front is first item of arr
		curr->back = 0;										// back is first item of arr
		curr->link = NULL;									// last node, no next node
		curr->dataSet[curr->front] = newItem;				// put new item in front (in array)

		queueRear->link = curr;								// old last node's next node is this
		queueRear = curr;									// this is now the last node
	}

	else 													// if array is not full, insert in back
	{
		queueRear->back ++;									// last node, next array index	
		queueRear->dataSet[queueRear->back] = newItem;		// insert item in back (in array)
	}

	count ++; // new item added to back of queue
}

// remove item from the front of the queue
template <class myType>
void linkedQueue<myType>::deleteItem()
{
	if (!isEmptyQueue())							// if queue has items
	{
		if (queueFront->front == queueFront->back)	// if array only has 1 item 
		{
			queueNode<myType> *curr = queueFront;	// copy front node 
			queueFront = queueFront->link;			// go to next node (detached old front)
			delete curr;							// deallocate old front node
			count --;								// 1 less item

		}

		else 										// if queue has more than 1 item
		{
			queueFront->front++;					// front node's array, go to next (detached)
			count --; 								// 1 less item 
		}
	}
}

// print all items in queue
template <class myType>
void linkedQueue<myType>::printQueue()
{	
	queueNode<myType> *curr = queueFront;			// copy front node

	while (curr != NULL)							// iterate through whole queue
	{	
		for (int i = 0; i <= curr->back; i++)		// print every element in the array
		{
			cout << setw(5) << right;
			cout << curr->dataSet[i] << " ";
			if ((i + 1) % 10 == 0)
				cout << endl;
		}
		curr = curr->link;							// next array/node
	}
	cout << endl;
}

// return count of items in queue (arrays)
template <class myType>
int linkedQueue<myType>::queueCount()
{
	return count; 	
}

// return first item from first node
template <class myType>
myType linkedQueue<myType>::front() const
{
	return queueFront->dataSet[queueFront->front];
}

// return last item from last node 
template <class myType>
myType linkedQueue<myType>::back() const
{
	return queueRear->dataSet[queueRear->back]; 
}

#endif
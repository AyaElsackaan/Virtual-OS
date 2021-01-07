#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>


typedef struct node_p { 
    int id; 
    int priority; // Lower values -> higher priority 
    int runningTime;
  
} Node_priority; 
  
typedef struct node_c {
	int data;
	struct node_c* next;
	int runningTime;
}Node_circular;


//get parent
int parent(int i) 
{ 
	return (i - 1) / 2; 
} 

//get left child
int leftChild(int i) 
{ 
	return ((2 * i) + 1); 
} 

//get right child
int rightChild(int i) 
{ 

	return ((2 * i) + 2); 
} 

//move a node up in the heap to keep the representation invariant
void reheapUp(int i, Node_priority ** H) 
{ 
	//compare node to parent
	while (i > 0 && H[parent(i)]->priority > H[i]->priority) 
	{ 
		// Swap  
		Node_priority* temp=H[parent(i)];
		H[parent(i)]= H[i];
		H[i]=temp;
 		
 		//move up
		i = parent(i); 
	} 
} 

//move a node down in the heap to keep the representation invariant
void reheapDown(int i, Node_priority **H,int * size) 
{ 
	//get min of node &its 2 children
	int minIndex = i; 

	int l = leftChild(i); 

	if (l <= *size && H[l]->priority < H[minIndex]->priority) 
		minIndex = l; 

	int r = rightChild(i); 

	if (r <= *size && H[r]->priority < H[minIndex]->priority) 
		minIndex = r; 

	if (i != minIndex) 
	{ 
		//swap
		Node_priority* temp=H[minIndex];
		H[minIndex]= H[i];
		H[i]=temp;
 
		reheapDown(minIndex,H, size); 
	} 
} 

//add a node to the heap
void enqueue_priority(int p, int ID, Node_priority **H, int * size, int rTime) 
{ 
	Node_priority* temp = (Node_priority*)malloc(sizeof(Node_priority)); 
    temp->id = ID; 
    temp->priority = p; 
    temp->runningTime = rTime;
    
	*size = *size + 1; 
	H[*size] = temp; 

	reheapUp(*size,H); 
	printf("enqueued :%d\n",ID);
} 

//remove the min (highest priority)
Node_priority* dequeue_priority(Node_priority **H, int *size) 
{ 
	Node_priority* temp=H[0];
	Node_priority* result = H[0]; 

	// Replace the value at the root 
	// with the last leaf 
	H[0] = H[*size]; 
	*size = *size - 1; 

	reheapDown(0, H, size); 
	return result; 
} 

//change priority of node at i -> for SRTN 
void updatePriority(int i, int p, Node_priority **H, int * size) 
{ 
	int oldp = H[i]->priority; 
	H[i]->priority = p; 

	if (p < oldp) 
		reheapUp(i, H); 
	else 
		reheapDown(i,H, size); 
} 

//int peek_shortestTime(Node_priority * f) 
//{ 
//    return f->runningTime; 
//} 

/////////////////////////////////////Circular Queue for round robin////////////////////////////////////////


void enqueue_circular(int d, Node_circular **f, Node_circular **r,int rTime) //Insert elements in Queue
{
	Node_circular* n = (Node_circular*)malloc(sizeof(Node_circular));
	n->data = d;
	n->runningTime = rTime;
	n->next = NULL;
	if((*r==NULL)&&(*f==NULL))
	{
		*f = *r = n;
		(*r)->next = *f;
	}
	else
	{
		(*r)->next = n;
		*r = n;
		n->next = *f;
	}
} 
void dequeue_circular(Node_circular **f, Node_circular **r) // Delete an element from Queue
{
	Node_circular* t;
	t = *f;
	if((*f==NULL)&&(*r==NULL))
		printf("\nQueue is Empty");
	else if(*f == *r){
		*f = *r = NULL;
		free(t);
	}
	else{
		*f = (*f)->next;
		(*r)->next = *f;
		free(t);
	}
	
}

void rotate(Node_circular **f, Node_circular **r) // moves first element in queue to the end of queue
{
	Node_circular* t;
	t = *f;
	if((*f==NULL)&&(*r==NULL))
		printf("\nQueue is Empty");
	else if(*f == *r){
		*f = *r = NULL;
		enqueue_circular(t->data, f, r, t->runningTime);
		free(t);
		
	}
	else{
		*f = (*f)->next;
		(*r)->next = *f;
		enqueue_circular(t->data, f, r, t->runningTime);
		free(t);
	}
	
}

int peek_runningTime(Node_circular * f) 
{ 
    return f->runningTime; 
} 




#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
//////////////////////////Priority queue//////////////////

typedef struct node_p { 
    int id; 
    int priority; // Lower values -> higher priority 
    int runningTime;
    int memory_size;
  
} Node_priority; 
  

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
void enqueue_priority(int p, int ID, Node_priority **H, int * size, int rTime, int mem) 
{ 
	Node_priority* temp = (Node_priority*)malloc(sizeof(Node_priority)); 
    temp->id = ID; 
    temp->priority = p; 
    temp->runningTime = rTime;
    temp->memory_size=mem;
    
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
typedef struct node_c {
	int id;
	struct node_c* next;
	int runningTime;
	int memory_size;
}Node_circular;

struct Queue_c
{
  Node_circular *front;
  Node_circular *rear;
};

struct Queue_c* create_Queue_c()
{
  struct Queue_c* q = (struct Queue_c*)malloc(sizeof(struct Queue_c));
  q->front = NULL;
  q->rear = NULL;
   
  return q;
} 

void enqueue_circular(struct Queue_c* q, int d, int rTime, int mem)
{
  Node_circular* n = (Node_circular*)malloc(sizeof(Node_circular));
  n->id = d;
  n->runningTime = rTime;
  n->memory_size=mem;
  n->next = NULL;
  if (q->rear ==NULL)  //queue is empty
  {
    q->front = n;
    q->rear = n;
    q->rear->next = q->front;
  }
  else
  {
    q->rear->next = n;
    q->rear = n;
    q->rear->next = q->front;
  }
}


void dequeue_circular(struct Queue_c* q) // Delete an element from Queue
{
	Node_circular* t;
	t = q->front;
	if((q->front==NULL)&&(q->rear==NULL))
		printf("\nQueue is Empty");
	else if(q->front == q->rear){
		q->front= q->rear = NULL;
		free(t);
	}
	else{
		q->front = q->front->next;
		q->rear->next = q->front;
		free(t);
	}
	
}

void rotate(struct Queue_c* q) // moves first element in queue to the end of queue
{
	Node_circular* t;
	t = q->front;
	if((q->front==NULL)&&(q->rear==NULL))
		printf("\nQueue is Empty");
	else if(q->front == q->rear){
		q->front = q->rear = NULL;
		enqueue_circular(q, t->id, t->runningTime,t->memory_size);
		free(t);
		
	}
	else{
		q->front = q->front->next;
		q->rear->next = q->front;
		enqueue_circular(q, t->id, t->runningTime,t->memory_size);
		free(t);
	}
	
}

Node_circular* peek_runningTime(struct Queue_c* q) 
{ 
    return q->front; 
} 

int isempty_priority(int size)
{ 
 return (size==-1);
}

int isempty_circular(struct Queue_c* q)
{ 
 return ((q->front==NULL)&&(q->rear==NULL));
}

////////////////////////////Waiting List for memory (linked list)/////////
typedef struct queue_ll
{
  Node_circular *front;
  Node_circular *rear;

}WaitingList;


WaitingList* create_Waiting_List()
{
  WaitingList* q = (WaitingList*)malloc(sizeof(WaitingList));
  q->front = NULL; 
  q->rear=NULL; 
  return q;
} 

void enqueue_waitingist(WaitingList* q, int d, int rTime, int mem)
{
  Node_circular* n = (Node_circular*)malloc(sizeof(Node_circular));
  n->id = d;
  n->runningTime = rTime;
  n->memory_size=mem;
  n->next = NULL;
  if (q->front ==NULL)  //queue is empty
  {
    q->front = n;
    q->rear=n;
  }
  else
  {
    q->rear->next = n;
    q->rear = n;
  }
}

void RemoveFromList(WaitingList* f, Node_circular* node_before_me)
{
	if(node_before_me==NULL)  //remove first node
	{
		Node_circular* temp=f->front;
		f->front=temp->next;
		free(temp);
	}
	else 
	{
		Node_circular* temp= node_before_me->next;
		if(temp->next==NULL) //the last node
		{
			f->rear=node_before_me;
			node_before_me->next=NULL;
			free(temp);
		}
		else
		{
			node_before_me->next=temp->next;
			free(temp);
		}
	}
}



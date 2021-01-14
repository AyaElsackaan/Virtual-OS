#include "headers.h"
#define MAX_MEMORY 1024

typedef struct memorynode
{
	char type; //P or H
	int starts_at;
	int size;
	struct memorynode* next;
}MemoryNode;

typedef struct memorymap
{
	MemoryNode * map_head;
}MemoryMap;

MemoryNode* createMap()
{
	MemoryNode* n = (MemoryNode*)malloc(sizeof(MemoryNode));
	n->type='H';
	n->starts_at=0;
	n->size=MAX_MEMORY;
	n->next= NULL;
}
MemoryNode* getBlock(MemoryMap * memmap, int s)
{
	MemoryNode*required_node=memmap->map_head;
	int min_possible_size=MAX_MEMORY+1; //more than 1024
	while(required_node!=NULL)
	{
		if(required_node->type=='H' && required_node->size>=s && required_node->size<min_possible_size) //hole of suitable size
		{
			min_possible_size= required_node->size;
		}
		required_node=required_node->next;
	}
	if(min_possible_size==MAX_MEMORY+1) //no node found
		return NULL;
	return required_node;

}

void addNodeAfter (MemoryNode* node_before_me, char t, int st, int s)
{
	  MemoryNode* n = (MemoryNode*)malloc(sizeof(MemoryNode));
	  n->type=t;
	  n->starts_at=st;
	  n->size=s;
	  n->next= node_before_me->next;
	  node_before_me->next=n;
}
void removeNodeAfter(MemoryNode* node_before_me)
{
	MemoryNode* temp= node_before_me->next;
	node_before_me->next=temp->next;
	free(temp);
}

void getNeededSize(MemoryNode*block,int proc_size)
{
	if(block->size<proc_size)
	{
		//merge &assign
		block->size=(block->size)*2;
		removeNodeAfter(block);
		block->type='P';
		return ;
		
	}
	
	//divide
	block->size=size/2;
	addNodeAfter (block, 'H', start_at+(size/2), size/2);	
	getNeededSize(block, proc_size);
}

bool allocate(MemoryMap * memmap,int proc_size)
{
	MemoryNode* block= getBlock(memmap, proc_size);
	if (block==NULL) 
		return false; //can't allocate
		
	 getNeededSize(block,proc_size);
	return true;
}



int main()
{

int* mem_map= (int*) malloc(1024*sizeof(int));
for (int i=0; i<1024; i++)
{
	mem_map[i]=0;
}

printf("%d\n",getNeededSize(1024,20));
	return 0;
}

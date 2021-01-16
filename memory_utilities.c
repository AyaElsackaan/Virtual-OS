#include "headers.h"
#define MAX_MEMORY 1024

typedef struct memorynode
{
	char type; //P or H
	int starts_at;
	int size;
	struct memorynode* next;
	struct memorynode* prev;
}MemoryNode;

typedef struct memorymap
{
	MemoryNode * map_head;
}MemoryMap;

MemoryMap* createMap()
{
	MemoryMap* M = (MemoryMap*)malloc(sizeof(MemoryMap));
	M->map_head = (MemoryNode*)malloc(sizeof(MemoryNode));
	M->map_head->type='H';
	M->map_head->starts_at=0;
	M->map_head->size=MAX_MEMORY;
	M->map_head->next= NULL;
	M->map_head->prev= NULL;
	return M;
}
MemoryNode* getBlock(MemoryMap * memmap, int s)
{
	MemoryNode*required_node=memmap->map_head;
	MemoryNode*traversal_node=memmap->map_head;
	int min_possible_size=MAX_MEMORY+1; //more than 1024
	
	while(traversal_node!=NULL)
	{
		if(traversal_node->type=='H' && traversal_node->size>=s && traversal_node->size<min_possible_size) //hole of suitable size
		{
			required_node=traversal_node;
			min_possible_size= required_node->size;
		}
		traversal_node=traversal_node->next;
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
	  n->prev=node_before_me;
	  if(n->next!=NULL) //node before me was the last node
	  	n->next->prev=n;
	  
}
void removeNodeAfter(MemoryNode* node_before_me)
{
	MemoryNode* temp= node_before_me->next;
	node_before_me->next=temp->next;
	temp->next->prev=node_before_me;
	free(temp);
}

void getNeededSize(MemoryNode*block,int proc_size)
{
	if(block->size < proc_size)
	{
		//merge &assign
		block->size=(block->size)*2;
		removeNodeAfter(block);
		block->type='P';
		return ;
		
	}
	
	//divide
	block->size=block->size/2;
	addNodeAfter (block, 'H', block->starts_at+(block->size), block->size);	
	getNeededSize(block, proc_size);
}

bool allocate(MemoryMap * memmap,int proc_size)
{
	MemoryNode* block= getBlock(memmap, proc_size);
	if (block==NULL) 
	{
		printf("false\n");
		return false; //can't allocate
	}	
	printf("true\n");
	 getNeededSize(block,proc_size);
	return true;
}
/*
void Merge(MemoryNode* block)
{
	if(block->size==MAX_MEMORY)
		return;
		
	if(block->starts_at % (2*block->size)) //merge with next
	{
		if(block->next->type=='H')
		{
			block->size=2*block->size;
			removeNodeAfter(block);
			Merge(block);
		}
	}
	else //merge with prev
	{
		if(block->prev->type=='H')
		{
			block->prev->size=2*block->prev->size;
			removeNodeAfter(block->prev);
			Merge(block->prev);
		}	
	}

}

void deallocate(MemoryNode* block)
{
	block->type='H';
	Merge(block);
}
*/

void PrintMemory(MemoryMap* M)
{
	MemoryNode* temp=M->map_head;
	int i=0;
	while(temp!=NULL)
	{
		printf("Node %d type:%c, start:%d, size:%d\n",i,temp->type,temp->starts_at,temp->size);
		temp=temp->next;
		i++;
	}
}

int main()
{
	MemoryMap* Mem_Map= createMap();
	printf("created\n");
	//addNodeAfter (Mem_Map->map_head, 'P', 1, 10);
	PrintMemory(Mem_Map);
	if(allocate(Mem_Map,70))
	{	printf("\nsuccess1\n");
		PrintMemory(Mem_Map);
	}	
	if(allocate(Mem_Map,35))
	{	printf("\nsuccess2\n");
		PrintMemory(Mem_Map);
	}	
	if(allocate(Mem_Map,80))
	{	printf("\nsuccess3\n");
		PrintMemory(Mem_Map);
	}	

	return 0;
}

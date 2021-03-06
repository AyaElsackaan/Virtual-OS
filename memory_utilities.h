#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

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
	if(temp->next!=NULL) //node before me is the last node
		temp->next->prev=node_before_me;
	free(temp);
}

MemoryNode* getNeededSize(MemoryNode*block,int proc_size)
{
	if(block->size < proc_size)
	{
		//merge &assign
		block->size=(block->size)*2;
		removeNodeAfter(block);
		block->type='P';
		return block;
		
	}
	
	//divide
	block->size=block->size/2;
	addNodeAfter (block, 'H', block->starts_at+(block->size), block->size);	
	return getNeededSize(block, proc_size);
}

MemoryNode* allocate(MemoryMap * memmap,int proc_size)
{
	MemoryNode* block= getBlock(memmap, proc_size);
	if (block==NULL) 
		return NULL; //can't allocate
			
	return getNeededSize(block,proc_size);
}

void Merge(MemoryNode* block)
{
	if(block->size==MAX_MEMORY)
	{
		return;
	}	
	if(block->starts_at % (2*block->size)==0) //merge with next
	{
		if(block->next->type=='H'&&block->next->size==block->size)
		{
			block->size=2*block->size;
			removeNodeAfter(block);
			Merge(block);
		}
	}
	else //merge with prev
	{
		if(block->prev->type=='H'&&block->prev->size==block->size)
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

/* ////for testing////
int main()
{
	MemoryMap* Mem_Map= createMap();
	printf("created\n");
	//addNodeAfter (Mem_Map->map_head, 'P', 1, 10);
	PrintMemory(Mem_Map);
	
	MemoryNode* A= allocate(Mem_Map,70);
	if(A!=NULL)
	{	printf("\nsuccess1\n");
		PrintMemory(Mem_Map);
	}
	
	MemoryNode* B=allocate(Mem_Map,35);	
	if(B!=NULL)
	{	printf("\nsuccess2\n");
		PrintMemory(Mem_Map);
	}
	
	MemoryNode* C=allocate(Mem_Map,80);	
	if(C!=NULL)
	{	printf("\nsuccess3\n");
		PrintMemory(Mem_Map);
	}
	
	MemoryNode* D=allocate(Mem_Map,1024);  //no enough space
	if(D==NULL)
	{	printf("\nfailed\n");
		PrintMemory(Mem_Map);
	}	
	
	deallocate(A);
	printf("\nsuccess\n");
	PrintMemory(Mem_Map);
	
	D=allocate(Mem_Map,60);
	if(D!=NULL)
	{	printf("\nsuccess\n");
		PrintMemory(Mem_Map);
	}
		
	deallocate(B);
	printf("\nsuccess\n");
	PrintMemory(Mem_Map);
	
	deallocate(D);
	printf("\nsuccess\n");
	PrintMemory(Mem_Map);
	
	deallocate(C);
	printf("\nsuccess\n");
	PrintMemory(Mem_Map);
	return 0;
}*/

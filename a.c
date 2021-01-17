#include "data_structures.h"

int main()
{
	/*WaitingList*w= create_Waiting_List();
	enqueue_waitingist(w, 0, 0, 0);
	enqueue_waitingist(w, 1, 0, 0);
	enqueue_waitingist(w, 2, 0, 0);
	
	Node_circular*temp=w->front;
	
	if(temp->id==2)
		RemoveFromList(w, NULL);
	else{
		//temp=temp->next;
		while(temp->next!=NULL)
		{
			if(temp->next->id==2)
			{
				RemoveFromList(w, temp);
				printf("removed\n");
			}
			if(temp->next!=NULL)
				temp=temp->next;
			else
				break;
		}
	}
	
	PrintWaitingList(w);*/
	
	struct Queue_c* q= create_Queue_c();
	
	Node_circular* n = (Node_circular*)malloc(sizeof(Node_circular));
  n->id = 0;
  n->runningTime = 0;
  n->memory_size=0;
	enqueue_at_front(q,n);
	
	 n = (Node_circular*)malloc(sizeof(Node_circular));
  n->id = 1;
  n->runningTime = 1;
  n->memory_size=1;
	enqueue_at_front(q,n);
	
	n = (Node_circular*)malloc(sizeof(Node_circular));
  n->id = 2;
  n->runningTime = 2;
  n->memory_size=2;
	enqueue_at_front(q,n);
	
	n=q->front;
	
	while(n)
	{
		printf("%d\n",n->id);
		n=n->next;
	}
	return 0;
}	
//////////////////////////////////////////////////////
	  	//check waiting list
		      Node_circular* temp=w_list->front; //iterator
		      
		      //for first element 
		      int wl_index=binarySearch(id,0,index_p-1,temp->id);
			  memory_location[wl_index]=allocate(Mem_Map,temp->memory_size);	
				if(memory_location[wl_index]!=NULL)
				{
					enqueue_at_front(q,temp); //add to front of ready queue
					RemoveFromList(w_list, NULL); //remove first element
				}
				else //check rest of waiting list
				{
					while(temp->next!=NULL)
					{
						wl_index=binarySearch(id,0,index_p-1,temp->id);
						memory_location[wl_index]= allocate(Mem_Map,temp->memory_size);	
						if(memory_location[wl_index]!=NULL)
						{
enqueue_at_front(q,temp); //add to front of ready queue							
RemoveFromList(w_list, temp);//remove first element
break;
							
							
							//printf("removed\n");
						}
						
						temp=temp->next;
					}
				}///end of waiting list check
	


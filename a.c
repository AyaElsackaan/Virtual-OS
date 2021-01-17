#include "data_structures.h"

int main()
{
	WaitingList*w= create_Waiting_List();
	enqueue_waitingist(w, 0, 0, 0);
	enqueue_waitingist(w, 1, 0, 0);
	enqueue_waitingist(w, 2, 0, 0);
	
	Node_circular*temp=w->front;
	
	if(temp->id==2)
		RemoveFromList(w, NULL);
	else{
		//temp=temp->next;
		while(temp->next!=NULL)
		{//printf("4\n");
			if(temp->next->id==2)
			{//printf("4\n");
				RemoveFromList(w, temp);
			}
			temp=temp->next;
		}
	}
	
	PrintWaitingList(w);
	return 0;
	
	
}

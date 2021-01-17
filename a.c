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
	
	PrintWaitingList(w);
	return 0;
	
	
}

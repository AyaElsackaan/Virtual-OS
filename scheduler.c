#include "headers.h"
#include "data_structures.h"

void clearResources(int);
void HPF();
/*arguments:
 * 	1:algo 
 *	2:time slot
 *	3: 
 */
 int msgq_ready,timeslot,n; 
int main(int argc, char * argv[])
{
	signal(SIGINT, clearResources);
	initClk();
printf("\n started\n\n");printf("\n arguments=%d \n\n",argc);
	//TODO implement the scheduler :) 
	
	//command line arguments
	int algo= atoi(argv[1]);
     timeslot=atoi(argv[2]);
     n=atoi(argv[3]);
    printf("%d %d %d\n",algo, timeslot, n);
    
	//get msg queue
	key_t ready_id; 

    ready_id = ftok("keyfile", 'R');  //unique
    
    msgq_ready = msgget(ready_id, 0666 | IPC_CREAT);
    if (msgq_ready == -1)
    {
        perror("Error in creating ready");
        exit(-1);
    }
    printf("Message Queue ID  (ready) sched= %d\n", msgq_ready);
    
    if(algo==1)
    {
    	HPF();
    }
    /*if (algo == 3) //RR
    {
      Node_circular *ready_front=NULL;
      Node_circular *ready_rear=NULL;
    }
    else //HPF or SRTN
    {
      Node_priority *ready = (Node_priority*)malloc(sizeof(Node_priority)*n);
    }*/

	//read from msg queue
	/*while(true)
	{
		P_msgbuff newProcess;
	
		int rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);

		if (rec_val == -1)
		   perror("Error in receive");
		else
			printf("\nMessage received from server: %d\n", newProcess.id);
				    //add to data structure//enqueue
				    
				    //if !flag dequeue
				    //for
				    
				    
				    
	}*/
	
	
	
    
	//every clk--> if no one working :dequeue
	//fork new process
	//wait 
	//repeat till msg queue is empty
	 
    //PCB : id, run time, remainig, state, waiting
    int id[n];
    int arrival[n];
    int run[n];
    int priority[n];
    int waiting_time[n]; //start-arrival
    int remaining_time[n];
    char status[n]; //R:running, T:terminated, P: paused
    

     
 	    
    
    //upon termination release the clock resources.
    
    destroyClk(true);
}
Node_priority **ready;
void HPF()
{
	int CPU_busy=0;
	ready = (Node_priority**)malloc(sizeof(Node_priority*)*n);
	int size=-1;
	while(true)
	{
		P_msgbuff newProcess;
	
		int rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);

		if (rec_val == -1)
		   perror("Error in receive");
		else
			printf("\nMessage received from server: %d\n", newProcess.id);
		//add to data structure
		//enqueue
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run) ;
		//printf("id: %d",ready[0]->id);
			    
		//if CPUs not busy dequeue ->no one executing
				    //for
				    
				    
				    
	}
}

void clearResources(int signum)
{
  /*for (int i=0; i<3; i++)
   	printf("id: %d\n",ready[i]->id);*/
   	kill(getpid(),SIGKILL);
   	
}

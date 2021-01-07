#include "headers.h"
#include "data_structures.h"
/*arguments:
 * 	1:algo 
 *	2:time slot
 *	3: 
 */
int main(int argc, char * argv[])
{
	initClk();
printf("\n started\n\n");printf("\n arguments=%d \n\n",argc);
	//TODO implement the scheduler :) 
	
	//command line arguments
	int algo= atoi(argv[1]);
    int timeslot=atoi(argv[2]);
    int n=atoi(argv[3]);
    printf("%d %d %d\n",algo, timeslot, n);
    
	//get msg queue
	key_t ready_id; 

    ready_id = ftok("keyfile", 'R');  //unique
    
    int msgq_ready = msgget(ready_id, 0666 | IPC_CREAT);
    if (msgq_ready == -1)
    {
        perror("Error in creating ready");
        exit(-1);
    }
    printf("Message Queue ID  (ready) sched= %d\n", msgq_ready);

	//read from msg queue
	while(true)
	{
		P_msgbuff newProcess;
	
		int rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);

				if (rec_val == -1)
				    perror("Error in receive");
				else
				    printf("\nMessage received from server: %d\n", newProcess.id);
	}
	
	//add to data structure
	
    if (algo == 3) //RR
    {
      Node_circular *ready_front=NULL;
      Node_circular *ready_rear=NULL;
    }
    else //HPF or SRTN
    {
      Node_priority *ready = (Node_priority*)malloc(sizeof(Node_priority)*n);
    }
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

#include "headers.h"
#include "data_structures.h"
#include <errno.h>
extern int errno ;

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
int msgq_busy;
char*str;
void HPF()
{

 	key_t key_busy;
    

	//create/get message queue
    key_busy = ftok("keyfile", 'B');  //unique
    
    msgq_busy = msgget(key_busy, 0666 | IPC_CREAT); 
    if (msgq_busy == -1)
    {
        perror("Error in creating busy");
        exit(-1);
    }
    printf("Message Queue ID  (busy)= %d\n", msgq_busy);
    
    int CPU_busy=0;
    
    int send_val = msgsnd(msgq_busy, &CPU_busy, sizeof(CPU_busy), !IPC_NOWAIT); 

		if (send_val == -1)
		    perror("Error in send");
		printf("busy=%d\n",CPU_busy);

	//struct msgbuff msg_sent, msg_received;

	/////shared memory and semaphore sets for CPU_busy
	
	/*int key_id_busy = ftok("keyfile", 'B');
	int key_id_s1 = ftok("keyfile", 'w');
	int key_id_s2 = ftok("keyfile", 'r');
	int busyid = shmget(key_id_busy, 256, IPC_CREAT | 0666);
	if (busyid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID busy= %d\n", busyid);
        
	//create 2 semaphore sets 
	int sem1 = semget(key_id_s1, 1, 0666 | IPC_CREAT);
    int sem2 = semget(key_id_s2, 1, 0666 | IPC_CREAT);

    if (sem1 == -1 || sem2 == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    printf("\nsemaphore sets = %d, %d\n", sem1, sem2);
    
    //initialize semaphore sets
    union Semun semun;
    semun.val = 0; 
    if (semctl(sem1, 0, SETVAL, semun) == -1 || semctl(sem2, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    int *busyaddr = (int*)shmat(busyid, (void *)0, 0); //attach shred memo
	if ((long)busyaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	(*busyaddr)=0; //CPU is not busy
	*/
	////
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
		if(!CPU_busy)
		{
		//dequeue
		Node_priority* dequeued_proc= dequeue_priority(ready, &size); 
		///fork
		
		int pid=fork();
      	if (pid==-1)
     	{
      		perror("couldn't fork process of id \n");
      		exit(-1);
		}
		else if (pid==0) //parent 
     	{
      		printf("parent: next cycle\n");
      		/*int x, exit_code;
      		wait(&x);
      		if(!(x & 0x00FF))
      		{  //terminated normally
  	    		exit_code=x>>8; //getting exit code
  	    		printf("exit %d\n",exit_code);
  	    	}*/
		   int rec_val = msgrcv(msgq_ready, &CPU_busy, sizeof(CPU_busy), 0, IPC_NOWAIT);
				if (rec_val == -1 && errno==ENOMSG)
				    printf("not changed yet\n");
      		
		}
		else //process (child)
     	{
      		perror("started process of id \n");
      		int length = snprintf( NULL, 0, "%d", dequeued_proc->runningTime );
			str = malloc( length + 1 );
			snprintf( str, length + 1, "%d", dequeued_proc->runningTime );
      		execlp("./process.out", "process.out", NULL);
		}
				    
		}		    
				    
				    
	}
}

void clearResources(int signum)
{
  /*for (int i=0; i<3; i++)
   	printf("id: %d\n",ready[i]->id);*/
   	free(str);
   	msgctl(msgq_busy, IPC_RMID, 0);
   	kill(getpid(),SIGKILL);
   	
}

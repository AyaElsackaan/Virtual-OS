#include "headers.h"
#include "data_structures.h"
#include <errno.h>
#include "memory_utilities.h"
extern int errno ;

//void clearResources(int);
//void processTerm(int);
void HPF();
void RR(int t_slot);
void SRTN();

int msgq_ready,timeslot,n;
int msg_changed;
int start_time;
  //PCB : id, run time, remainig, state, waiting
    int* id;
    int *pidarr;
    int* arrival;
    int* run;
    int* priority;
    int* waiting_time; //start-arrival
    int* remaining_time;
    char* status; //R:running, T:terminated, P: paused
int index_p=0;
int *busyaddr;
int busyid;

int*  start_wait;
    
int *stat_id;  
char **stataddr;

//int *remaddr;
//int remid;

int *rem_id;  
int **remaddr;

int *wait_id;  
int **waitaddr;

Node_priority **ready;
char*str,*str2,*str3,*str4,*str5;
double* WTA;
int* start_time_RR;
int* start_time_SRTN;
int sem1; 
Node_running* dequeuedS;
Node_running **readySRTN;
MemoryMap* Mem_Map; //keeps track of free & allocated memory blocks
/*arguments:
 * 	1:algo 
 *	2:time slot
 *	3:number of processes (for PCB)
 */ 
int main(int argc, char * argv[])
{
	//signal(SIGINT, clearResources);
	//signal(SIGUSR1, processTerm);
	initClk();
printf("\n started\n\n");printf("\n arguments=%d \n\n",argc);
	
	//command line arguments
	int algo= atoi(argv[1]);
     timeslot=atoi(argv[2]);
     n=atoi(argv[3]);
     
     //arrays
    id=(int*)malloc(n*sizeof(int));
    pidarr=(int*)malloc(n*sizeof(int));
    arrival=(int*)malloc(n*sizeof(int));
    run=(int*)malloc(n*sizeof(int));
    priority=(int*)malloc(n*sizeof(int));
    waiting_time=(int*)malloc(n*sizeof(int)); //start-arrival
    remaining_time=(int*)malloc(n*sizeof(int));
    status=(char*)malloc(n*sizeof(char));
    stat_id=(int*)malloc(n*sizeof(int));
    stataddr=(char**)malloc(n*sizeof(char*));
    rem_id=(int*)malloc(n*sizeof(int));
    remaddr=(int**)malloc(n*sizeof(int*));
    wait_id=(int*)malloc(n*sizeof(int));
    waitaddr=(int**)malloc(n*sizeof(int*));
    start_wait=(int*)malloc(n*sizeof(int));
    start_time_RR=(int*)malloc(n*sizeof(int));
    start_time_SRTN=(int*)malloc(n*sizeof(int));
    WTA=(double*)malloc(n*sizeof(double));
    printf("%d %d %d\n",algo, timeslot, n);
  
	//create the memory map
	Mem_Map= createMap();  
  
	//get msg queue
	key_t ready_id; 

    ready_id = ftok("keyfile", 'R');  //unique
 //msg queue to receive from process generator   
    msgq_ready = msgget(ready_id, 0666 | IPC_CREAT);
    if (msgq_ready == -1)
    {
        perror("Error in creating ready");
        exit(-1);
    }
    printf("Message Queue ID  (ready) sched= %d\n", msgq_ready);
    
 //shared memory busyid//   
    int key_id_busy = ftok("keyfile", 'B');
	busyid = shmget(key_id_busy, sizeof(int), IPC_CREAT | 0666);
	if (busyid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID busy= %d\n", busyid);
        
        busyaddr = (int*)shmat(busyid, (void *)0, 0); //attach shred memo
	if ((long)busyaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
//---------------------------------------------------------------//
    //shared memory status//
    for (int i=0; i<n; i++)
    {
		int key_id_stat = ftok("keyfile", i+1);
		stat_id[i] = shmget(key_id_stat, sizeof(char), IPC_CREAT | 0666);
	if (stat_id[i] == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID status= %d, index(i)= %d\n", stat_id[i],i);
        
        stataddr[i] = (char*)shmat(stat_id[i], (void *)0, 0); //attach shared memory
	if ((long)stataddr[i] == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	*(stataddr[i])='W';
     }
	//(*stataddr)=status;
   
   //shared memory remaining time//
    for (int i=0; i<n; i++)
    {
        printf("entering for loop\n");
		int key_id_rem = ftok("remainingfile", i+1);
		rem_id[i] = shmget(key_id_rem, sizeof(int), IPC_CREAT | 0666);
		printf("remaining id=%d\n",rem_id[i]);
	if (rem_id[i] == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID remaining= %d, index(i)= %d\n", rem_id[i],i);
        
        remaddr[i] = (int*)shmat(rem_id[i], (void *)0, 0); //attach shared memory
	if ((long)remaddr[i] == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	
     }	
    //----------------------------------------------------------------// 
     //shared memory waiting time//
    for (int i=0; i<n; i++)
    {
        //printf("entering for loop\n");
		int key_id_wait = ftok("waitingfile", i+1);
		wait_id[i] = shmget(key_id_wait, sizeof(int), IPC_CREAT | 0666);
		printf("waiting id=%d\n",wait_id[i]);
	if (wait_id[i] == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID waiting= %d, index(i)= %d\n", wait_id[i],i);
        
        waitaddr[i] = (int*)shmat(wait_id[i], (void *)0, 0); //attach shared memory
	if ((long)waitaddr[i] == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	*(waitaddr[i])=0;
     }	
    //----------------------------------------------------------------// 
    //semaphores//
    int key_id_s1 = ftok("keyfile", 'X');
    sem1 = semget(key_id_s1, 1, 0666 | IPC_CREAT);
    if (sem1 == -1)
    {
        perror("Error in create sem");
        exit(-1);
    }
    printf("\nsemaphore sets = %d\n", sem1);
   union Semun semun;
    semun.val = 0; 
    if (semctl(sem1, 0, SETVAL, semun) == -1)
    {
        perror("Error in semctl");
        exit(-1);
    }
    //---------------------------------------------------------------//
    
    if(algo==1)
    {
    	HPF();
    }
   else if (algo==3)
   {
      RR(timeslot);
   }
	else if (algo==2)
    {
        SRTN();
    }
	
	
   printf("at end of main before freeing\n");
    //upon termination release the clock resources.
    
		/*destroyClk(false);
		free(str2);
	   	free(str);
	   	free(str3);
	   	free(str4);
	   	free(str5);
	   	
	   	free (id);
	   	free (pidarr);
    	free (arrival);
    	free (run);
    	free (priority);
    	free (waiting_time); //start-arrival
    	free (remaining_time);
    	 
    	free(start_wait);
    	
    	free(start_time_RR);
    	free(start_time_SRTN);
    	free(WTA);
    	//free(ready);
    	//free(readySRTN);
    	
    	shmdt(busyaddr);
    	
    	
	//delete shared memory
	shmctl(busyid, IPC_RMID, 0);
	  
	//dettach
	//delete shared memory
	for (int i=0; i<n; i++)
	{
		shmdt(stataddr[i]);
		shmctl(stat_id[i], IPC_RMID, 0);
		shmdt(remaddr[i]);
		shmctl(rem_id[i], IPC_RMID, 0);
		shmdt(waitaddr[i]);
		shmctl(wait_id[i], IPC_RMID, 0);
         }
        
        free(remaddr);
    	free(rem_id);
    	free(waitaddr);
    	free(wait_id);
		free(stataddr);
    	free(stat_id);
    
  		free (status);*/
    
   printf("at end of main after freeing\n"); 
    
}
int Pindex=-1;

void HPF()
{
    int start_CPU=getClk();
    FILE * pFile;
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k\n");
    /////shared memory and semaphore sets for CPU_busy
    FILE * pFile2;
    pFile2 = fopen("scheduler.perf", "w");
    
    FILE * pFile3;
    pFile3 = fopen("memory.log", "w");
    fprintf(pFile3, "#At time x allocated y bytes for process z from i to j\n");
    	
	ready = (Node_priority**)malloc(sizeof(Node_priority*)*n);
	int size=-1;
	msg_changed=0;
	P_msgbuff newProcess;
	newProcess.id=0;
	MemoryNode* memory_location =NULL; //location of the current process 
	
	int rec_val=0;
	down(sem1); //all processes at the same time have been sent
	while(rec_val!=-1) //change condition
	{
	  rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);
	if (rec_val == -1 && errno==ENOMSG)
	{
         //perror("Error in receive");
         msg_changed=0;
        }
	else
	{
	printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
	msg_changed=1;
	}
	if(msg_changed==1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run,newProcess.memsize);
		//flag=1;
	}
	}
	while(newProcess.id!=-1 || !isempty_priority(size))
	{
		
			//if CPUs not busy dequeue ->no one executing
		if(*busyaddr==0 && !isempty_priority(size))
		{
			if(memory_location!=NULL)
			{
			 fprintf(pFile3, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),memory_location->size,id[Pindex],memory_location->starts_at,(memory_location->starts_at)+(memory_location->size));
			  deallocate(memory_location);
			  printf("\ndeallocated\n");
			 	PrintMemory(Mem_Map);
			 } 
		//dequeue
		if(Pindex>-1)
		{
		 WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
		 waiting_time[Pindex]=start_time-arrival[Pindex];
		fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),waiting_time[Pindex],(getClk()-arrival[Pindex]),WTA[Pindex]);
		}
		Node_priority* dequeued_proc= dequeue_priority(ready, &size);
		(*busyaddr)=1;
		//*(remaddr[Pindex]) = dequeued_proc->runningTime;
		
		//try to allocate memory
		memory_location= allocate(Mem_Map,dequeued_proc->memory_size);
		
		/*since all processes are <=256 bytes so this is the only case*/
		if (memory_location!=NULL) //successful memory allocation
		{
		   Pindex=binarySearch(id,0,index_p-1,dequeued_proc->id);
		     fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location->size,id[Pindex],memory_location->starts_at,(memory_location->starts_at)+(memory_location->size));
			printf("\nallocated\n");
			PrintMemory(Mem_Map);
			printf("process of id %d is dequeued\n",dequeued_proc->id); 
			///fork
			
			
			int pid=fork();
			start_time=getClk();
			fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],run[Pindex],(start_time-arrival[Pindex]));
			printf("process of id %d after forking command with pid= %d\n",dequeued_proc->id, pid);
		  	      if (pid==-1)
		 	       {
		  		perror("couldn't fork process of id \n");
		  		exit(-1);
			}
			else if (pid==0) //child process 
		 	      {
		 	       //(*busyaddr)=1;
		  		printf("started process of id %d runing \n",dequeued_proc->id);
		  		printf("size of id array in schedular=%d\n",index_p);
		  		//int Pindex=binarySearch(id,0,index_p-1,dequeued_proc->id);
		  		printf("index in schedular=%d\n",Pindex);
		  		
		  		int length = snprintf( NULL, 0, "%d", dequeued_proc->runningTime );
				str = malloc( length + 1 );
				snprintf( str, length + 1, "%d", dequeued_proc->runningTime );
				
				length = snprintf( NULL, 0, "%d", dequeued_proc->id );
				str2 = malloc( length + 1 );
				snprintf( str2, length + 1, "%d", dequeued_proc->id );
				
				length = snprintf( NULL, 0, "%d", Pindex );
				str3 = malloc( length + 1 );
				snprintf( str3, length + 1, "%d", Pindex );
				
				length = snprintf( NULL, 0, "%d", n );
				str4 = malloc( length + 1 );
				snprintf( str4, length + 1, "%d", n );

		  		       length = snprintf( NULL, 0, "%d", start_time );
				str5 = malloc( length + 1 );
				snprintf( str5, length + 1, "%d", start_time );

		  		execlp("./process.out", "process.out",str,str2,str3,str4,str5, NULL);
			}
			else //parent scheduler
		 	     {
		 		printf("parent: next cycle\n");	
			
			printf("remaining time of process %d is %d\n",id[Pindex],*(remaddr[Pindex]));	    
			  }		    
						
		}
					
	}
	if (isempty_priority(size) && newProcess.id!=-1)
	{
	rec_val=0;
	down(sem1);
	while(rec_val!=-1) //change condition
	{
	  rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);
	if (rec_val == -1 && errno==ENOMSG)
	{
         //perror("Error in receive");
         msg_changed=0;
        }
	else
	{
	printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
	msg_changed=1;
	}
	if(msg_changed==1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run,newProcess.memsize);
		//flag=1;
	}
	}
        }
        else //check if something has arrived
        {
        rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);

		if (rec_val == -1 && errno==ENOMSG)
		{
		    //printf("queue is not empty\n");
		    msg_changed=0;
		    //down (sem1); //?????????????????????????????????
		 }
		else
		{
		   printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
		   msg_changed=1;
		}
        
        }
        if(msg_changed==1 && newProcess.id!=-1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run,newProcess.memsize);
	}	//printf("id: %d",ready[0]->id);
	
            
}
 
 printf("last process id=%d, Pindex=%d\n",id[Pindex],Pindex);
 printf("status of last process=%c\n",*(stataddr[Pindex]));
 
 
   while (*(stataddr[Pindex])!='T')
   {
     printf("last process id=%d, Pindex=%d\n",id[Pindex],Pindex);
     sleep(1);
     printf("waiting for last process %c\n",*(stataddr[Pindex]));
   }  
    //deallocate last process from memory
   if(memory_location!=NULL)
	{
       fprintf(pFile3, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),memory_location->size,id[Pindex],memory_location->starts_at,(memory_location->starts_at)+(memory_location->size));
	    deallocate(memory_location);
	    printf("\ndeallocated\n");
	    PrintMemory(Mem_Map);
	} 
	
	//output files
   WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
   waiting_time[Pindex]=start_time-arrival[Pindex];
   fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),waiting_time[Pindex],(getClk()-arrival[Pindex]),WTA[Pindex]);
    fclose(pFile);   
    int end_CPU=getClk();
    int use_time=0;
    double wta_avg=0;
    double avg_wait=0;
    double std_wta=0;
    for(int i=0; i<n; i++)
    {
      use_time+=run[i];
      wta_avg+=WTA[i];
      avg_wait+=(double)waiting_time[i];
    }
    wta_avg/=(double)n;
    avg_wait/=(double)n;
    
    //standard deviation
    for(int i=0; i<n; i++)
    {
      std_wta+=pow((wta_avg-WTA[i]),2);
    }
    std_wta/=(double)n;
    std_wta=sqrt(std_wta);
   double CPU_Util =(((double)use_time)/((double)end_CPU-(double)start_CPU))*100;
   fprintf(pFile2,"CPU Utilization = %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f",CPU_Util,wta_avg,avg_wait,std_wta);
    fclose(pFile2);
    fclose(pFile3);
    free(ready);
    printf("End of HPF\n");
    return;      
}



void RR(int t_slot)
{
  int start_CPU=getClk();
  FILE * pFile;
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k\n");
    
    FILE * pFile2;
    pFile2 = fopen("scheduler.perf", "w");

    FILE * pFile3;
    pFile3 = fopen("memory.log", "w");
    fprintf(pFile3, "#At time x allocated y bytes for process z from i to j\n");

  (*busyaddr)=0;
  int cont_time=0;
  int start_slot_time;
  int start_time_s;
  struct Queue_c* q = create_Queue_c();
 
 //waiting list for processes that couldn't be allocated in memory 
 WaitingList* w_list= create_Waiting_List();
 
    //memory locations for all processes
	MemoryNode** memory_location = (MemoryNode**)malloc(n*sizeof(MemoryNode*));
	
	int slot=0;
	P_msgbuff newProcess;
	newProcess.id=0;
	int rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);
	if (rec_val == -1)
	{
         perror("Error in receive");
         //msg_changed=0;
        }
	else
	{
	printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
	}
	
	       id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		//priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		
		//enqueue
		printf("before enqueue\n");
		enqueue_circular(q, newProcess.id, newProcess.run,newProcess.memsize);  
		
		
		while(newProcess.id!=-1 || !isempty_circular(q))
		{
		printf("front of queue is id %d\n",q->front->id);
		 int rec_val_2 = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);

		if (rec_val_2 == -1 && errno==ENOMSG)
		{
		    //printf("queue is not empty\n");
		    msg_changed=0;
		 }
		else
		{
		   printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
		   msg_changed=1;
		}
        
       //add to ready queue
        if(msg_changed==1 && newProcess.id!=-1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		//priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		
		//enqueue
		enqueue_circular(q, newProcess.id,newProcess.run,newProcess.memsize);
	}
	
	//get index of next process in ready queue
	Pindex=binarySearch(id,0,index_p-1,q->front->id);
	
	//no process is working & ready queue is not empty yet
	if((*busyaddr)==0 && !isempty_circular(q))
	{
	
	
	(*busyaddr)=1;
	
	if (*(stataddr[Pindex])!='P') //not paused--> a new process
	{
	if(memory_location[Pindex]==NULL) 
		//try to allocate memory
		memory_location[Pindex]= allocate(Mem_Map,q->front->memory_size);
		
		if(memory_location[Pindex]!=NULL) //successful memory allocation
		{
		   fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[Pindex]->size,id[Pindex],memory_location[Pindex]->starts_at,(memory_location[Pindex]->starts_at)+(memory_location[Pindex]->size));
		      printf("\nallocated %d\n",Pindex);
		      PrintMemory(Mem_Map);
		      
	        int pid=fork();
	        start_time_RR[Pindex]=getClk();
	        fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],run[Pindex],*(waitaddr[Pindex])+(start_time_RR[Pindex]-arrival[Pindex]));

	        printf("forking condition entered with Pindex=%d and id=%d\n",Pindex,q->front->id);
		*(remaddr[Pindex]) = q->front->runningTime;
		remaining_time[Pindex]=q->front->runningTime;
	         if (pid==-1)
	         {
      		  perror("couldn't fork process of id \n");
      		  exit(-1);
			 }
			else if (pid==0) //child process 
     	        {
     	          //printf("started process of id %d runing \n",(*readyf)->id);
      		  printf("size of id array in schedular=%d\n",index_p);
      		//Pindex=binarySearch(id,0,index_p-1,(*readyf)->id);
      		//Pindex=binarySearch(id,0,index_p-1,q->front->id);
      		printf("index in schedular=%d\n",Pindex);
      		*(waitaddr[Pindex]) = 0;
      		
      		int length = snprintf( NULL, 0, "%d", q->front->runningTime );
			str = malloc( length + 1 );
			snprintf( str, length + 1, "%d", q->front->runningTime );
			
			length = snprintf( NULL, 0, "%d", q->front->id );
			str2 = malloc( length + 1 );
			snprintf( str2, length + 1, "%d", q->front->id );
			
			length = snprintf( NULL, 0, "%d", Pindex );
			str3 = malloc( length + 1 );
			snprintf( str3, length + 1, "%d", Pindex );
			
			length = snprintf( NULL, 0, "%d", n );
			str4 = malloc( length + 1 );
			snprintf( str4, length + 1, "%d", n );
			
			length = snprintf( NULL, 0, "%d", start_time_RR[Pindex]);
			str5 = malloc( length + 1 );
			snprintf( str5, length + 1, "%d", start_time_RR[Pindex]);

      		execlp("./process.out", "process.out",str,str2,str3,str4,str5, NULL);
		}
		else //parent scheduler
     	     {
     		printf("parent: next cycle\n");
     		
      		pidarr[Pindex]=pid;
		   // int start_slot = getClk();
	  		slot = t_slot;
	  	//printf("start_slot=%d\n",start_slot);	
	  	printf("t_slot=%d\n",t_slot);	
	  	 //sleep(3);
	  	// printf("status of id %d is %c\n",Pindex,*(stataddr[Pindex]));
		}
		
		printf("clock in schedular=%d\n",getClk());
		//kill(pid,SIGUSR1);
	}
	else //memory not found--> add to waiting list
	{
		printf("Process of id=%d has no memory\n",id[Pindex]);
		
		//add to waitinglist
		enqueue_waitingist(w_list, q->front->id, q->front->runningTime, q->front->memory_size);
		printf("Process of id=%d is added to waiting list\n",id[Pindex]);
		
		//remove from circular queue
		dequeue_circular(q);
		printf("Process of id=%d is dequeued for memory\n",id[Pindex]);
		
		*(busyaddr)=0;
		
	}
		}
		
	       
	       else //resume -> status==P
	       {  
	       *(waitaddr[Pindex])=*(waitaddr[Pindex])+(getClk()-start_wait[Pindex]);
	       printf("process of id= %d is resumed at clock= %d with waiting time  	=%d and start wait time=%d\n",id[Pindex],getClk(),*(waitaddr[Pindex]),start_wait[Pindex]);
              *(stataddr[Pindex])='R';
              fprintf(pFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_RR[Pindex]-arrival[Pindex]));
	      kill(pidarr[Pindex],SIGCONT);
	       cont_time=getClk();
	       
	       printf("remaining time of process with id=%d is %d\n",id[Pindex],*(remaddr[Pindex]));
	      /**(waitaddr[Pindex])=*(waitaddr[Pindex])+(getClk()-start_wait[Pindex]);
	       printf("process of id= %d is resumed at clock= %d with waiting time  	=%d and start wait time=%d\n",id[Pindex],getClk(),*(waitaddr[Pindex]),start_wait[Pindex]);*/
	       
	      // sleep(10);
	       }
	    if(memory_location[Pindex]!=NULL) //successful memory allocation
		{   
	       if (*(stataddr[Pindex])=='S' || *(stataddr[Pindex])=='W')
	       {
	          start_slot_time = start_time_RR[Pindex];
	       }
	       else
	       {
	          start_slot_time = cont_time;
	       }
	       
	       printf("start_slot_time of id=%d is %d ,start_time=%d, cont_time=%d\n",id[Pindex],start_slot_time,start_time_RR[Pindex],cont_time);
	       while(t_slot!=getClk()-start_slot_time && *(remaddr[Pindex])>0)
                {
	  	 	 if(newProcess.id!=-1)
	  	 	 {
	  	 
	  	   int rec_val_1 = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);

		if (rec_val_1 == -1 && errno==ENOMSG)
		{
		    //printf("queue is not empty\n");
		    msg_changed=0;
		 }
		else
		{
		   printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
		   msg_changed=1;
		}
        
        }
        if(msg_changed==1 && newProcess.id!=-1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		//priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		enqueue_circular(q, newProcess.id,newProcess.run, newProcess.memsize);
	}	//printf("id: %d",ready[0]->id);
	  	 
	  	} 
	       
	       start_wait[Pindex]=getClk();
		//printf("remaining time of process %d in schedular is %d\n",id[Pindex],*(remaddr[Pindex]));
//		sleep(6);
		
		(*busyaddr)=0; 
		//printf("waiting time %d\n",*(waitaddr[Pindex]));
		
	       
	       
		if (*(remaddr[Pindex])<=0)
		{
		      (*busyaddr)=0;
		      *(stataddr[Pindex])='T';
		      fprintf(pFile3, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),memory_location[Pindex]->size,id[Pindex],memory_location[Pindex]->starts_at,(memory_location[Pindex]->starts_at)+(memory_location[Pindex]->size));
		      //deallocate from memory
		      deallocate(memory_location[Pindex]);
		      printf("\ndeallocated\n");
		      PrintMemory(Mem_Map);
		      
		      //remove from circular queue
		      dequeue_circular(q);
		      printf("Process of id=%d is dequeued\n",id[Pindex]);
		      
		      WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
   		      //waiting_time[Pindex]=start_time-arrival[Pindex];
		      fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_RR[Pindex]-arrival[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
		      
		        //check waiting list
		      Node_circular* temp=w_list->front; //iterator
		      if(temp!=NULL)
		      {
		      //for first element 
		      printf("w.l not empty\n");
		      int wl_index=binarySearch(id,0,index_p-1,temp->id);
			  memory_location[wl_index]=allocate(Mem_Map,temp->memory_size);	
				if(memory_location[wl_index]!=NULL)
				{
				fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[wl_index]->size,id[wl_index],memory_location[wl_index]->starts_at,(memory_location[wl_index]->starts_at)+(memory_location[wl_index]->size));
				printf("w.l first was allocated id %d\n",temp->id);
				PrintMemory(Mem_Map);
					enqueue_at_front(q,temp->id,temp->runningTime, temp->memory_size); //add to front of ready queue
					printf("enqueued at front\n");
					RemoveFromList(w_list, NULL); //remove first element
					printf("removed from w.l\n");
				}
				else //check rest of waiting list
				{
					while(temp->next!=NULL)
					{
						wl_index=binarySearch(id,0,index_p-1,temp->next->id);
						memory_location[wl_index]= allocate(Mem_Map,temp->next->memory_size);	
						if(memory_location[wl_index]!=NULL)
						{
						fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[wl_index]->size,id[wl_index],memory_location[wl_index]->starts_at,(memory_location[wl_index]->starts_at)+(memory_location[wl_index]->size));
							enqueue_at_front(q,temp->next->id,temp->next->runningTime, temp->next->memory_size); //add to front of ready queue							
							RemoveFromList(w_list, temp);//remove first element
							break;
							
						}
						
						temp=temp->next;
					}
				}
	
		      }///end of waiting list check
	

		}
		else //time slot ended 
		{
		     (*busyaddr)=0;
		     //printf("process id=%d before being stopped,status=%c and remaining time =%d\n",id[Pindex],*(stataddr[Pindex]),*(remaddr[Pindex]));
		     *(stataddr[Pindex])='P';
		     while(*(remaddr[Pindex])!=remaining_time[Pindex]-t_slot)
		     {
		       printf("process of id=%d entered the while check loop\n",id[Pindex]);
		       *(remaddr[Pindex])=(q->front->runningTime)-(getClk()-start_time_RR[Pindex]-*(waitaddr[Pindex]));
		     }
		      printf("process id=%d before being stopped,status=%c and remaining time =%d\n",id[Pindex],*(stataddr[Pindex]),*(remaddr[Pindex]));
		      if(*(remaddr[Pindex])>0)
		      {
		        kill(pidarr[Pindex],SIGSTOP);
		        fprintf(pFile, "At time %d process %d paused arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_RR[Pindex]-arrival[Pindex]));

		        remaining_time[Pindex]=*(remaddr[Pindex]);
		        rotate(q);
		        printf("process of id=%d is stopped at clock= %d\n",id[Pindex],getClk());
		      }
		      else
		      { 
		        (*busyaddr)=0;
		        *(stataddr[Pindex])='T';
		        
		        //deallocate from memory
		        fprintf(pFile3, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),memory_location[Pindex]->size,id[Pindex],memory_location[Pindex]->starts_at,(memory_location[Pindex]->starts_at)+(memory_location[Pindex]->size));
				deallocate(memory_location[Pindex]);
				printf("\ndeallocated\n");
				PrintMemory(Mem_Map);
		      
		        dequeue_circular(q);
		        printf("Process of id=%d is dequeued\n",id[Pindex]);
		        WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
		        fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_RR[Pindex]-arrival[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
		        
		        //check waiting list
		      Node_circular* temp=w_list->front; //iterator
		      if(temp!=NULL)
		      {
		      //for first element 
		      int wl_index=binarySearch(id,0,index_p-1,temp->id);
			  memory_location[wl_index]=allocate(Mem_Map,temp->memory_size);	
				if(memory_location[wl_index]!=NULL)
				{
				fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[wl_index]->size,id[wl_index],memory_location[wl_index]->starts_at,(memory_location[wl_index]->starts_at)+(memory_location[wl_index]->size));
					enqueue_at_front(q,temp->id, temp->runningTime,temp->memory_size); //add to front of ready queue
					RemoveFromList(w_list, NULL); //remove first element
				}
				else //check rest of waiting list
				{
					while(temp->next!=NULL)
					{
						wl_index=binarySearch(id,0,index_p-1,temp->next->id);
						memory_location[wl_index]= allocate(Mem_Map,temp->next->memory_size);	
						if(memory_location[wl_index]!=NULL)
						{
						fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[wl_index]->size,id[wl_index],memory_location[wl_index]->starts_at,(memory_location[wl_index]->starts_at)+(memory_location[wl_index]->size));
							enqueue_at_front(q,temp->next->id, temp->next->runningTime,temp->next->memory_size); //add to front of ready queue							
							RemoveFromList(w_list, temp);//remove first element
							break;
							
						}
						
						temp=temp->next;
					}
				}
	
		      }///end of waiting list check
		      

		      }
		}
		}
	        }
	       // printf("remaining time of process 1 in loop=%d\n",*(remaddr[0]));
	        }
fclose(pFile);	        
int end_CPU=getClk();	        
int use_time=0;
double wta_avg=0;
double avg_wait=0;
double std_wta=0;
   for(int i=0; i<n; i++)
    {
      use_time+=run[i];
      wta_avg+=WTA[i];
      avg_wait+=(double)(*(waitaddr[i]))+(double)((start_time_RR[i]-arrival[i]));
    }
 wta_avg/=(double)n;
 avg_wait/=(double)n;
   for(int i=0; i<n; i++)
    {
      std_wta+=pow((wta_avg-WTA[i]),2);
    }
  std_wta/=(double)n;
  std_wta=sqrt(std_wta);
  double CPU_Util =(((double)use_time)/((double)end_CPU-(double)start_CPU))*100;
   fprintf(pFile2,"CPU Utilization = %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f",CPU_Util,wta_avg,avg_wait,std_wta);	        
fclose(pFile2);
fclose(pFile3);
free(memory_location);	 
}
//////////////////////////////////
void SRTN()
{
(*busyaddr)=0;
Pindex=-1;
int pid;
int start_CPU=getClk();
int waiting_list_flag=0;
  FILE * pFile;
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k\n");
    
    FILE * pFile2;
    pFile2 = fopen("scheduler.perf", "w");

    FILE * pFile3;
    pFile3 = fopen("memory.log", "w");
    fprintf(pFile3, "#At time x allocated y bytes for process z from i to j\n");
//memory locations for all processes
MemoryNode** memory_location = (MemoryNode**)malloc(n*sizeof(MemoryNode*));

 //waiting list for processes that couldn't be allocated in memory 
 WaitingList* w_list= create_Waiting_List();
 	
readySRTN = (Node_running**)malloc(sizeof(Node_running*)*n);
	int size=-1;
	msg_changed=0;
	P_msgbuff newProcess;
	newProcess.id=0;
	
	down(sem1);
	int rec_val;
	while(rec_val!=-1)
	{
		rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);
		if (rec_val == -1 && errno==ENOMSG)
		{
		     perror("Error in receive");
		     msg_changed=0;
		}
		else
		{
		printf("\nMessage received from server: %d\n", newProcess.id);
		msg_changed=1;
		}
		
		if(msg_changed==1 && newProcess.id!=-1)
		{	
			//add to data structure
			id[index_p]= newProcess.id;
			run[index_p]= newProcess.run;
			priority[index_p]= newProcess.priority;
			arrival[index_p]= newProcess.arrival;
			index_p++;
			//enqueue
			enqueue_running(newProcess.priority, newProcess.id, readySRTN, &size, newProcess.run, newProcess.memsize);


		}	
	}
	while(newProcess.id!=-1 || !isempty_priority(size))
	{
	rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);

		if (rec_val == -1 && errno==ENOMSG)
		{
		    //printf("queue is not empty\n");
		    msg_changed=0;
		 }
		else
		{
		   printf("\nMessage received from server: %d\n", newProcess.id);
		   msg_changed=1;
		   
		}
		if(msg_changed==1 && newProcess.id!=-1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		enqueue_running(newProcess.priority, newProcess.id, readySRTN, &size, newProcess.run, newProcess.memsize);
		index_p++;
		
		down(sem1);
		while(rec_val!=-1)
	     {
		rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, IPC_NOWAIT);
		if (rec_val == -1 && errno==ENOMSG)
		{
		     perror("Error in receive");
		     msg_changed=0;
		    }
		else
		{
		printf("\nMessage received from server: %d\n", newProcess.id);
		msg_changed=1;
		}
		
		if(msg_changed==1 && newProcess.id!=-1)
		{	
			//add to data structure
			id[index_p]= newProcess.id;
			run[index_p]= newProcess.run;
			priority[index_p]= newProcess.priority;
			arrival[index_p]= newProcess.arrival;
			index_p++;
			//enqueue
			enqueue_running(newProcess.priority, newProcess.id, readySRTN, &size, newProcess.run, newProcess.memsize);


		}	
	}
		
               if((*busyaddr)==1 && !isempty_priority(size))
               {
                  if (newProcess.run < *(remaddr[Pindex]))
  		  {
  		  printf("preemption\n");
  		      (*busyaddr)=0;
  		      printf("preemption2\n");
  		      *(stataddr[Pindex])='P';
  		      printf("preemption3\n");
  		      start_wait[Pindex]=getClk();
  		      printf("preemption4\n");
                     kill(pidarr[Pindex],SIGSTOP); 
                      fprintf(pFile, "At time %d process %d paused arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_SRTN[Pindex]-arrival[Pindex]));
                     printf("preemption5\n");   
     		      enqueue_running(dequeuedS->priority, dequeuedS->id, readySRTN, &size, *(remaddr[Pindex]), dequeuedS->runningTime);
     		      printf("preemption6\n");
     		     
   		 }
		} 

	}
		////// 3lshan afdal a-recieve (elly fo2 y3ny)
		if((*busyaddr)==0 && !isempty_priority(size))
		{
		if(Pindex>-1 && *(stataddr[Pindex])!='P')
		{
		fprintf(pFile3, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),memory_location[Pindex]->size,id[Pindex],memory_location[Pindex]->starts_at,(memory_location[Pindex]->starts_at)+(memory_location[Pindex]->size));
		//deallocates
		deallocate(memory_location[Pindex]);
		memory_location[Pindex]=NULL;
		printf("\ndeallocated \n%d",Pindex);
		PrintMemory(Mem_Map);
		
		 WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
		 waiting_time[Pindex]=start_time-arrival[Pindex];
		fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),waiting_time[Pindex]+*(waitaddr[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
		
		//check waiting list
		waiting_list_flag=0;
		      Node_circular* temp=w_list->front; //iterator
		      if(temp!=NULL)
		      {
		      //for first element 
		      printf("w.l not empty\n");
		      int wl_index=binarySearch(id,0,index_p-1,temp->id);
			  memory_location[wl_index]=allocate(Mem_Map,temp->memory_size);	
				if(memory_location[wl_index]!=NULL)
				{
				fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[wl_index]->size,id[wl_index],memory_location[wl_index]->starts_at,(memory_location[wl_index]->starts_at)+(memory_location[wl_index]->size));
				printf("w.l first was allocated id %d\n",temp->id);
				PrintMemory(Mem_Map);
					//enqueue_at_front(q,temp->id,temp->runningTime, temp->memory_size); //add to front of ready queue
					Node_running* tempnode = (Node_running*)malloc(sizeof(Node_running)); 
                                       tempnode->id = temp->id; 
                                       //tempnode->priority = temp->priority; 
                                       tempnode->runningTime = temp->runningTime;
                                       tempnode->memory_size= temp->memory_size;
                                       dequeuedS=tempnode;
                                      waiting_list_flag=1;
					printf("enqueued at front\n");
					RemoveFromList(w_list, NULL); //remove first element
					printf("removed from w.l\n");
				}
				else //check rest of waiting list
				{
					while(temp->next!=NULL)
					{
						wl_index=binarySearch(id,0,index_p-1,temp->next->id);
						memory_location[wl_index]= allocate(Mem_Map,temp->next->memory_size);	
						if(memory_location[wl_index]!=NULL)
						{
						fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[wl_index]->size,id[wl_index],memory_location[wl_index]->starts_at,(memory_location[wl_index]->starts_at)+(memory_location[wl_index]->size));
							//enqueue_at_front(q,temp->next->id,temp->next->runningTime, temp->next->memory_size); //add to front of ready queue		
							Node_running* tempnode = (Node_running*)malloc(sizeof(Node_running)); 
                                                   tempnode->id = temp->next->id; 
                                                   //tempnode->priority = temp->next->priority; 
                                                   tempnode->runningTime = temp->next->runningTime;
                                                   tempnode->memory_size= temp->next->memory_size;
                                                   dequeuedS=tempnode;
                                                   waiting_list_flag=1;					
							RemoveFromList(w_list, temp);//remove first element
							break;
							
						}
						
						temp=temp->next;
					}
				}
	
		      }///end of waiting list check
	
		}
		
		if(waiting_list_flag==0)
		{
		//dequeue
	 	  dequeuedS = dequeue_running(readySRTN, &size);
		}
		
		(*busyaddr)=1;
		printf("process of id %d is dequeued\n",dequeuedS->id); 
		
		Pindex=binarySearch(id,0,index_p-1,dequeuedS->id);
		
		
		
		if (*(stataddr[Pindex])!='P')//new process
		{
		memory_location[Pindex]=NULL;
		//try to allocate
		memory_location[Pindex]=allocate(Mem_Map,dequeuedS->memory_size);
		printf("\nallocated \n%d",Pindex);
		PrintMemory(Mem_Map);
		
		if(memory_location[Pindex]!=NULL) //successful allocation
		{
		fprintf(pFile3, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),memory_location[Pindex]->size,id[Pindex],memory_location[Pindex]->starts_at,(memory_location[Pindex]->starts_at)+(memory_location[Pindex]->size));
		///fork
		pid=fork();
		*(remaddr[Pindex])=run[Pindex];
		*(stataddr[Pindex])='R';
		start_time_SRTN[Pindex]=getClk();
		start_time=getClk();
		fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],run[Pindex],*(waitaddr[Pindex])+(start_time_SRTN[Pindex]-arrival[Pindex]));
		printf("process of id %d after forking command with pid= %d\n",dequeuedS->id, pid);
		 if (pid==-1)
     	        {
      		perror("couldn't fork process of id \n");
      		exit(-1);
		}
		else if (pid==0) //child process 
     	      {
     	       
      		printf("started process of id %d runing \n",dequeuedS->id);
      		printf("size of id array in schedular=%d\n",index_p);
      		printf("index in schedular=%d\n",Pindex);
      		
      		int length = snprintf( NULL, 0, "%d", dequeuedS->runningTime );
			str = malloc( length + 1 );
			snprintf( str, length + 1, "%d", dequeuedS->runningTime );
			
			length = snprintf( NULL, 0, "%d", dequeuedS->id );
			str2 = malloc( length + 1 );
			snprintf( str2, length + 1, "%d", dequeuedS->id );
			
			length = snprintf( NULL, 0, "%d", Pindex );
			str3 = malloc( length + 1 );
			snprintf( str3, length + 1, "%d", Pindex );
			
			length = snprintf( NULL, 0, "%d", n );
			str4 = malloc( length + 1 );
			snprintf( str4, length + 1, "%d", n );

      		       length = snprintf( NULL, 0, "%d", start_time );
			str5 = malloc( length + 1 );
			snprintf( str5, length + 1, "%d", start_time );

      		execlp("./process.out", "process.out",str,str2,str3,str4,str5, NULL);
		}
		else //parent scheduler
     	     {
     	        pidarr[Pindex]=pid;
     		printf("parent: next cycle\n");
		
		printf("remaining time of process %d is %d\n",id[Pindex],*(remaddr[Pindex]));	    
	     }	
	      
		}
		else //no memory found
		{
			//add to waiting list
		enqueue_waitingist(w_list, dequeuedS->id, dequeuedS->runningTime, dequeuedS->memory_size);
		printf("Process of id=%d is added to waiting list\n",id[Pindex]);
		}
	}
		else if (*(stataddr[Pindex])=='P')
		{
		    *(waitaddr[Pindex])=*(waitaddr[Pindex])+(getClk()-start_wait[Pindex]);
                  fprintf(pFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_SRTN[Pindex]-arrival[Pindex]));
		    (*busyaddr)=1;
		    *(stataddr[Pindex])='R';
		    kill(pidarr[Pindex],SIGCONT);
		}
		
	}
	
	printf("\nrem=%d\n",*(remaddr[Pindex]));
	//check if process has terminated
	/*if (*(remaddr[Pindex])<=0 && memory_location[Pindex]!=NULL)
	{
		printf("\npindex=%d type=%c at clk= %d rem=%d\n",Pindex, memory_location[Pindex]->type,getClk(), *(remaddr[Pindex]));
		//deallocates
		deallocate(memory_location[Pindex]);
		memory_location[Pindex]=NULL;
		printf("\ndeallocated \n%d",Pindex);
		PrintMemory(Mem_Map);
		/*fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
	}*/

}
 printf("last process id=%d, Pindex=%d\n",id[Pindex],Pindex);
 printf("status of last process=%c\n",*(stataddr[Pindex]));
 
   while (*(stataddr[Pindex])!='T')
   {
     printf("last process id=%d, Pindex=%d\n",id[Pindex],Pindex);
     sleep(1);
   }          
//deallocate last process
printf("\npindex=%d type=%c at clk= %d\n",Pindex, memory_location[Pindex]->type,getClk());
		//deallocate
		fprintf(pFile3, "At time %d freed %d bytes for process %d from %d to %d\n",getClk(),memory_location[Pindex]->size,id[Pindex],memory_location[Pindex]->starts_at,(memory_location[Pindex]->starts_at)+(memory_location[Pindex]->size));
		deallocate(memory_location[Pindex]);
		memory_location[Pindex]=NULL;
		printf("\ndeallocated \n%d",Pindex);
		PrintMemory(Mem_Map);
WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];		
fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex])+(start_time_SRTN[Pindex]-arrival[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
fclose(pFile);	        
int end_CPU=getClk();	        
int use_time=0;
double wta_avg=0;
double avg_wait=0;
double std_wta=0;
   for(int i=0; i<n; i++)
    {
      use_time+=run[i];
      wta_avg+=WTA[i];
      avg_wait+=(double)(*(waitaddr[i]))+(double)(start_time_SRTN[i]-arrival[i]);
    }
 wta_avg/=(double)n;
 avg_wait/=(double)n;
   for(int i=0; i<n; i++)
    {
      std_wta+=pow((wta_avg-WTA[i]),2);
    }
  std_wta/=(double)n;
  std_wta=sqrt(std_wta);
  double CPU_Util =(((double)use_time)/((double)end_CPU-(double)start_CPU))*100;
   fprintf(pFile2,"CPU Utilization = %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f",CPU_Util,wta_avg,avg_wait,std_wta);	        
fclose(pFile2);
fclose(pFile3);
		
		free(memory_location);
		free(readySRTN);
		
		destroyClk(false);
		free(str2);
	   	free(str);
	   	free(str3);
	   	free(str4);
	   	free(str5);
	   	
	   	free (id);
	   	free (pidarr);
    	free (arrival);
    	free (run);
    	free (priority);
    	free (waiting_time); //start-arrival
    	free (remaining_time);
    	 
    	free(start_wait);
    	
    	free(start_time_RR);
    	free(start_time_SRTN);
    	free(WTA);
    	//free(ready);
    	//free(readySRTN);
    	
    	shmdt(busyaddr);
    	
    	
	//delete shared memory
	shmctl(busyid, IPC_RMID, 0);
	  
	//dettach
	//delete shared memory
	for (int i=0; i<n; i++)
	{
		shmdt(stataddr[i]);
		shmctl(stat_id[i], IPC_RMID, 0);
		shmdt(remaddr[i]);
		shmctl(rem_id[i], IPC_RMID, 0);
		shmdt(waitaddr[i]);
		shmctl(wait_id[i], IPC_RMID, 0);
         }
        
        free(remaddr);
    	free(rem_id);
    	free(waitaddr);
    	free(wait_id);
		free(stataddr);
    	free(stat_id);
    
  		free (status);
  		printf("End of SRTN\n");
}

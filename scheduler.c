#include "headers.h"
#include "data_structures.h"
#include <errno.h>
extern int errno ;

//void clearResources(int);
//void processTerm(int);
void HPF();
/*arguments:
 * 	1:algo 
 *	2:time slot
 *	3: 
 */
void RR(int t_slot);
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
int sem1;  
int main(int argc, char * argv[])
{
	//signal(SIGINT, clearResources);
	//signal(SIGUSR1, processTerm);
	initClk();
printf("\n started\n\n");printf("\n arguments=%d \n\n",argc);
	//TODO implement the scheduler :) 
	
	//command line arguments
	int algo= atoi(argv[1]);
     timeslot=atoi(argv[2]);
     n=atoi(argv[3]);
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
    WTA=(double*)malloc(n*sizeof(double));
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
   //------------------------------------------------------------//
   //shared memory remaining time//
   /*int key_id_rem = ftok("remainingfile", 'M');
	remid = shmget(key_id_rem, sizeof(int), IPC_CREAT | 0666);
	if (remid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID remaining= %d\n", remid);
        
        remaddr = (int*)shmat(remid, (void *)0, 0); //attach shred memo
	if ((long)remaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}*/
   //------------------------------------------------------------//
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
	
	
	
    
	//every clk--> if no one working :dequeue
	//fork new process
	//wait 
	//repeat till msg queue is empty
	 
    //PCB : id, run time, remainig, state, waiting
    /*int id[n];
    int arrival[n];
    int run[n];
    int priority[n];
    int waiting_time[n]; //start-arrival
    int remaining_time[n];
    char status[n]; //R:running, T:terminated, P: paused*/
    

     
 	    
    //sleep(5);
    //upon termination release the clock resources.
    
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
    	free(WTA);
    	free(ready);
    	shmdt(busyaddr);
    	
    	
	//delete shared memory
	shmctl(busyid, IPC_RMID, 0);
	
	//dettach
	//shmdt(remaddr);
	
	//delete shared memory
	//shmctl(remid, IPC_RMID, 0);
        
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
    //kill(getpid(),SIGINT);
  	free (status);
    /* P_msgbuff t_end;  //end of processes
    t_end.id=-2;
    int send_val = msgsnd(msgq_ready,&t_end,sizeof(t_end), !IPC_NOWAIT); 
		if (send_val == -1)
	    	perror("Errror in send");
		printf("will send %d\n", t_end.id);
    while(true);*/
    
    
}
int Pindex=-1;
//Node_priority **ready;
//int msgq_busy;
//int *busyaddr;
//int busyid;
//char*str,*str2,*str3,*str4;
void HPF()
{
    int start_CPU=getClk();
    FILE * pFile;
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k\n");
    /////shared memory and semaphore sets for CPU_busy
    FILE * pFile2;
    pFile2 = fopen("scheduler.perf", "w");
    	
	/*int key_id_busy = ftok("keyfile", 'B');
	busyid = shmget(key_id_busy, 256, IPC_CREAT | 0666);
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
	}*/
	(*busyaddr)=0; //CPU is not busy
        //int Pindex;
        //int counter=0; //track if all processes are forked
        //int max;
 	/*key_t key_busy;
    

	//create/get message queue
    key_busy = ftok("keyfile", 'B');  //unique
    
    msgq_busy = msgget(key_busy, 0666 | IPC_CREAT); 
    if (msgq_busy == -1)
    {
        perror("Error in creating busy");
        exit(-1);
    }
    printf("Message Queue ID  (busy)= %d\n", msgq_busy);
    
    int CPU_busy=10;
    
    int send_val = msgsnd(msgq_busy, &CPU_busy, sizeof(CPU_busy), !IPC_NOWAIT); 

		if (send_val == -1)
		    perror("Error in send");
		printf("busy=%d\n",CPU_busy);*/


	ready = (Node_priority**)malloc(sizeof(Node_priority*)*n);
	int size=-1;
	msg_changed=0;
	P_msgbuff newProcess;
	newProcess.id=0;
	/*int rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);
	if (rec_val == -1)
	{
         perror("Error in receive");
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
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run);
	}*/
	//int flag=0;
	//int counter=5;
	/*while(flag==0) //change condition
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
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run);
		//flag=1;
	}
	/*if (flag==1 && counter>=0)
	{
	  flag=0;
          counter=5;
	}
	else if(flag==0 && counter==0)
	{
	  flag=1;
	}
	counter--;*/
	//}	//printf("id: %d",ready[0]->id);
	int rec_val=0;
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
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run);
		//flag=1;
	}
	}
	while(newProcess.id!=-1 || !isempty_priority(size))
	{
		
		//printf("message changed=%d", msg_changed);
		//check if ready queue is empty or not (if empty-->wait, else-->NoWait)
	
	/*if(msg_changed==1 && newProcess.id!=-1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run);
	}*/	//printf("id: %d",ready[0]->id);
		
		//check CPU state
		/*rec_val = msgrcv(msgq_ready, &CPU_busy, sizeof(CPU_busy), 0, IPC_NOWAIT);
				if (rec_val == -1 && errno==ENOMSG)
				    printf("not changed yet\n");*/
		//printf("empty condition=%d", isempty_priority(size));		    	    
		//if CPUs not busy dequeue ->no one executing
		if(*busyaddr==0 && !isempty_priority(size))
		{
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
		printf("process of id %d is dequeued\n",dequeued_proc->id); 
		///fork
		Pindex=binarySearch(id,0,index_p-1,dequeued_proc->id);
		/*if(Pindex!=0)
		{
		  //fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d\n",getClk(),id[Pindex-1],arrival[Pindex-1],run[Pindex-1],*(remaddr[Pindex-1]),(start_time-arrival[Pindex-1]));
		}*/
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
      		
		   /*rec_val = msgrcv(msgq_ready, &CPU_busy, sizeof(CPU_busy), 0, !IPC_NOWAIT);
				if (rec_val == -1 && errno==ENOMSG)
				    printf("not changed yet\n");*/
		//counter++;	
		
		printf("remaining time of process %d is %d\n",id[Pindex],*(remaddr[Pindex]));	    
	      }		    
				    
				    
	}
	if (isempty_priority(size) && newProcess.id!=-1)
	{
	/*rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);

		if (rec_val == -1)
		{
		   perror("Error in receive");
		   msg_changed=0;
		}
		else
		{
		   printf("\nMessage received from server: %d at clk= %d\n", newProcess.id,getClk());
		   msg_changed=1;
		}*/
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
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run);
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
		enqueue_priority(newProcess.priority, newProcess.id, ready, &size, newProcess.run);
	}	//printf("id: %d",ready[0]->id);
	
      /*if (counter==n-1)
      {
       max=Pindex;
      } */ 
 //printf("last received id=%d\n",newProcess.id);      
}
 
 printf("last process id=%d, Pindex=%d\n",id[Pindex],Pindex);
 printf("status of last process=%c\n",*(stataddr[Pindex]));
 
 
   while (*(stataddr[Pindex])!='T')
   {
     printf("last process id=%d, Pindex=%d\n",id[Pindex],Pindex);
     sleep(1);
     printf("waiting for last process %c\n",*(stataddr[Pindex]));
   }  
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
    for(int i=0; i<n; i++)
    {
      std_wta+=pow((wta_avg-WTA[i]),2);
    }
    std_wta/=(double)n;
    std_wta=sqrt(std_wta);
   double CPU_Util =(((double)use_time)/((double)end_CPU-(double)start_CPU))*100;
   fprintf(pFile2,"CPU Utilization = %.2f%%\nAvg WTA = %.2f\nAvg Waiting = %.2f\nStd WTA = %.2f",CPU_Util,wta_avg,avg_wait,std_wta);
    fclose(pFile2);      
}


//Node_circular **readyf=NULL;
//Node_circular **readyr=NULL;

void RR(int t_slot)
{
  int start_CPU=getClk();
  FILE * pFile;
    pFile = fopen("scheduler.log", "w");
    fprintf(pFile, "#At time x process y state arr w total z remain y wait k\n");
    
    FILE * pFile2;
    pFile2 = fopen("scheduler.perf", "w");

  (*busyaddr)=0;
  int cont_time=0;
  int start_slot_time;
  int start_time_s;
  struct Queue_c* q = create_Queue_c();
  /*readyf = (Node_circular**)malloc(sizeof(Node_circular*)*n);
  readyr = (Node_circular**)malloc(sizeof(Node_circular*)*n);
	int size=-1;
	int msg_changed=0;*/
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
		//enqueue_circular(newProcess.id, readyf, readyr, newProcess.run);
		enqueue_circular(q, newProcess.id, newProcess.run);  
		
	//if(msg_changed==1)
	//{	
		//add to data structure
		
		while(newProcess.id!=-1 || !isempty_circular(q))
		{
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
        
       
        if(msg_changed==1 && newProcess.id!=-1)
	{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		//priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		enqueue_circular(q, newProcess.id,newProcess.run);
	}
	Pindex=binarySearch(id,0,index_p-1,q->front->id);
	if((*busyaddr)==0 && !isempty_circular(q))
	{
	
	//Pindex=binarySearch(id,0,index_p-1,q->front->id);
	(*busyaddr)=1;
	//printf("status of id %d is %c\n",id[Pindex],*(stataddr[Pindex]));
	
	if (*(stataddr[Pindex])!='P')
	{
	        int pid=fork();
	        start_time_RR[Pindex]=getClk();
	        fprintf(pFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],run[Pindex],*(waitaddr[Pindex]));

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
		
	       
	       else //resume -> status==P
	       {
	   //    han get el id elly han-continue beeh   
	       //(*busyaddr)=1;
	       *(waitaddr[Pindex])=*(waitaddr[Pindex])+(getClk()-start_wait[Pindex]);
	       printf("process of id= %d is resumed at clock= %d with waiting time  	=%d and start wait time=%d\n",id[Pindex],getClk(),*(waitaddr[Pindex]),start_wait[Pindex]);
              *(stataddr[Pindex])='R';
              fprintf(pFile, "At time %d process %d resumed arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex]));
	      kill(pidarr[Pindex],SIGCONT);
	       cont_time=getClk();
	       
	       printf("remaining time of process with id=%d is %d\n",id[Pindex],*(remaddr[Pindex]));
	      /**(waitaddr[Pindex])=*(waitaddr[Pindex])+(getClk()-start_wait[Pindex]);
	       printf("process of id= %d is resumed at clock= %d with waiting time  	=%d and start wait time=%d\n",id[Pindex],getClk(),*(waitaddr[Pindex]),start_wait[Pindex]);*/
	       
	      // sleep(10);
	       }
	       
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
		enqueue_circular(q, newProcess.id,newProcess.run);
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
		      dequeue_circular(q);
		      printf("Process of id=%d is dequeued\n",id[Pindex]);
		      WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
   		      //waiting_time[Pindex]=start_time-arrival[Pindex];
		      fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
		}
		else
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
		        fprintf(pFile, "At time %d process %d paused arr %d total %d remain %d wait %d\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex]));

		        remaining_time[Pindex]=*(remaddr[Pindex]);
		        rotate(q);
		        printf("process of id=%d is stopped at clock= %d\n",id[Pindex],getClk());
		      }
		      else
		      { 
		        (*busyaddr)=0;
		        *(stataddr[Pindex])='T';
		        dequeue_circular(q);
		        printf("Process of id=%d is dequeued\n",id[Pindex]);
		        WTA[Pindex]= (double)(getClk()-arrival[Pindex])/(double)run[Pindex];
		        fprintf(pFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",getClk(),id[Pindex],arrival[Pindex],run[Pindex],*(remaddr[Pindex]),*(waitaddr[Pindex]),(getClk()-arrival[Pindex]),WTA[Pindex]);
		      }
		}
		
	        }
	        printf("remaining time of process 1 in loop=%d\n",*(remaddr[0]));
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
      avg_wait+=(double)(*(waitaddr[i]));
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
}



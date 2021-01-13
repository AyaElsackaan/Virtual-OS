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
  
  //PCB : id, run time, remainig, state, waiting
    int* id;
    int* arrival;
    int* run;
    int* priority;
    int* waiting_time; //start-arrival
    int* remaining_time;
    char* status; //R:running, T:terminated, P: paused
int index_p=0;
int *busyaddr;
int busyid;
    
int *stat_id;  
char **stataddr;

//int *remaddr;
//int remid;

int *rem_id;  
int **remaddr;

Node_priority **ready;
char*str,*str2,*str3,*str4,*str5;
  
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
    	free (arrival);
    	free (run);
    	free (priority);
    	free (waiting_time); //start-arrival
    	free (remaining_time);
    	 
    	
    	
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
        }
        
        free(remaddr);
    	free(rem_id);
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
int Pindex;
//Node_priority **ready;
//int msgq_busy;
//int *busyaddr;
//int busyid;
//char*str,*str2,*str3,*str4;
void HPF()
{

    /////shared memory and semaphore sets for CPU_busy
	
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
	int msg_changed=0;
	P_msgbuff newProcess;
	newProcess.id=0;
	int rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);
	if (rec_val == -1)
	{
         perror("Error in receive");
         msg_changed=0;
        }
	else
	{
	printf("\nMessage received from server: %d\n", newProcess.id);
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
	}	//printf("id: %d",ready[0]->id);
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
		Node_priority* dequeued_proc= dequeue_priority(ready, &size);
		(*busyaddr)=1;
		printf("process of id %d is dequeued\n",dequeued_proc->id); 
		///fork
		Pindex=binarySearch(id,0,index_p-1,dequeued_proc->id);
		
		int pid=fork();
		int start_time=getClk();
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
		
		printf("remaining time of process %d is %d\n",id[Pindex],(*remaddr)[Pindex]);	    
	      }		    
				    
				    
	}
	if (isempty_priority(size) && newProcess.id!=-1)
	{
	rec_val = msgrcv(msgq_ready, &newProcess, sizeof(newProcess),0, !IPC_NOWAIT);

		if (rec_val == -1)
		{
		   perror("Error in receive");
		   msg_changed=0;
		}
		else
		{
		   printf("\nMessage received from server: %d\n", newProcess.id);
		   msg_changed=1;
		}
        }
        else
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
     //printf("waiting for last process %c\n",*(stataddr[Pindex]));
   }          
}


//Node_circular **readyf=NULL;
//Node_circular **readyr=NULL;

void RR(int t_slot)
{
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
	printf("\nMessage received from server: %d\n", newProcess.id);
	//msg_changed=1;
	}
	//if(msg_changed==1)
	//{	
		//add to data structure
		id[index_p]= newProcess.id;
		run[index_p]= newProcess.run;
		priority[index_p]= newProcess.priority;
		arrival[index_p]= newProcess.arrival;
		index_p++;
		//enqueue
		printf("before enqueue\n");
		//enqueue_circular(newProcess.id, readyf, readyr, newProcess.run);
		enqueue_circular(q, newProcess.id, newProcess.run);  
	//}	
	        printf("after enqueue process id=%d\n",q->front->id);
	//printf("id: %d",ready[0]->id);
	        int pid=fork();
	        int start_time=getClk();
	        printf("after forking pid=%d\n",pid);
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
      		Pindex=binarySearch(id,0,index_p-1,q->front->id);
      		printf("index in schedular=%d\n",Pindex);
      		
      		
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
			
			length = snprintf( NULL, 0, "%d", start_time );
			str5 = malloc( length + 1 );
			snprintf( str5, length + 1, "%d", start_time );

      		execlp("./process.out", "process.out",str,str2,str3,str4,str5, NULL);
		}
		else //parent scheduler
     	     {
     		printf("parent: next cycle\n");
      		
		   // int start_slot = getClk();
	  		slot = t_slot;
	  	//printf("start_slot=%d\n",start_slot);	
	  	printf("t_slot=%d\n",t_slot);	
	  		
		while(t_slot!=getClk()-start_time)
		{
	  	 //slot = slot - (getClk()-start_slot);
	  	 //printf("slot in schedular=%d\n",slot);
	  	 //printf("clock=%d\n",getClk());
	  	 
		}
		printf("clock in schedular=%d\n",getClk());
		//kill(pid,SIGUSR1);
		kill(pid,SIGSTOP);
		printf("remaining time of process %d is %d\n",id[Pindex],(*remaddr)[Pindex]);
		sleep(6);
		kill(pid,SIGCONT);
		//sleep(5);	    
	      }		    
	         
	        
	 
	/*while(newProcess.id!=-1 || !isempty_circular(readyf, readyr))
	{
	  Node_circular* dequeued_proc= dequeue_circular(readyf, readyr);
		//(*busyaddr)=1;
		printf("process of id %d is dequeued\n",dequeued_proc->id); 
		///fork
		Pindex=binarySearch(id,0,index_p-1,dequeued_proc->id);
		int pid=fork();
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
      		int Pindex=binarySearch(id,0,index_p-1,dequeued_proc->id);
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

      		execlp("./process.out", "process.out",str,str2,str3,str4, NULL);
		}
		else //parent scheduler
     	     {
     		printf("parent: next cycle\n");
      		
		   /*rec_val = msgrcv(msgq_ready, &CPU_busy, sizeof(CPU_busy), 0, !IPC_NOWAIT);
				if (rec_val == -1 && errno==ENOMSG)
				    printf("not changed yet\n");
		//counter++;	
		
		printf("remaining time of process %d is %d\n",id[Pindex],(*remaddr));	    
	      }*/		    
	  
}

/*void clearResources(int signum)
{
printf("schedular killed"); 
  /*for (int i=0; i<3; i++)
   	printf("id: %d\n",ready[i]->id);
   	free(str2);
   	free(str);
   	free(str3);
   	free(str4);
   	
   	free (id);
    	free (arrival);
    	free (run);
    	free (priority);
    	free (waiting_time); //start-arrival
    	free (remaining_time);
    	free (status); 
    	
    	free(stataddr);
    	free(stat_id);
    	free(ready);
    	//free(readyf);
    	//free(readyr);
   	//msgctl(msgq_busy, IPC_RMID, 0);

   	//dettach
	shmdt(busyaddr);
	
	//delete shared memory
	shmctl(busyid, IPC_RMID, 0);
	
	//dettach
	shmdt(remaddr);
	
	//delete shared memory
	shmctl(remid, IPC_RMID, 0);
	
	//dettach
	//delete shared memory
	for (int i=0; i<n; i++)
	{
	shmdt(stataddr[i]);
	shmctl(stat_id[i], IPC_RMID, 0);
        }
   	kill(getpid(),SIGKILL);
}*/




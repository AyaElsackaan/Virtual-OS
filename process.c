#include "headers.h"

/* Modify this file as needed*/
//int remainingtime;

//void pause(int signum);
void resume(int signum); 
void pre_empt(int signum); 
/*arguments:
*	1: running time
*	2: time slot
*	3: ID
*/
int msgq_busy;
int runningtime;
int starttime;
int main(int agrc, char * argv[])
{
//running time,id,
	signal(SIGUSR1,pre_empt);
	printf("process started\n");
	//signal(SIGCONT,resume);
       initClk();
     runningtime= atoi(argv[1]);
    int id= atoi(argv[2]);
    int myindex= atoi(argv[3]);
    int n= atoi(argv[4]);
    starttime= atoi(argv[5]);
    
    //shared memory of busy id
    int key_id_busy = ftok("keyfile", 'B');
	int busyid = shmget(key_id_busy, sizeof(int), IPC_CREAT | 0666);
	if (busyid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID busy= %d\n", busyid);
        
        int *busyaddr = (int*)shmat(busyid, (void *)0, 0); //attach shred memo
	if ((long)busyaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	//(*busyaddr)=1; //CPU is busy
    //----------------------------------------------------// 
    
    //shared memory of status//
    int key_id_stat = ftok("keyfile", myindex+1);
    int stat_id = shmget(key_id_stat, sizeof(char), IPC_CREAT | 0666);
	if (stat_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID status= %d\n", stat_id);
        
    char *stataddr = (char*)shmat(stat_id, (void *)0, 0); //attach shared memory
	if ((long)stataddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	printf("process %d myindex=%d\n",id,myindex);
    	(*stataddr)='S';
    	printf("process %d status=%c\n",id,(*stataddr));
    //----------------------------------------------//	
    //shared memory remaining time//
	/*int key_id_rem = ftok("keyfile", 'M');
	int remid = shmget(key_id_rem, sizeof(int), IPC_CREAT | 0666);
	if (remid == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID remaining= %d\n", remid);
        
        int *remaddr = (int*)shmat(remid, (void *)0, 0); //attach shred memo
	if ((long)remaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}*/
     
   //------------------------------------------------------------//
   //shared memory remaining time//
   int key_id_rem = ftok("remainingfile", myindex+1);
    int rem_id = shmget(key_id_rem, sizeof(int), IPC_CREAT | 0666);
	if (rem_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID remaining= %d\n", rem_id);
        
    int *remaddr = (int*)shmat(rem_id, (void *)0, 0); //attach shared memory
	if ((long)remaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	printf("process %d myindex=%d\n",id,myindex);
    //--------------------------------------------------------------//
    //shared memory waiting time//
   int key_id_wait = ftok("waitingfile", myindex+1);
    int wait_id = shmget(key_id_wait, sizeof(int), IPC_CREAT | 0666);
	if (wait_id == -1)
    {
        perror("Error in create");
        exit(-1);
    }
    else
        printf("\nShared memory ID waiting= %d\n", wait_id);
        
    int *waitaddr = (int*)shmat(wait_id, (void *)0, 0); //attach shared memory
	if ((long)waitaddr == -1)
	{
	    perror("Error in attach in server");
	    exit(-1);
	}
	printf("process %d myindex=%d\n",id,myindex);
    //--------------------------------------------------------------//	   
   	   
    //key_t key_busy;
    
	//create/get message queue
    /*key_busy = ftok("keyfile", 'B');  //unique
    
    msgq_busy = msgget(key_busy, 0666 | IPC_CREAT); 
    if (msgq_busy == -1)
    {
        perror("Error in creating busy");
        exit(-1);
    }
    printf("Message Queue ID  (busy)= %d\n", msgq_busy);
    int CPU_busy=1;
    int send_val = msgsnd(msgq_busy, &CPU_busy, sizeof(CPU_busy), !IPC_NOWAIT); 

		if (send_val == -1)
		    perror("Error in send");
		printf("busy=%d\n",CPU_busy);*/
		
    
     printf("id %d runningtime=%d\n",id,runningtime);
    //TODO it needs to get the remaining time from somewhere
    //int time_waited_so_far=0;
    (*remaddr) = runningtime;
    //int starttime= getClk();
   printf("id %d start:%d\n",id,starttime);
    while ((*remaddr) > 0)
    {
         (*remaddr)=runningtime-(getClk()-starttime-(*waitaddr));
         //remaddr[myindex]=remainingtime;
         //printf("rem:%d\n",remainingtime);
         //(*remaddr)=remainingtime;
         //printf("remaining time = %d\n",(*remaddr));
    }
    int finishtime=getClk();
    (*busyaddr)=0;
    printf("id %d ,finish time=%d\n",id,finishtime);
    
   	 /*CPU_busy=10;
   	 printf("id %d before send finishtime=%d\n",id,finishtime);
     send_val = msgsnd(msgq_busy, &CPU_busy, sizeof(CPU_busy), !IPC_NOWAIT); 

		if (send_val == -1)
		    perror("Error in send");*/
    printf("busy=%d\n",*busyaddr);

    //kill(getppid(),SIGUSR1);
    (*stataddr)='T';
    printf("process %d status=%c\n",id,(*stataddr));
    //dettach
    shmdt(busyaddr);
    shmdt(stataddr);
    shmdt(remaddr);
    shmdt(waitaddr);	
    destroyClk(false);
    return 0;
}


void pre_empt(int signum)
{
  printf("process paused clock=%d\n",getClk());
//  printf("process remaining time from process handler=%d\n",remainingtime);
	//exit(remainingtime); //paused
}
void resume(int signum)
{
	printf("\nresumed\n");
	printf("running time=%d, start time=%d\n",runningtime,starttime);
	//exit(remainingtime);
}




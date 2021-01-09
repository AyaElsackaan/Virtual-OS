#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

//void pause(int signum);
void resume(int signum); 

/*arguments:
*	1: running time
*	2: time slot
*	3: ID
*/
int msgq_busy;
int main(int agrc, char * argv[])
{
//running time,id,
	//signal(SIGUSR1,pause);
	printf("process started\n");
	signal(SIGUSR2,resume);
    initClk();
    
     //busy id
    int key_id_busy = ftok("keyfile", 'B');
	int busyid = shmget(key_id_busy, 256, IPC_CREAT | 0666);
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
	(*busyaddr)=1; //CPU is busy
    //----------------------------------------------------//    
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
		
    int runningtime= atoi(argv[1]);
    int id= atoi(argv[2]);
     printf("id %d runningtime=%d\n",id,runningtime);
    //TODO it needs to get the remaining time from somewhere
    
    int remainingtime = runningtime;
    int starttime= getClk();
   printf("id %d start:%d\n",id,starttime);
    while (remainingtime > 0)
    {
         remainingtime=runningtime-(getClk()-starttime);
         //printf("rem:%d\n",remainingtime);
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

    //dettach
    shmdt(busyaddr);	
    destroyClk(false);
    return 0;
}

/*
void pause(int signum)
{

	exit(remainingtime); //paused
}*/
void resume(int signum)
{
	printf("\nresumed\n");
	exit(remainingtime);
}




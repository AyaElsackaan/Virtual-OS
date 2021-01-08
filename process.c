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
    int CPU_busy=1;
    int send_val = msgsnd(msgq_busy, &CPU_busy, sizeof(CPU_busy), !IPC_NOWAIT); 

		if (send_val == -1)
		    perror("Error in send");
		printf("busy=%d\n",CPU_busy);
    int runningtime= atoi(argv[1]);
    
    //TODO it needs to get the remaining time from somewhere
    
    int remainingtime = runningtime;
    int starttime= getClk();
   
    while (remainingtime > 0)
    {
         remainingtime=runningtime-(getClk()-starttime);
    }
    int finishtime=getClk()-starttime;
    
    
   	 CPU_busy=10;
     send_val = msgsnd(msgq_busy, &CPU_busy, sizeof(CPU_busy), !IPC_NOWAIT); 

		if (send_val == -1)
		    perror("Error in send");
		printf("busy=%d\n",CPU_busy);
		
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




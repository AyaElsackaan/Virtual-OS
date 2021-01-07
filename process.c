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
int main(int agrc, char * argv[])
{
//running time,id,
	//signal(SIGUSR1,pause);
	signal(SIGUSR2,resume);
    initClk();
    int runningtime= atoi(argv[1]);
    
    //TODO it needs to get the remaining time from somewhere
    
    int remainingtime = runningtime;
    int starttime= getClk();
   
    while (remainingtime > 0)
    {
         remainingtime=runningtime-getClk();
    }
    int finishtime=getClk()-starttime;
    
    destroyClk(false);
    exit(0); //terminated
    
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




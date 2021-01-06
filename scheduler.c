#include "headers.h"

/*arguments:
 * 	1:algo 
 *	2:time slot
 *	3: 
 */
int main(int argc, char * argv[])
{
	//TODO implement the scheduler :) 
	printf("\nstarted\n\n");
    initClk();
    printf("%d ", argc);
    for(int i=0; i<argc; i++)
    {
    printf("%d \n", atoi(argv[0]));
    }
    
    int algo= atoi(argv[1]);
    int timeslot=atoi(argv[2]);
    int n=atoi(argv[3]);
     
    //PCB : id, run time, remainig, state, waiting
    int id[n];
    int arrival[n];
    int run[n];
    int priority[n];
    int waiting_time[n]; //start-arrival
    int remaining_time[n];
    char status[n]; //R:running, T:terminated, P: paused
    
    int j=4;
     for(int i=0; i<n; i++)
 	 {  	
 	 	id[i]=atoi(argv[j]);
 	   	j++;
 	 }
	 for(int i=0; i<n; i++)
     {
    	arrival[i]=atoi(argv[j]);
	   	j++;
     }
     for(int i=0; i<n; i++)
 	 {
 	    run[i]=atoi(argv[j]);
 	   	j++;
     }
     for(int i=0; i<n; i++)
     {
 	   	priority[i]=atoi(argv[j]);
 	   	j++;
     }
     
     
 	    
    
    //upon termination release the clock resources.
    
    destroyClk(true);
}

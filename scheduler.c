#include "headers.h"

/*arguments:
 * 	1:algo 
 *	2:time slot
 *	3: 
 */
int main(int argc, char * argv[])
{
    initClk();
    printf("%d ", argc);
    for(int i=0; i<argc; i++)
    {
    printf("%d \n", atoi(argv[0]));
    }
    //printf("%d ", id[0]);
    //PCB : id, run time, remainig, state, waiting
   /* int id[n];
    int waiting_time[n];
    int running_time[n];
    int priority[n];
    int remaining_time[n];
    char status[n];
    
    */
    //TODO implement the scheduler :)
    //upon termination release the clock resources.
    
    destroyClk(true);
}

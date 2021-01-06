#include "data_structures.h"

void clearResources(int);
int getlines();
void readfile(int *id,int *arrival,int *run, int *priority, int lines);

int main(int argc, char * argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    int n=getlines();
    int id[n];
    int arrival[n];
    int run[n];
    int priority[n];
    readfile(id,arrival,run,priority,n);
    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int algo;  //1-for HPF, 2-SRTN, 3-RR
    int timeslot=0; //parameter for RR
  
    printf("Please enter the desired algorithm number (1-for HPF,2-for SRTN,3-for RR):");
    scanf("%d",&algo);
    if (algo==3)
    {
     printf("Please enter the timeslot for RR:");
     scanf("%d",&timeslot);
    }
    printf("algo=%d and timeslot=%d\n",algo,timeslot); 
    // 3. Initiate and create the scheduler and clock processes.
    int pid;
    int flag=0;
    for (int i=0; i<2; i++)
    {
      pid=fork();
      if (pid==0)
     {
      flag=i+1;
      break;
     }
    }
    if (pid == -1)
  	perror("error in fork");
  	
  else if (pid == 0)
 	{ 
 	  if (flag==1)
 	  {
 	  	printf("i am the child (scheduler), pid=%d\n",getpid());
 	    char*argv[4*n+4];
 	    argv[0]="scheduler.out";
 	    sprintf(argv[1], "%d", algo);
 	    sprintf(argv[2], "%d", timeslot);
 	    sprintf(argv[3], "%d", n);
 	    int j=4;
 	    for(int i=0; i<n; i++)
 	    {
 	    	sprintf(argv[j], "%d", id[i]);
 	    	j++;
 	    }
		for(int i=0; i<n; i++)
 	    {
 	    	sprintf(argv[j], "%d", arrival[i]);
 	    	j++;
 	    }
 	    for(int i=0; i<n; i++)
 	    {
 	    	sprintf(argv[j], "%d", run[i]);
 	    	j++;
 	    }
 	    for(int i=0; i<n; i++)
 	    {
 	    	sprintf(argv[j], "%d", priority[i]);
 	    	j++;
 	    }
 	    argv[4*n+3]=NULL;
 	    execvp("./scheduler.out", argv);
 	    //execlp("./clk.out","clk.out",NULL);
 	    //execlp("./scheduler.out" ,"scheduler.out",NULL);
 	    printf("\nstill here\n");
 	  
 	  ///////////////
 	 
 	  }
 	  else if (flag==2)
 	  {
 	     printf("i am the child (clock), pid=%d\n",getpid());
 	  	execlp("./clk.out","clk.out",NULL);
 	  }
        }
    // 4. Use this function after creating the clock process to initialize clock
    else
   {
     initClk();
    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    
    
    //create a message queue
    key_t ready_id;
    
    ready_id = ftok("keyfile", 'R');  //unique

    int msgq_ready = msgget(ready_id, 0666 | IPC_CREAT); 
    if (msgq_ready == -1)
    {
        perror("Error in creating ready");
        exit(-1);
    }
    printf("Message Queue ID  (ready)= %d\n", msgq_ready);

    
    /////////////////////////////
   /* if (algo == 3)
    {
      Node_circular *ready_front=NULL;
      Node_circular *ready_rear=NULL;
    }
    else
    {
      Node_priority *ready = (Node_priority*)malloc(sizeof(Node_priority)*n);
    }*/
    // 6. Send the information to the scheduler at the appropriate time.
    

    //for(int i=0; i<n; i++)
    //{
    //  if (arrival[i] <= x)
   //   {
   //     
   //   }
    //}
    
    // 7. Clear clock resources
    //int temp;
    //wait(&temp); wait(&temp);
    destroyClk(true);
   }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    kill(getpid(),SIGKILL);
}

int getlines()
{
  FILE *fp;
  fp = fopen("processes.txt","r");
  if (fp==NULL)
    {
      printf("\n can't open file");
    }
  //counting processes in file  
    char chr;
    int count_lines=0;
    int hash_count=0;
    chr = getc(fp);
    while (chr != EOF)
    {
        //Count whenever comment line is encountered
       if (chr == '#')
       {
         hash_count = hash_count+1;
       }
       //count whenever new line is encountered
        if (chr == '\n')
        {
            count_lines = count_lines + 1;
        }
        //take next character from file.
       chr = getc(fp);
    }
   int lines=count_lines-hash_count; //actual info
   printf("There are %d lines in file\n", lines);
   fclose(fp); //close file to begin scanning info
   return lines;
}

void readfile(int *id,int *arrival,int *run, int *priority, int lines)
{
  FILE *fp;
 //getting data  
  fp = fopen("processes.txt", "r");
  /*int id[lines];
  int arrival[lines];
  int run[lines];
  int priority[lines];*/ 
  fscanf(fp, "%*[^\n]\n");
  
 for (int i=0; i<lines; i++)
  {
    fscanf(fp,"%d", &id[i]);
    fscanf(fp,"%d", &arrival[i]);
    fscanf(fp,"%d", &run[i]);
    fscanf(fp,"%d", &priority[i]);
  } 
  for (int i=0; i<lines; i++)
  {
    printf("%d\t%d\t%d\t%d\n", id[i], arrival[i], run[i], priority[i]);  
  }
  fclose(fp); //close file 
}

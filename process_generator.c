#include "headers.h"
#include "string.h"

char* str=NULL;
char* str2=NULL;
char* str3=NULL;
void clearResources(int);
int getlines();
void readfile(int *id,int *arrival,int *run, int *priority, int lines);
int msgq_ready;
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
 	  	
 	  	//arguments for scheduler
 	    int length = snprintf( NULL, 0, "%d", algo );
		str = malloc( length + 1 );
		snprintf( str, length + 1, "%d", algo );
		
		length = snprintf( NULL, 0, "%d", timeslot );
		str2 = malloc( length + 1 );
		snprintf( str2, length + 1, "%d", timeslot );
		
		length = snprintf( NULL, 0, "%d", n );
		str3 = malloc( length + 1 );
		snprintf( str3, length + 1, "%d", n );
		
 	  execlp("./scheduler.out","scheduler.out",str,str2, str3,NULL);
 	  
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
    P_msgbuff processes[n];
    for(int i=0; i<n; i++)
    {
    	processes[i].id=id[i];
    	processes[i].arrival=arrival[i];
    	processes[i].run=run[i];
    	processes[i].priority=priority[i];
    	//Processes[i].memory=memory[i];
    }
        //create a message queue for scheduler
    key_t ready_id;
    
    ready_id = ftok("keyfile", 'R');  //unique

    msgq_ready = msgget(ready_id, 0666 | IPC_CREAT); 
    if (msgq_ready == -1)
    {
        perror("Error in creating ready");
        exit(-1);
    }
    printf("Message Queue ID  (ready)= %d\n", msgq_ready);

    // 6. Send the information to the scheduler at the appropriate time.
    int i=0; //loop on processes
    while (i<n)
    {
    
    	x=getClk();

    	while(processes[i].arrival>x)
    	{ 
    	//printf("%d",x);
    		x=getClk();
    	}
    	
    	int send_val = msgsnd(msgq_ready, &processes[i],sizeof(processes[i]), !IPC_NOWAIT); 

		if (send_val == -1)
	    	perror("Errror in send");
		printf("will send %d\n", processes[i].id);
		i++;
	}
   //int sid;
   //wait(&sid); //wait for scheduler
   
    // 7. Clear clock resources
    destroyClk(true);
   }
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    //delete msg queue
    free(str);
	free(str2);
	free(str3);
   msgctl(msgq_ready, IPC_RMID, 0);
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

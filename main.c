#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>
#include <stdbool.h>

int shmkey = 112369;

typedef struct PCB
{
  int occupied;			// either true or false
  pid_t pid;			// PID of this child
  int startSeconds;		// time this was forked
  int startNano;		// time this was forked (nano seconds)
} PCB;

void helpFunction ()
{
  printf
    ("The function call should go as follows: ./oss [-h] [-n proc] [-s simu] [-t timeLimit] ");
  printf
    ("where -h displays this help function and terminates, proc is the number of processes you want to run, simu is the number of simultaneous");
  printf
    ("processes that run and timeLimit is approximately the time limit each child process runs.");
}

void incrementClock(int* shmSec, int* shmNano){
    *shmNano+=1;
    if (*shmNano >= 100000){
    *shmSec+=1;
    *shmNano=0;
    }
}

int forker (int totaltoLaunch, int simulLimit, int timeLimit, int* totalLaunched, PCB * processTable, int* shmSec, int*shmNano)
{
  pid_t pid;// RECREATES AN EXTRA BECAUSE TOTALLAUNCHED HAS INCREMENTED BUT FUNCTION HASNT TERMINATED 
  
    if (*totalLaunched == simulLimit || totaltoLaunch == 0)
    {
      return (totaltoLaunch);
    }
  else if (totaltoLaunch > 0)
    {
      if ((pid = fork ()) < 0) // FORK HERE
	{
	  perror ("fork");
	}
      else if (pid == 0)
	{
	    srand(time(NULL)*getpid());
        int rSec = rand() % timeLimit + 1;
        int rNano = rand()% (100000);
        
        char* args[]={"./worker",rSec,rNano,0};
        execlp(args[0],args[0],args[1],args[2],args[3]);
	}
      else if (pid > 0)
	{
  processTable[*totalLaunched].occupied = 1;
  processTable[*totalLaunched].pid = pid;
  processTable[*totalLaunched].startSeconds = *shmSec;
  processTable[*totalLaunched].startNano = *shmNano;
	    *totalLaunched += 1;
	  forker (totaltoLaunch - 1, simulLimit, timeLimit, totalLaunched,
		  processTable,shmSec,shmNano);
	}
    }
  else
    return (0);
}

bool checkifChildTerminated(int status, PCB *processTable)
{
    int pid = waitpid(-1, &status, WNOHANG);
    int i = 0;
    if (pid > 0){
    for (i; i < 20; i++){
        if (processTable[i].pid == pid)
            processTable[i].occupied = 0;
    }
    return true;
    }
    else if (pid == 0)
        return false;
}

void printStruct (struct PCB *processTable, int size)
{
  printf ("Process Table: \n");
  printf ("ENTRY  OCCUPIED  PID  STARTS  STARTN\n");
  int i = 0;
  for (i; i < size; i++)
    {
      printf ("%d        %d       %d    %d        %d\n", i,
	      processTable[i].occupied, processTable[i].pid,
	      processTable[i].startSeconds, processTable[i].startNano);
    }
}

char *x = NULL;
char *y = NULL;
char *z = NULL;


int main (int argc, char **argv)
{
  int option;
  while ((option = getopt (argc, argv, "n:s:t:h")) != -1)
    {
      switch (option)
	{
	case 'h':
	  helpFunction ();
	  return 0;
	case 'n':
	  x = optarg;
	  break;
	case 's':
	  y = optarg;
	  break;
	case 't':
	  z = optarg;
	  break;
	}
    }
    
    //INITIALIZE ALL VARIABLES
  int totaltoLaunch = 0;	// int to hold -n arg
  int simulLimit = 0;		// int to hold -s arg
  int totalLaunched = 0;	// int to count total children launched
  totaltoLaunch = atoi (x);	// casting the argv to ints
  simulLimit = atoi (y);
  int timeLimit = atoi (z);
  int status;
  int exCess;
  int *shmSec;
  int *shmNano;
  PCB processTable[20];
  bool stillChildrenRunning = true;
  bool initialLaunch = false;
  

  int shmid = shmget (shmkey, 2 * sizeof (int), 0777 | IPC_CREAT);	// create shared memory
  if (shmid == -1)
    {
      printf ("ERROR IN SHMGET\n");
      exit (0);
    }
    
    //Clock
  shmSec = (int *) (shmat (shmid, 0, 0)); // create clock variables
  shmNano = shmSec + 1;
  *shmSec = 0; // initialize clock to zero
  *shmNano = 0;
    
    
   while(stillChildrenRunning){
  // FORK CHILDREN 
    incrementClock(shmSec,shmNano);
    if (*shmNano == 50000){
    printStruct (processTable, 20);
    }
    if(initialLaunch == false){
        exCess = forker (totaltoLaunch, simulLimit, timeLimit, &totalLaunched, processTable, shmSec, shmNano);
        initialLaunch = true;
    }
    bool childHasTerminated = false;
    childHasTerminated = checkifChildTerminated(status, processTable);
    if(childHasTerminated == true){
        if (exCess > 0){
            forker (1, 1, timeLimit, &totalLaunched, processTable, shmSec,shmNano);
            exCess--;
        }
        totaltoLaunch--;
    }
    if (totaltoLaunch == 0)
            stillChildrenRunning = false;
    }

printStruct (processTable, 20);
  //DETACH SHARED MEMORY
  shmdt (shmSec);
  shmctl (shmid, IPC_RMID, NULL);
  return (0);
}




// MADE THIS COMMENT

















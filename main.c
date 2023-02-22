#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int shmkey = 112369;

struct PCB {
    int occupied; // either true or false
    pid_t pid; // PID of this child
    int startSeconds; // time this was forked
    int startNano; // time this was forked (nano seconds)
};

void helpFunction()
{
    printf("The function call should go as follows: ./oss [-h] [-n proc] [-s simu] [-t timeLimit] ");
    printf("where -h displays this help function and terminates, proc is the number of processes you want to run, simu is the number of simultaneous");
    printf("processes that run and timeLimit is approximately the time limit each child process runs.");
}
int forker(int totaltoLaunch, int simulLimit, int timeLimit, int totalLaunched,int* stillChildrentoLaunch,struct PCB *processTable)
{
    pid_t pid;
    if(totalLaunched==simulLimit){
        return (totaltoLaunch);
    }
    else if(totaltoLaunch > 0)
    {
        if ((pid = fork()) < 0)
        {
            perror("fork");
        }
        else if (pid == 0)
        {
            processTable[totalLaunched].occupied = 1;
            processTable[totalLaunched].pid = pid;
            processTable[totalLaunched].startSeconds = 0;
            processTable[totalLaunched].startNano = 1;
            
        printf("I am a child and supposed to run %d seconds\n", timeLimit);
        exit(0);

        }
        else if(pid > 0)
        {
            forker(totaltoLaunch - 1, simulLimit, timeLimit, totalLaunched + 1, stillChildrentoLaunch, processTable);
        }
    }
    else{
        *stillChildrentoLaunch = 1;
        return (0);
    }
}

void printStruct(struct PCB *processTable, int size){
    printf("Process Table: \n");
    printf("ENTRY  OCCUPIED  PID  STARTS  STARTN\n");
    for (int i = 0; i<size; i++){
        printf("%d        %d        %d        %d        %d\n", i,processTable->occupied,processTable->pid,processTable->startSeconds,processTable->startNano);
    }
}

char *x = NULL;
char *y = NULL;
char *z = NULL;
struct PCB processTable[20];


int main(int argc, char** argv)
{
    int option;
    while((option=getopt(argc,argv, "n:s:t:h"))!=-1){
        switch(option){
            case 'h':
            helpFunction();
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
   int totaltoLaunch=0; // int to hold -n arg
   int simulLimit=0; // int to hold -s arg
   int totalLaunched=0; // int to count total children launched
   totaltoLaunch = atoi(x); // casting the argv to ints
   simulLimit = atoi(y);
   int timeLimit = atoi(z);
   int status;
   int* shmPtr1;
   int* stillChildrentoLaunch = 0; // bool to know if children need to be launched

   
  int shmid = shmget(shmkey,2*sizeof(int),0777|IPC_CREAT); // create shared memory
  if(shmid==-1)
  {
      printf("ERROR IN SHMGET\n");
      exit(0);
  }
   shmPtr1 = (int*)(shmat(shmid,0,0));
   *shmPtr1 = 0;
       *shmPtr1++;
       int exCess = forker(totaltoLaunch,simulLimit,timeLimit,totalLaunched,stillChildrentoLaunch,processTable);
       printf("%d\n", exCess);
       if (exCess>0)
       for(exCess;exCess>0;exCess--){
           forker(1,1,timeLimit,totalLaunched,stillChildrentoLaunch,processTable);
    }
    pid_t pid = wait(&status);
    printStruct(processTable,20);
    shmdt(shmPtr1); // detach shared memory
    shmctl(shmid,IPC_RMID,NULL);
    return(0);
}











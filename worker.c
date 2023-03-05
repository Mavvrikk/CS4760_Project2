#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <time.h>

int shmkey = 112369;

int main (int argc, char** argv){



int* shmSec;
int* shmNano;   
  int shmid = shmget(shmkey,2*sizeof(int),0777|IPC_CREAT); // create shared memory
  if(shmid==-1)
  {
      printf("ERROR IN SHMGET\n");
      exit(0);
  }
   shmSec = (int*)(shmat(shmid,0,0));
   shmNano = shmSec + 1;    
	 int runTime;
        runTime = atoi(argv[1]); // timeLimit
srand(time(NULL)*getpid());
int rSec = rand() % runTime + 1;
int rNano = rand() % 1000000;
int termTime = rSec + *shmSec;
int termTimeNano = rNano + *shmNano;
int timeLimit = 1;
int loopsExecuted = 1;
printf("WORKER PID: %d PPID: %d SysClock: %d SysclockNano: %d TermTime: %d TermTimeNano: %d\n",getpid(),getppid(),*shmSec,*shmNano,termTime,termTimeNano);
printf("--Just Starting\n");
 while(timeLimit == 1){
	if (runTime != *shmSec){
		runTime = *shmSec;
		if (termTime <= runTime){
			printf("WORKER PID: %d PPID: %d SysClock: %d SysclockNano: %d TermTime: %d TermTimeNano: %d \n", getpid(), getppid(),*shmSec, *shmNano, termTime, termTimeNano);
			printf("--Now Terminating\n");
			timeLimit = 0;
		}
		printf("WORKER PID: %d PPID: %d SysClock: %d SysclockNano: %d TermTime: %d TermTimeNano: %d \n", getpid(), getppid(),*shmSec, *shmNano, termTime, termTimeNano);
		printf("--%d seconds have passed since starting\n", loopsExecuted);
		loopsExecuted++;
	}
}
return(0);       
}


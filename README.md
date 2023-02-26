# CS4760_Project2
This second project for Operating Systems was adding functionality to our first project. In project one we had to launch X child processes with a simultaneous limit. In project 2, we created a clock in shared memory that the children were to look at, report their findings and terminate upon a randomly determined time. As a precaution, this project was outfitted with a signal to catch ctrl+C and a timeout of 60 seconds in case the children ran too long. To run this file, simply use the makefile, to 'make' both oss.c and worker.c and then call the file with a -h option to pull a helper function OR you can the use the following options:
-n Number of child processes TOTAL to launch
-s Simultaneous limit on number of processes allowed to run
-t Sets random time interval between 1 and -t each child will run. 

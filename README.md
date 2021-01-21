# Virtual-OS
An implementation of a scheduler and a memeory manager to simulate the function of a mini operating system.

This is CMPN303 course project - Faculty of Engineering, Cairo University.

## Algorithms implemented
Scheduler: non pre-emptive Highest Priority First (HPF), Round Robin (RR), pre-emptive Shortest Remaining Time Next (SRTN).

Memory Manager: Buddy System Allocation Algorithm

## To use this repo
headers.h contains clk functions, it should be included anywhere the clock functions are used. This clock is just an emulation for synchronization purposes.

To get time call: getClk();

You can use Makefile to build and run your project

If you add a file to your project add it to the build section in the Makefile

Always start the line with a tab in Makefile, it is its syntax

## To compile your project, use the command:
make

## To run your project, use the command:
make run

## Input File:
### processes.txt
A .txt file containing id, arrival time, running time, priority and required memory size of each process.

You can generate a random file using the command: ./test_generator.out

## Output Files:

### sheduler.log
Contains the timeline of start, pause, resume and finish of each process after running the selected algorithm.

### scheduler.perf
Contains the CPU utilization percentage, as well as both the mean and the standard deviation of the weighted turaround time.

### memory.log
Contains the timeline of memory allocation and deallocation after applying the buddy system allocation algorithm.

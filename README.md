# Virtual-OS
An implementation of a scheduler and a memeory manager to simulate the function of a mini operating system.

This is CMPN303 course project - Faculty of Engineering, Cairo University.

## Algorithms implemented
Sheduler: non pre-emptive Highest Priority First (HPF), Round Robin (RR), pre-emptive Shortest Remaining Time Next (SRTN).

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


#ifndef _PROCESS_QUEUE_H
#define _PROCESS_QUEUE_H

#define _POSIX_C_SOURCE 200809L // to deal with the crazy "implicit declaration of getline()
// OR #define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

typedef struct process
{
    char name[8];
    int entryTime; // time of entry to the system
    int burstTime; // tick to run
    int offTime; // time when cpu lost
    int waitTime; // accumulative waiting time
} PROCESS;

// process table functions

/***
 * constructor of the process table
 */
void createProcessTable(int capacity);

/***
 * adds any processes that arrive at the current time to ready queue
 */
void addArrivingProcessesToReadyQueue(int time);

/***
 * adds a process to the process table and expands the table if necessary
 */
void addProcessToTable(PROCESS process);

/***
 * determines if any processes in the process table still need to execute
 */
bool processesLeftToExecute();

/***
 * display the processes table
 */
void displayProcessTable();

// process ready queue functions

/***
 * constructor of the ready queue
 */
void createReadyQueue(int capacity);

/***
 * adds a process to the ready queue and expands it if necessary
 */
void addProcessToReadyQueue(PROCESS *process);

/***
 * fetches the first process from the ready queue
 */
PROCESS *fetchFirstProcessFromReadyQueue();

/***
 * fetches the shortest burstTime so that we don't have to mess up queue
 */
int fetchShortestProcessInReadyQueue();

/***
 * finds the shortest job in the ready queue
 */
PROCESS *findShortestProcessInReadyQueue();

/***
 * removes a process from the ready queue and fills the "hole"
 */
void removeProcessFromReadyQueue(PROCESS *process);

/***
 * displays the contents of the ready queue
 */
void displayQueue();

/***
 * calculates and prints the average wait time using information in the process table
 */
void printAverageWaitTime();

/***
 * clean up the process table
 */
void cleanUp();

#endif

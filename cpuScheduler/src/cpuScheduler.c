#include "cpuScheduler.h"

static int quantum;

int main(int argc, char **argv)
{

#ifdef _DEBUG
    if (argc > 1)
    {
        freopen(argv[1], "r", stdin);
        printf("Reading input from file %ss.\n\n", argv[1]);
    }
    else
        printf("Reading input from stdin.\n\n");
#endif

    int time = 0; // simulated time

    ALGORITHM_PARAMS parameters = {
            .cpu = NULL, .algorithm = "",
            .step = NULL, .quantum = 0,
            .time = 0
    }; // simulation parameters

    // read the algorithm type and time quantum if necessary
    scanf("%s", parameters.algorithm);

    //check which algorithm was passed, set values accordingly
    if (strcmp(parameters.algorithm, "RR") == 0)
    {
        scanf("%d", &parameters.quantum);
        quantum = parameters.quantum;
        parameters.step = &rrStep;
    }
    else if (strcmp(parameters.algorithm, "FCFS") == 0)
        parameters.step = &fcfsStep;
    else if (strcmp(parameters.algorithm, "SJF") == 0)
        parameters.step = &sjfStep;
    else if (strcmp(parameters.algorithm, "SRTF") == 0)
        parameters.step = &srtfStep;
    else
    {
        printf("The job type input is not a valid input!");
        exit(EXIT_FAILURE);
    }

    scanf("\n"); // skip over the end of line marker

    printf("\nALGORITHM: %s", parameters.algorithm);
    if (strcmp(parameters.algorithm, "RR") == 0)
        printf("%3d", parameters.quantum);
    printf("\n\n");

    createProcessTable(INITIAL_CAPACITY); //create process table
    createReadyQueue(INITIAL_CAPACITY); //create ready queue

    readProcessTable(); //populate global process table
    displayProcessTable();

    printf("SIMULATION:\n\n");

    while (processesLeftToExecute())
    {
        addArrivingProcessesToReadyQueue(time);

        parameters.time = time;

        //this is the step executor
        doStep(parameters.step, &parameters);

        displayTimeTick(time, parameters.cpu);

        if (parameters.cpu != NULL)
            parameters.cpu->burstTime--;

        time++;
    }

    printAverageWaitTime();

    cleanUp();

    return EXIT_SUCCESS;
}

/***
 * step executor
 */
void doStep(void (*func)(void *), void *param)
{
    func(param);
}

/***
 * function implementing a step of FCFS
 */
void fcfsStep(void *param)
{
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;

    //if the cpu has nothing currently executing
    if (p->cpu == NULL || p->cpu->burstTime == 0)
    {
        p->cpu = fetchFirstProcessFromReadyQueue(); //start executing the first process in the ready queue
        if (p->cpu != NULL)
            p->cpu->waitTime = p->time - p->cpu->entryTime; // update the wait time
    }
}

/***
 * function implementing a step of SJF
 */
void sjfStep(void *param)
{
// TODO: implement
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;
    //if nothing is executing
    if (p->cpu == NULL) {
        //get shortest process from ready queue (already implemented)
        p->cpu = findShortestProcessInReadyQueue();
    }
    //if process is executing
    if (p->cpu != NULL) {
        //if burst time left is 0
        if (p->cpu->burstTime == 0) {
            //find the next shortest process
            p->cpu = findShortestProcessInReadyQueue();
            //update wait time: time - entryTime (burst-- is done in while loop)
            p->cpu->waitTime = p->time - p->cpu->entryTime;
        }
    }
}

/***
 * function implementing a step of SRTF
 */
void srtfStep(void *param)
{
// TODO: implement
    ALGORITHM_PARAMS *p = (ALGORITHM_PARAMS *) param;
    //if no process is running
    if (p->cpu == NULL) {
        //get shortest process from ready queue
        p->cpu = findShortestProcessInReadyQueue();
    }
    //if process is running
    if (p->cpu != NULL) {
        //if burst time left is 0
        if (p->cpu->burstTime == 0){
            //find the next shortest process
            p->cpu = findShortestProcessInReadyQueue();
            //update wait time: time - entry - off
            p->cpu->waitTime = p->time - p->cpu->offTime;
        }
        //check queue for shortest process burst (already implemented)
        if (p->cpu->burstTime > fetchShortestProcessInReadyQueue()){
            //update offtime of process
            p->cpu->offTime = p->time;
            //add process back to ready queue
            addProcessToReadyQueue(p->cpu);
            //get the next shortest process
            p->cpu = findShortestProcessInReadyQueue();
        }

    }
}

/***
 * function implementing a step of RR
 */
void rrStep(void *param)
{

// TODO: implement

}

/***
 * fills the process table with processes from input
 */
int readProcessTable()
{
    PROCESS tempProcess = {
            .name = "",
            .entryTime = 0,
            .burstTime = 0,
            .offTime = 0,
            .waitTime = 0
    };

    char *line = NULL;
    char *currPos;
    size_t len = 0;

    int counter = 0;
    int offset = 0;

    while (getline(&line, &len, stdin) != -1)
    {
        currPos = line;
        sscanf(currPos, "%s%n", tempProcess.name, &offset);
        currPos += offset;
        sscanf(currPos, "%d%n", &tempProcess.entryTime, &offset);
        tempProcess.offTime = tempProcess.entryTime; // simplifies computation of the wait time
        currPos += offset;
        sscanf(currPos, "%d", &tempProcess.burstTime);
        addProcessToTable(tempProcess);
        counter++;
    }

    free(line);

    return counter;
}

void displayTimeTick(int time, PROCESS *cpu)
{
    printf("T%d:\nCPU: ", time);
    if (cpu == NULL)
        printf("<idle>\n");
    else
        printf("%s(%d)\n", cpu->name, cpu->burstTime);

    displayQueue();
    printf("\n\n");
}







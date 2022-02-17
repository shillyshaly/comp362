/**
* Name: Jamie Hernandez
* Lab/task: Lab 7
* Date: 10/9/2021
**/
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>

#define SLEEP_TIME ( (rand() % 5 + 1) * 1000)

void *philosopher(void *id);

void test(int i);

void putdown(int i);

void pickup(int i);

void initializationCode();

pthread_mutex_t *chopstick;

//condition (pthread_cond_t is initializer)
pthread_cond_t *self;

//signal mutex initializer
pthread_mutex_t monitorMutex = PTHREAD_MUTEX_INITIALIZER;

int numOfSeats, numOfTurns;

//enum states
enum {
    THINKING,
    HUNGRY,
    EATING
} *state;

void initializationCode() {
    for (int i = 0; i < numOfSeats; i++) {
        state[i] = THINKING;
    }
}

//checks for eating on each side of i (signal resumes a suspended thread)
void test(int i) {
    if ((state[(i + (numOfSeats - 1)) % numOfSeats] != EATING) && (state[i] == HUNGRY) &&
        (state[(i + 1) % numOfSeats] != EATING)) {
        state[i] = EATING;
        //slide 32
        //self[i].signal();
        pthread_cond_signal(&self[i]);
    }
}

//sets i to hungry, if i is hungry and not eating, i has to wait i think? (wait suspends thread)
void pickup(int i) {
    state[i] = HUNGRY;
    test(i);
    if (state[i] != EATING) {
        //slide 32
        //self[i].wait();
        pthread_cond_wait(&self[i], &monitorMutex);
    }
}

//sets i to thinking, test() for each side of i
void putdown(int i) {
    state[i] = THINKING;
    //test l/r neighbors
    test((i + (numOfSeats - 1)) % numOfSeats);
    test((i + 1) % numOfSeats);
}

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("Usage: philosophers <number of seats> <number of turns>");
        return 1;
    }

    numOfSeats = strtod(argv[1], NULL);
    numOfTurns = strtod(argv[2], NULL);

    chopstick = calloc(numOfSeats, sizeof(pthread_mutex_t));

    //allocate memory for state and self
    state = malloc(numOfSeats * sizeof(int *));
    self = calloc(numOfSeats, sizeof(pthread_cond_t));

    //initialize state here maybe??
    initializationCode();

    // set the seed for random number generator
    srand(time(NULL));

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    printf("Start a dinner for %d diners\n", numOfSeats);

    pthread_t philosopher_tid[numOfSeats];

    long i;
    for (i = 0; i < numOfSeats; i++)
        pthread_mutex_init(chopstick + i, NULL);

    for (i = 0; i < numOfSeats; i++)
        pthread_create(&philosopher_tid[i], NULL, philosopher, (void *) i);

    for (i = 0; i < numOfSeats; i++)
        pthread_join(philosopher_tid[i], NULL);

    for (i = 0; i < numOfSeats; i++)
        pthread_mutex_destroy(chopstick + i);

    printf("Dinner is no more.\n");

    return 0;
}

void *philosopher(void *num) {
    int id = (long) num;

    printf("Philsopher no. %d sits at the table.\n", id);

    // philosophers arrive at the table at various times
    usleep(SLEEP_TIME);

    int i;
    for (i = 0; i < numOfTurns; i++) {
        //pickup, pause, putdown??? hangs
        //lock, wait, unlock -> lock, signal, unlock??? hangs. not really sure if the locking is happening else where
        //lock, wait,pickup, eat, putdown, signal, unlock??

        printf("Philsopher no. %d gets hungry for the %d time!\n", id, i + 1);
        printf("Philsopher no. %d tries to grab chopstick %d\n", id, id);

        //lock
        //first_mutex???
        pthread_mutex_lock(&(chopstick[id]));
        printf("Philsopher no. %d has grabbed chopstick %d\n", id, id);
        usleep(SLEEP_TIME * 2);
        printf("Philsopher no. %d tries to grab chopstic %d\n", id, (id + 1) % numOfSeats);
        //second_mutex???
        pthread_mutex_lock(&(chopstick[(id + 1) % numOfSeats]));
        printf("Philsopher no. %d grabbed chopstick %d\n", id, (id + 1) % numOfSeats);
        //lock

        pickup(i);  //wait, pickup, eating

//        test(i);  //

        // YEEEAAAAH !!!
        printf("Philsopher no. %d eating\n", id);

        // usleep (DELAY);

        printf("Philsopher no. %d stopped eating\n", id);

        putdown(i);  //putdown

        test(i);  //signal

        //unlock
        pthread_mutex_unlock(&(chopstick[id]));
        printf("Philsopher no. %d has returned chopstick %d\n", id, id);
        usleep(SLEEP_TIME * 4);
        pthread_mutex_unlock(&(chopstick[(id + 1) % numOfSeats]));
        printf("Philsopher no. %d has returned chopstick %d\n", id, (id + 1) % numOfSeats);
        printf("Philsopher no. %d finished turn %d\n", id, i + 1);
        //unlock
    }

    printf(">>>>>> Philsopher no. %d finished meal. <<<<<<\n", id);

    pthread_exit(NULL);
}

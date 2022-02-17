/**
* Name: Jamie Hernandez
* Lab/task: Lab 10
* Date: 11/1/2021
**/

/**
 * This implements the LRU page-replacement algorithm.
 */

#include "../inc/lruList.h"

int *referenceString;
int refStringLength;

FRAME *pageTableTop;
FRAME *leastRecentlyUsed;
int pageTableSize;

int numberOfFramesPerProcess = 0;

// statistics
int numberOfFaults = 0;

int hitPageNumber;

/*
 * insert pages from a reference string into a page table and measure
 * the page fault rate
 */
int testLRU(int numOfFrames, int *refString, int refStrLen) {
    // TODO: implement
    pageTableTop = NULL;

    numberOfFramesPerProcess = numOfFrames;

    for (int i = 0; i < numOfFrames; i++) {
        FRAME *newFrame;
        newFrame = malloc(sizeof(FRAME));

        if (pageTableTop == NULL) {
            newFrame->up = NULL;
            newFrame->pageNumber = -1;
            newFrame->down = NULL;
        } else {
            pageTableTop->down = newFrame;
            newFrame->up = pageTableTop;
            newFrame->pageNumber = -1;
            newFrame->down = NULL;
            numberOfFaults++;
        }
        pageTableTop = newFrame;
    }
    while (pageTableTop->up != NULL) {
        pageTableTop = pageTableTop->up;
    }

    leastRecentlyUsed = pageTableTop;

    for (int i = 0; i < refStrLen; i++) {
        printf("%d ->\t", refString[i]);
        insertLRU(refString[i]);
    }

    return 0;
}

/*
 *	try to insert a page into the page table
 */
void insertLRU(int pageNumber) {
    // TODO: implement
    hitPageNumber = -1;

    FRAME *found = pageTableTop;

    if (pageTableSize == 0) {
        pageTableTop->pageNumber = pageNumber;
        leastRecentlyUsed = pageTableTop;
        pageTableSize++;
    } else {
        found = searchLRU(pageNumber);

        if (found == NULL) {
            pageTableSize++;
            numberOfFaults++;
            if (pageTableSize > numberOfFramesPerProcess) {
                leastRecentlyUsed = leastRecentlyUsed->up;
            } else {
                found = pageTableTop;
                int prevPage = pageNumber;
                int currPage = found->pageNumber;

                found->pageNumber = pageNumber;

                for (int i = 0; i < pageTableSize; i++) {
                    found->pageNumber = prevPage;
                    found = found->down;
                    prevPage = currPage;
                    currPage = found->pageNumber; //blows up here
                }
            }
        } else {
            if (hitPageNumber == 0) {}
            else if (hitPageNumber == pageTableSize) {
                found->down->up = found->down;
                leastRecentlyUsed = found->up;
                found->up = NULL;
                found->down = pageTableTop;
                pageTableTop->up = found;
                pageTableTop = found;
            } else {
                found->up->down = found->down;
                found->down->up = found->up;
                pageTableTop->up = found;
                found->down = found;
                found->up = NULL;
                pageTableTop = found;
            }
        }
    }
    displayLRU();

}

/**
 * Searches for page pageNumber in the page frame list
 * returns NULL if a frame with pageNumber was not found
 * otherwise, returns a reference to the frame with pageNumber
 */
FRAME *searchLRU(int pageNumber) {
    // TODO: implement - done
    FRAME *currFrame = pageTableTop;
    //if pageNumber is found in table, return index, else return null
    for (int i = 0; i < pageTableSize; i++) {
        if (currFrame->pageNumber == pageNumber) {
            hitPageNumber = i;
            return currFrame;
        }
        currFrame = currFrame->down;
    }
    return NULL;
}

void displayLRU() {
    // TODO: implement - done
    FRAME *currFrame = pageTableTop;
    //if hitPageNumber is not -1
    if (hitPageNumber != -1) {
        //loop table
        for (int i = 0; i < pageTableSize; i++) {
            if (i == 0) {
                printf("%d<\t", currFrame->pageNumber);
            } else {
                printf("%d\t", currFrame->pageNumber);
            }
            //next frame
            currFrame = currFrame->down;
        }
        printf("\n");
        hitPageNumber = -1;
    } else {
        for (int i = 0; i < pageTableSize; i++) {
            printf("%d", currFrame->pageNumber);
            if ((i + 1) != pageTableSize) {
                printf("\t");
            }
            currFrame = currFrame->down;
        }
        printf("*\n");
    }
}

void freePageTableLRU() {
    // TODO: implement
    while (pageTableTop->down != NULL) {
        pageTableTop = pageTableTop->down;
        pageTableTop->up = NULL;
        free(pageTableTop->up);
    }
    free(pageTableTop);
    free(leastRecentlyUsed);
}


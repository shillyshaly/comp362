/*
 *  replace.h
 */

#ifndef __REPLACE_H
#define __REPLACE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define FREE_SLOT -1

int testLRU(int, int[], int);
int testOPT(int, int[], int);

#endif // __REPLACE_H

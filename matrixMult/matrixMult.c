#include "matrixMult.h"

int main(int argc, char *argv[])
{
    if (freopen(argv[1], "r", stdin) == 0)
    oops("Cannot open the input file.\n", -1);

    int **a1, **b1, **c1, **a2, **b2, **c2; // matrices
    int m1, k1, n1, m2, k2, n2; // dimensions of the matices m x k and k x n

    allocateAndLoadMatrices(&a1, &b1, &c1, &m1, &k1, &n1);
    allocateAndLoadMatrices(&a2, &b2, &c2, &m2, &k2, &n2);

    // the real magic happens in here
    pthread_t **tids1 = multiply(a1, b1, c1, m1, k1, n1);
    pthread_t **tids2 = multiply(a2, b2, c2, m2, k2, n2);

    free_tids(tids1, m1);
    free_tids(tids2, m2);

    // dispaly results of matrix multiplication
    printf("\nMATRIX A1\n");
    displayMatrix(a1, m1, k1);
    freeMatrix(a1, m1);
    printf("\nMATRIX B1\n");
    displayMatrix(b1, k1, n1);
    freeMatrix(b1, k1);
    printf("\nMATRIX A1 x B1\n");
    displayMatrix(c1, m1, n1);
    freeMatrix(c1, m1);

    printf("\nMATRIX A2\n");
    displayMatrix(a2, m2, k2);
    freeMatrix(a2, m2);
    printf("\nMATRIX B2\n");
    displayMatrix(b2, k2, n2);
    freeMatrix(b2, k2);
    printf("\nMATRIX A2 x B2\n");
    displayMatrix(c2, m2, n2);
    freeMatrix(c2, m2);

    return 0;
}

void *matrixThread(void *param)
{
    // map the parameter onto the structure
    MATRIX_CELL *cell = (MATRIX_CELL *)param;

    // TODO: implement
    int stepr;
    // k = length of row/col
    for(stepr = 0; stepr < cell->k; stepr++) {
        cell->c[cell->i][cell->j] += (cell->a[cell->i][stepr]) *(cell->b[stepr][cell->j]);
    }

    free(cell);

    return NULL;
}

void allocateAndLoadMatrices(int ***a, int ***b, int ***c, int *m, int *k, int *n)
// takes pointers to two-dimensional matrices, so they can be allocated in here
// and used by the caller
// should call the function loadMatrix(int ***matrix, int m, int n)
{
    if (scanf("%d %d %d", m, k, n) == 0)
    oops("Cannot read matrix sizes.\n", -2);

    // TODO: implement
    int i;
    //mxk
    (*a) = (int **) malloc(*m * sizeof(int *));
    for (i = 0; i < *m; i++) {
        (*a)[i] = (int *) malloc(*k * sizeof(int));
    }

    // k x n
    (*b) = (int **) malloc(*k * sizeof(int *));
    for (i = 0; i < *k; i++) {
        (*b)[i] = (int *) malloc(*n * sizeof(int));
    }
    // mxn
    (*c) = (int **) malloc(*m * sizeof(int *));
    for (i = 0; i < *m; i++) {
        (*c)[i] = (int *) malloc(*n * sizeof(int));
    }

    loadMatrix(a,*m,*k);
    loadMatrix(b,*k,*n);
}

void loadMatrix(int ***matrix, int m, int n)
{
    // TODO: implement
    int i;
    int j;
    for(i = 0; i < m; i++) {
        for(j = 0; j < n; j++) {
            if(scanf("%d", &(*matrix)[i][j]) == 0) {
                oops("Cannot read matrix input data.\n", -2);
            }
        }
    }
}

void displayMatrix(int **matrix, int m, int n)
{
    // TODO: implement
    int i;
    int j;
    for (i = 0; i <  m; i++) {
        for (j = 0; j < n; j++) {
            printf("%d ", matrix[i][j]);
        }
        printf("\n");
    }
}


pthread_t **multiply(int **a, int **b, int **c, int m, int k, int n)
{
    pthread_t **tids = alloc_tids(m, n);

    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
        {
            //create a cell for each thread so that they can have their own independent data
            MATRIX_CELL *cell = (MATRIX_CELL *)malloc(sizeof(MATRIX_CELL));
            cell->i = i;
            cell->j = j;
            cell->k = k;
            cell->a = a;
            cell->b = b;
            cell->c = c;

            pthread_create(&((tids)[i][j]), NULL, matrixThread, (void *) cell);
        }

    join(tids, m, n);
    return tids;
}

pthread_t **alloc_tids(int m, int n)
{
    //alloc overall space
    pthread_t **tids = (pthread_t **)malloc(sizeof(pthread_t *) * m + sizeof(pthread_t) * n * m); //tids is an m by n matrix

    //setup row pointers
    pthread_t *rowPoint = (pthread_t *) (tids + m); //distance to a single row
    for (int i = 0; i < m; i++)
        (tids[i]) = (rowPoint + n * i);

    return tids;
}

void join(pthread_t **tids, int m, int n)
{
    for (int i = 0; i < m; i++)
        for (int j = 0; j < n; j++)
            if (pthread_join(tids[i][j], NULL) != 0)
            oops("Cannot join thread\n", -2);
}

void freeMatrix(int **matrix, int m)
{
    free(matrix); //free overall space
}

void free_tids(pthread_t **tids, int m)
{
    free(tids);
}





/**
* Name: Jamie Hernandez
* Lab/task: Lab 8
* Date: 10/18/2021
**/
#include "bank.h"

int numOfCustomers;        // the number of customers of the bank //threads n
int numOfAccounts;         // the number of accounts offered by the bank //resources m

double *available;    // the amount available of each customer
double **maximum;     // the maximum demand of each customer
double **allocation;  // the amount currently allocated to each customer
double **need;     // the remaining needs of each customer

/***
 * Read the state of the bank from a file.
 *
 * The file format is as follows:
 *
 * <num of customers> <num of accounts>
 * <initial state of accounts>
 * <max needs for each customer>
 *
 * e.g.,
 *
 * 5 3
 * 15 10 5
 * 7,5,3
 * 3,2,2
 * 9,0,2
 * 2,2,2
 * 4,3,3
 *
 * Any of  ", \n\t" is a valid separator
 *
 */
void loadBankState() {
    // create the bank
    initBank();

    // read initial values for maximum array
    for (int i = 0; i < numOfCustomers; i++)
        addBankCustomer(i);
}

/***
 * Initialize the bank for a number of accounts and customers.
 */
void initBank() {
    double in[2];
    readLine(in);
    numOfCustomers = (int) in[0];
    numOfAccounts = (int) in[1];

    available = calloc(numOfAccounts, sizeof(double));
    readLine(available);

    // initialize the accounts
    maximum = calloc(numOfCustomers, sizeof(double *));
    allocation = calloc(numOfCustomers, sizeof(double *));
    need = calloc(numOfCustomers, sizeof(double *));
}

/***
 * This function adds a customer to the bank system.
 * It records its maximum fund demand with the bank.
 */
void addBankCustomer(int customerNum) {
    maximum[customerNum] = calloc(numOfAccounts, sizeof(double));
    readLine(maximum[customerNum]);

    allocation[customerNum] = calloc(numOfAccounts, sizeof(double));
    // we start with zero allocated

    need[customerNum] = calloc(numOfAccounts, sizeof(double));
    arraycpy(need[customerNum], maximum[customerNum], numOfAccounts);
}

/***
 * Outputs the state of the bank; i.e., funds in each account of each customer
 */
void displayBankState() {
    printf("%13s", "Available =");
    display(available, numOfAccounts);

    printf("\n%13s", "Allocation =");
    for (int i = 0; i < numOfCustomers; i++)
        display(allocation[i], numOfAccounts);

    printf("\n%13s", "Max =");
    for (int i = 0; i < numOfCustomers; i++)
        display(maximum[i], numOfAccounts);

    printf("\n%13s", "Need =");
    for (int i = 0; i < numOfCustomers; i++)
        display(need[i], numOfAccounts);

    printf("\n");
}

//step 2 from safety slide
int findI(double *work, bool *finish) {
    for (int i = 0; i < numOfCustomers; i++) {
        bool *ptr = finish + i;
        if (*(ptr) == false && need[i] <= work) {
            return i;
        }
    }
    return -1;
}

/***
 * Determines whether the currect system state is safe.
 */
bool isSafeState(int customerNum, double *request) {
    // TODO implement
    bool t = false;

    //safety step 1
    double *work = calloc(numOfAccounts, sizeof(double));
    bool *finish = calloc(numOfCustomers, sizeof(double));
    double *tempAvail = calloc(numOfAccounts, sizeof(double));
    double *tempAllo = calloc(numOfAccounts, sizeof(double));
    double *tempNeed = calloc(numOfAccounts, sizeof(double));
    int *safe = calloc(numOfAccounts, sizeof(int));

    //save available, allocation[i] and need[i]
    ////test
    arraycpy(work, available, numOfAccounts);
    arraycpy(tempAllo, *allocation, numOfAccounts);
    arraycpy(tempAvail, available, numOfAccounts);
    arraycpy(tempNeed, *need, numOfAccounts);
    ////test
    set(false, finish, numOfCustomers);

    for (int j = 0; j < numOfAccounts; j++) {
        //pretend to allocate
        sub(available, &request[j], numOfAccounts);
        add(allocation[j], &request[j], numOfAccounts);
        sub(*need, request, numOfAccounts);

        ////testing stuff
        printf("\nrequested: %f\n", request[j]);
        printf("allocation: %f\n", *allocation[j]);
        printf("available: %f\n", available[j]);
        printf("need: %f\n", *need[j]);
        printf("\ngoing to run safety check now...\n");
        ////test end

        //safety step 2
//        xx:
        int i = findI(work, finish);

        if (i == -1) {
            //safety step 4
            if (allTrue(finish, numOfCustomers)) {
                t = true;
                return t;
            }
        } else {
            //safety step 3
            add(work, allocation[j], numOfAccounts); //changed from custNo
            finish[j] = true;//changed from custNo
            //goto safety step 2
            i = findI(work, finish);
            safe[j] = i;
//            goto xx;
        }
    }
    printf("Order: ");
    for (int i = 0; i < numOfAccounts; i++) {
        printf("%d ", safe[i]);
    }
    printf("\n");
    return t;
}

/***
 * Determines whether a request for loan can be granted.
 */
bool solvencyTest(int customerNum, double *request) {
    // TODO implement
    bool t;
    //step 1
    if (request[customerNum] > *need[customerNum]) {
        printf("\nrequest: %f > need: %f\n", request[customerNum], *need[customerNum]);
        return false;
    }
    //step 2
    if (request[customerNum] > available[customerNum]) {
        printf("Insufficient funds...\n");
        printf("gotta wait...\n");
    }
    //step 3
    t = isSafeState(customerNum, request);
    return t; // TODO: modify as appropriate
}

/***
 * Make a request for a loan.
 */

bool borrow(int customerNum, double funds[]) {
    //step 1 - running checks
    bool ret = solvencyTest(customerNum, funds);

    // TODO: complete
    //step 2 - if passes(true) -> grant and process loan

    //return if loan was granted

    return (ret ? printf("loan granted\n") : printf("loan denied\n"));
}

/***
 * Repay a loan.
 */
bool repay(int customerNum, double funds[]) {
    bool ret = lessOrSame(funds, allocation[customerNum], numOfAccounts);

    // TODO: complete

    return ret;
}


/***
 * In a loop, take input from the standard console until 'Q' or 'q' is entered.
 *
 * Each input should have the following syntax:
 *
 * {Q[UIT] | ST[ATUS] | [BO[RROW] | RE[PAY]] <customer number>  {<account funds> ...}}
 *
 * e.g. (for 3 accounts and with at least 2 customers):
 *
 * ST
 * BO 1 3 2 1
 * RE 1 1 0 1
 * QUIT
 *
 */
void bankTeller() {
    // now loop reading requests to withdraw or deposit funds
    double request[numOfAccounts];
    char *inp = malloc(MAX_LINE_SIZE * sizeof(char));
    while (true) {
        printf("Teller: How may I help you? > ");

        if (fgets(inp, MAX_LINE_SIZE, stdin) == 0)
            break;

        if (strlen(inp) == 0)
            goto exception;

        // need a copy, since "line" will be used for tokenizing, so
        // it will change until it gets NULL; we would nnot be able to re-use it
        char *line = strdup(inp);

        // get transaction type - borrow (BO) or repay (RE) or ST (status)
        line[strlen(line) - 1] = '\0';

        if (line == NULL)
            goto exception;

        char *trans = strsep(&line, " ,\t");
        strToUpper(&trans); // so the case does not matter

        printf("REQUEST: %s\n", trans);
        if (trans[0] == 'Q') {
            displayBankState();
            exit(1);
        } else if (strncmp(trans, "STATUS", 2) == 0)
            displayBankState();
        else {
            if ((strncmp(trans, "BORROW", 2) != 0) && (strncmp(trans, "REPAY", 2) != 0))
                goto exception;

            // get the customer number making the tranaction
            if (line == NULL)
                goto exception;

            int custNum = strtod(strsep(&line, " "), NULL);
            printf("CLIENT %d: ", custNum);

            if (custNum >= numOfCustomers)
                goto exception;

            // get the resources involved in the transaction
            for (int i = 0; i < numOfAccounts; i++) {
                if (line == NULL)
                    goto exception;

                request[i] = strtod(strsep(&line, " ,\t"), NULL);
            }

            display(request, numOfAccounts);

            // now check the transaction type
            if (strncmp(trans, "BORROW", 2) == 0) {
                if (borrow(custNum, request))
                    printf("\n*APPROVED*\n");
                else
                    printf("\n*DENIED*\n");
            } else if (strncmp(trans, "REPAY", 2) == 0) {
                if (repay(custNum, request))
                    printf("\n*APPROVED*\n");
                else
                    printf("\n*DENIED*\n");
            }
            continue;
            exception:
            printf("\nExpected input: Q[UIT] | ST[ATUS] | [BO[RROW] | RE[PAY]] <customer number> <resource #1> <#2> <#3> ...\n");
            continue;
        }
    }
}

/***
 * Read a line of values separated by a set of delimeters from a file into an array
 */
void readLine(double array[]) {
    char *line = malloc(MAX_LINE_SIZE * sizeof(char));
    fgets(line, MAX_LINE_SIZE, stdin);
    char *tok;
    int i = 0;
    while ((tok = strsep(&line, ", \n\t")) != NULL)
        if (strlen(tok) > 0)
            array[i++] = strtod(tok, NULL);

    free(line);
}

/***
 * Convert the string to upper case
 */
void strToUpper(char **s) {
    for (char *c = *s; *c; c++)
        if (isascii(*c) && islower(*c))
            *c = toupper(*c);
}

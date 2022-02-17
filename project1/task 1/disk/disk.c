/**
* Name: Jamie Hernandez
* Lab/task: Project 1
* Date: 11/15/2021
**/

//////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////

#include "disk.h"

disk_t disk;

/***
 *
 * Verifies correctness of the logical block address, and then translates it to a cylinder-head-sector equivalent.
 *
 */
CIDEV_RET_CODE lba2chs(lba_t lba, chs_t *chs) {
    if (lba >= MAX_LOGICAL_BLOCK) {
        return CIDEV_ADDRESS_ERROR;
    }

    // todo: implement
    /*CYL = LBA ÷ (HPC × SPT)
     *HEAD = (LBA ÷ SPT) mod HPC
     *SECT = (LBA mod SPT)
     */
    chs->cyl = lba / (NUM_OF_HEADS * NUM_OF_SECTS);
    chs->head = (lba / NUM_OF_SECTS) % NUM_OF_HEADS;
    chs->sect = (lba % NUM_OF_SECTS);

    return CIDEV_SUCCESS;
}

/***
 *
 * Verifies correctness of the cylinder-head-sector address, and then translates it to a logical block address.
 *
 */
CIDEV_RET_CODE chs2lba(chs_t *chs, lba_t *lba) {
    // todo: implement
    if ((chs->cyl * NUM_OF_HEADS * NUM_OF_SECTS) + (chs->sect) >= MAX_LOGICAL_BLOCK) {
        return CIDEV_ADDRESS_ERROR;
    }
    //CHS-to-LBA
    //LBA = (CYL x HPC x SPT) + (HEAD x SPT) + (SECT - 1)
    *lba = (chs->cyl * NUM_OF_HEADS * NUM_OF_SECTS) + chs->head * NUM_OF_SECTS + (chs->sect);

    return CIDEV_SUCCESS;
}

/***
 *
 * Verifies the parameters, then allocates space for the requested data (using
 * the caller-provided pointer buffer), copies the data from the disk to the buffer,
 * and appends '\0' to terminate the string.
 *
 */
CIDEV_RET_CODE readDisk(lba_t lba, unsigned int size, char **buffer) {
    // todo: verify parameters
    //check if request is greater than available space
    if (size >= MAX_LOGICAL_BLOCK * SECT_SIZE) {
        return CIDEV_SPACE_ERROR;
    }

    CIDEV_RET_CODE errCode = CIDEV_SUCCESS;
    unsigned int sectors = size / SECT_SIZE;

    //adjust number of sects
    if (size % SECT_SIZE != 0) {
        sectors += 1;
    }

    chs_t chs;

    *buffer = (char *) calloc(sizeof(char *), size + 1);
    // todo: implement

    //loop through sectors
    for (int i = 0; i < sectors; i++) {
        //convert logical to physical
        errCode = lba2chs((lba + i) % MAX_LOGICAL_BLOCK, &chs);
        if (errCode != CIDEV_SUCCESS) {
            return errCode;
        }

        //add string to buffer
        if ((size / SECT_SIZE) == 0) {
            strncat(*buffer, disk[chs.cyl][chs.head][chs.sect], size);
        } else {
            strncat(*buffer, disk[chs.cyl][chs.head][chs.sect], SECT_SIZE);
        }
        size -= SECT_SIZE;
    }
    //add string terminator to end
    strcat(*buffer, "\0");

    return errCode;
}

/***
 *
 * An auxiliary function to fill a single disk block with '.'
 *
*/
CIDEV_RET_CODE clearBlock(lba_t lba) {
    char *writeBuffer;
#ifdef __DEBUG_DISK
    writeBuffer = malloc(SECT_SIZE);
#else
    writeBuffer = kmalloc(SECT_SIZE, GFP_USER);
#endif
    if (writeBuffer == NULL)
        return CIDEV_SPACE_ERROR;

    for (int i = 0; i < SECT_SIZE; i++)
        writeBuffer[i] = '.';

    CIDEV_RET_CODE errCode = writeDisk(lba, writeBuffer);

#ifdef __DEBUG_DISK
    free(writeBuffer);
#else
    kfree(writeBuffer);
#endif

    return errCode;
}

/***
 *
 * Validates the parameters, and then writes the caller-provided data to the disk starting at the block pointed
 * to by the logical block address.
 *
 */
CIDEV_RET_CODE writeDisk(lba_t lba, char *buffer) {
    // todo: verify the parameters
    CIDEV_RET_CODE errCode = CIDEV_SUCCESS;

    size_t size = strlen(buffer);
    unsigned int sectors = size / SECT_SIZE;

    if (sectors >= MAX_LOGICAL_BLOCK) {
        return CIDEV_SPACE_ERROR;
    }

    if (strlen(buffer) % SECT_SIZE != 0) {
        sectors += 1;
    }

    chs_t chs;

    // todo: implement

    //loop through sectors
    for (int i = 0; i < sectors; i++) {
        //convert logical to physical
        errCode = lba2chs((lba + i) % MAX_LOGICAL_BLOCK, &chs);
        if (errCode != CIDEV_SUCCESS) {
            return errCode;
        }

        //copy to disk
        if ((size / SECT_SIZE) == 0) {
            strncpy(disk[chs.cyl][chs.head][chs.sect], buffer + i * SECT_SIZE, size);
        } else {
            strncpy(disk[chs.cyl][chs.head][chs.sect], buffer + i * SECT_SIZE, SECT_SIZE);
        }
        size -= SECT_SIZE;
    }

    return errCode;
}

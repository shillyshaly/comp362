//////////////////////////////////////////////////////////////////////////
///
/// Copyright (c) 2020 Prof. AJ Bieszczad. All rights reserved.
///
//////////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <stdio.h>

#include "../inc/ci_dev.h"

#define oops(msg, errnum) { perror(msg); exit(errnum); }

#define BUFSIZE 4096

int main(void)
{
	int fd, len;

	// test message to send to the device
	char msg[] = "Skeleton Kernel Module Test";
	char receive_buffer[BUFSIZE];

	// variables for holding device control data
	// int ioctl_control_data, ioctl_status_data;

	// open the I/O channel to the device
	fd = open("/dev/cidev", O_RDWR | O_SYNC);
	if ( fd == -1)
		oops("Unable to open device...", 1);

	// test device write function
	len = write(fd, msg, strlen(msg) + 1);
	if ( len == -1 )
		oops("Unable to write to the simuated device.\n", 2);

	printf("'%s' written to /dev/cidev\n", msg);

	// test device read function
	len = read(fd, receive_buffer, BUFSIZE);
	if ( len == -1 )
		oops("Unable to read from the simulated device.\n", 3);

	printf("'%s' read from /dev/cidev\n", receive_buffer);

	// test ioctl with the device

	// this is a sample content/command to send to the device
	// ioctl_control_data = 0xABCDEFEF;
	DISK_REGISTER cidev_register = {0, 0, 0, 0, 0, 0, 0, 0, 0};
	cidev_register.track = 100;
	cidev_register.sector = 20;
	cidev_register.command = 0xA; // arbitrary example
	ioctl(fd, IOCTL_CIDEV_WRITE, &cidev_register);

	// receive status from the device
	// ioctl(fd, IOCTL_CIDEV_WRITE, &ioctl_control_data);
	ioctl(fd, IOCTL_CIDEV_READ, &cidev_register);
	printf("DEVICE RESPONSE: '%s'\n",
	       !cidev_register.error_occured ? "OK" : "ERROR");

	close(fd);
}


/*
 * test.c
 *
 *  Created on: Oct 10, 2014
 *      Author: ross
 */

#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#define BUFFER_LEN	100

int main(int argc, char **argv)
{
	assert(argc > 1);
	char buf[BUFFER_LEN];
	char i = 0;

	memset(buf, 0, BUFFER_LEN);
	printf("input: %s\n", argv[1]);

	int fp = open("/dev/myCdev", 0, O_RDWR);
	write(fp, argv[1], strlen(argv[1]));
	while (read(fp, &buf[i++], 1)) ;
	printf("Reversed by the driver: %s", buf);

	return 0;
}

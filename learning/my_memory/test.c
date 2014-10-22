/*
 * test.c
 *
 *  Created on: Oct 23, 2014
 *      Author: ross
 */

#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>

#define BUFFER_LEN	1

int main(int argc, char *argv[])
{
	assert(argc > 1);
	char buf[BUFFER_LEN];
	char i = 0;

	memset(buf, 0, BUFFER_LEN);
	printf("input: %s\n", argv[1]);

	int fp = open("/dev/my_mem", O_RDWR);
	if (fp < 0)
	{
		printf("Error opening /dev/my_mem\n");
		return -1;
	}

	printf("Input string arg[1]: %s\n", argv[1]);
	write(fp, argv[1], strlen(argv[1]));
	read(fp, &buf, strlen(argv[1]));
	//while (read(fp, &buf[i++], 1)) ;
	printf("buffer: %s\n", buf);
	printf("Read by the driver: %s\n", buf);
	close (fp);

	return 0;
}

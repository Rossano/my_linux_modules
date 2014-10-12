#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char **argv)
{
  int fd;
  int rc = 0;
  char rd_buf[80];
  
  
  printf("%s Entered\n", argv[0]);
  
  fd = open("/dev/scull", O_RDWR);
  
  if(fd == -1)
  {
    printf("%s: open failed, error code is %d\n", argv[0], fd);
    exit(-1);
  }
  int wr;
  printf("%s: open scull successfull\n");
  rc = write(fd, "mo' ce provamo", wr);
  if (rc == -1)
  {
    rc = read(fd, rd_buf, 0);
    if (rc > 0)
    {
      printf("%s: read %s from scull\n", argv[0], rd_buf);
    }
  }
  close(fd);
  
  return 0;
}

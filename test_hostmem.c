


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main()
{
	int fd;
	char buffer[40];
	char response[40];
	unsigned long addr;
	strcpy(buffer,"123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	int len;
	fd = open("/dev/hostmem",O_RDWR);
	if (fd) {
		len = write(fd,buffer,strlen(buffer));
		printf("write len = %d\n",len);
		len = read(fd,response,40);
		printf("read len = %d\n",len);
		printf("read value = %s\n",response);
		addr = ioctl(fd,1);
		printf("addr phys is 0x%016lx\n",addr);
	}
	close(fd);
	return 0;
}

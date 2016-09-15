


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main()
{
	int fd_rx;
	int fd_tx;
	char buffer_rx[40];
	char buffer_tx[40];
	char response[40];
	unsigned long addr;
	strcpy(buffer_rx,"123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ");
	strcpy(buffer_tx,"abcdefghijklmnopqrstuvwxyz123456789");
	int len_rx;
	int len_tx;
	fd_rx = open("/dev/hostmem",O_RDWR);
	if(!fd_rx){
		printf("fd_rx open error\n");
		exit(0);
	}
	fd_tx = open("/dev/hostmem",O_RDWR);
	if(!fd_tx){
		printf("fd_tx open error\n");
		exit(0);
	}

	len_rx = write(fd_rx,buffer_rx,strlen(buffer_rx));
	printf("write len_rx = %d\n",len_rx);
	len_tx = write(fd_tx,buffer_tx,strlen(buffer_tx));
	printf("write len_tx = %d\n",len_tx);
	len_rx = read(fd_rx,response,40);
	printf("read len_rx = %d\n",len_rx);
	printf("read value rx = %s\n",response);
	len_tx = read(fd_tx,response,40);
	printf("read len_tx = %d\n",len_tx);
	printf("read value tx = %s\n",response);
	
	
	ioctl(fd_rx,1,&addr);
	printf("addr phys rx is 0x%016lx\n",addr);
	ioctl(fd_tx,1,&addr);
	printf("addr phys tx is 0x%016lx\n",addr);

	close(fd_rx);
	close(fd_tx);
	return 0;
}

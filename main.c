#include<stdio.h>
#include<string.h>
#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
//#include "mmu.h"
#include "proc.h"
#include "spinlock.h"
#include "fs.h"
#include "buf.h"
#include "file.h"
#include "fcntl.h"
int main() 
{

	int fd,fd1,fd2;
	char buf[512];

    // init the file system
	binit();
	initlog(1);
	iinit(1);
    fileinit();
	initproc();
	memset(buf,0,512);
	//Read a file
	fd = sys_open("/test_file2",0);
	sys_read(fd,buf,512);
	printf("The data is %s\n",buf);
	sys_close(fd);

	//First write,then read
	fd = sys_open("/LYR",O_CREATE);
	fd = sys_open("/LYR",O_WRONLY);
	memset(buf,'\0',512);
	memset(buf,'x',10);
	memset(buf+10,'y',10);
	sys_write(fd,buf,512);
	sys_close(fd);
	
	fd1 = sys_open("/LYR",O_RDONLY);
	memset(buf,0,512);
	sys_read(fd1,buf,10);
	sys_read(fd1,buf,10);
	printf("The data is %s\n",buf);

	fd2 = sys_open("/LYR",O_RDONLY);
	memset(buf,0,512);
	sys_read(fd2,buf,10);
	printf("The data is %s\n",buf);
	sys_close(fd1);
	sys_close(fd2);

/*
	sys_link("/LYR","/XQ");
	sys_unlink("/LYR");
	fd = sys_open("/XQ",O_RDONLY);
	memset(buf,0,512);
	sys_read(fd,buf,512);
	printf("The new write is %s\n",buf);
	//sys_unlink("/LYR");
	sys_close(fd);
*/
/*	fd = sys_open("/test_file1",O_RDONLY);
	memset(buf,0,512);
	sys_read(fd,buf,512);
	printf("The read is %s\n",buf);
	sys_close(fd);
*//*
	sys_mkdir("/home");
	sys_mkdir("/home/zbh");
	fd = sys_open("/home/zbh/tinyfs",O_CREATE);
	sys_close(fd);

	fd = sys_open("/home/zbh/tinyfs",O_WRONLY);
	memset(buf,0,512);
	memset(buf,'T',512);
	sys_write(fd,buf,512);
	sys_close(fd);

	fd = sys_open("/home/zbh/tinyfs",O_RDONLY);
        memset(buf,0,512);
        sys_read(fd,buf,512);
        printf("The read is %s\n",buf);
        sys_close(fd);
*/
}


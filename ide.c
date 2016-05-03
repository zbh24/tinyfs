#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include "types.h"
#include "buf.h"

static char *diskpath0 = "./DISK0";
static char *diskpath1 = "./DISK1";

static void 
idewrite(struct buf *b) 
{
	int fd;
	if(b->dev == 0) { 
		if((fd = open(diskpath0,O_WRONLY)) == -1) {
			printf("Open disk0 is filed.\n");
			return ;
		}
	} else if(b->dev == 1) {
		if((fd = open(diskpath1,O_WRONLY)) == -1) {
			printf("Open disk1 is filed.\n");
			return;
		}
	}
	if(lseek(fd,(b->blockno)*512,SEEK_SET) == -1) {
		printf("When write the disk,lseek is wrong.\n");
	}
	if(write(fd,b->data,512) == -1) {
		printf("Write the disk is wrong.\n");
		return;
	}
	if(close(fd) == -1) {
		printf("Close the disk is wrong.\n");
	}
}

static void
ideread(struct buf *b)
{
	int fd;
	if(b->dev == 0) { 
		if((fd = open(diskpath0,O_RDONLY)) == -1) {
			printf("Open disk0 is filed.\n");
			return ;
		}
	} else if(b->dev == 1) {
		if((fd = open(diskpath1,O_RDONLY)) == -1) {
			printf("Open disk1 is filed.\n");
			return;
		}
	}
	if(lseek(fd,(b->blockno)*512,SEEK_SET) == -1) {
		printf("When write the disk,lseek is wrong.\n");
	}
	if(read(fd,b->data,512) == -1) {
		printf("Read the disk is wrong.\n");
		return;
	}
	if(close(fd) == -1) {
		printf("Close the disk is wrong.\n");
	}
	return; 
}

void
idedump(int dev,int blockno)
{
	int fd;
	char buf0[512];
	if(dev == 0) { 
		fd = open(diskpath0,O_RDONLY);
	} else {
		fd = open(diskpath1,O_RDONLY);
	}
	lseek(fd,blockno*512,SEEK_SET);
	read(fd,buf0,512);
	printf("The [%d,%d] data is:\n%s\n",dev,blockno,buf0);
	close(fd);
	return; 
}

static void 
idestart(struct buf *b)
{
	if(b->flags & B_DIRTY) {
		idewrite(b);
	} else {
		ideread(b);
	}
			
}

void
iderw(struct buf *b) 
{
	idestart(b);
	b->flags |= B_VALID;
}


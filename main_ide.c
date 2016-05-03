#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "ide.h"
#include "buf.h"

int main() 
{
	char tmp[512] = "abcd";
	struct buf *b;
	b = (struct buf *)malloc(sizeof(struct buf));
	memmove(b->data,tmp,512);

	b->dev = 1;
	b->blockno = 1;
	b->flags = 0;
	iderw(b);
	printf("The is %s\n",b->data);
	return 0;

	//Test the write [0,0] abcd
	b->dev = 0;
	b->blockno = 0;
	b->flags = B_DIRTY;
	iderw(b);

	//Test the write [0,1] abcd
	b->flags = B_DIRTY;
	b->blockno = 1;
	iderw(b);
	idedump(0,0);
	idedump(0,1);
	memset(b->data,'z',512);
	printf("The buf b is %s:\n",b->data);
	//这种情况下，lseek会失败的。
	//b->blockno = 2;
	iderw(b);
	printf("The [0,1] is %s:\n",b->data);

	b->flags = B_DIRTY;
	memset(b->data,'a',512);
	b->blockno = 0;
	iderw(b);
	idedump(0,0);
	idedump(0,1);

	printf("-------------------------\n");
	idedump(0,3);
	b->blockno = 3;
	iderw(b);
	printf("The buf b is %s:\n",b->data);
	// lseek if beyond the len of file,lseek is success,but will read the 0 data.

}

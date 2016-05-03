#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "types.h"
#include "defs.h"
#include "param.h"
#include "spinlock.h"
#include "fs.h"
#include "buf.h"

int main()
{	
	struct buf *b;
	b = malloc(sizeof(struct buf));
	b->dev = 0;
	b->blockno = 1;
	memset(b->data,'b',512);
	binit();
	
	
	b = bread(0,1);
	brelse(b); 	
	printf("The buf b is %s\n",b->data);

	memset(b->data,'e',512);
	bwrite(b);
	brelse(b);	
	printf("The buf b is %s\n",b->data);

}



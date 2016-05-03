#ifndef IDE_H
#define IDE_H

#include "buf.h"


extern  void idedump(int dev,int blockno);

extern void iderw(struct buf *b);




#endif

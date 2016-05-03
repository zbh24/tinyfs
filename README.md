# tinyfs

This a mirco but complete and have POSIX interface like ex3 file system.It is come from the famous mit OS XV6.In the 2015,I wanted to formal vertify the file system,so I abstract the it from the XV6.This is the process of this project born.

###structure
It has six spects,includes ide.c bio.c,log.c inode.c file.c syscall.c.And it matched the six layers from the low to high.And I use the text file to simulate the DISK.So it can run like a truely file ststem run in the disk.The  layers function like follows:

- ide.c:it can operate the disk directly,it is in the lowest int the file system
- bio.c:it is the buffer cache,read or write data in the buffer
- log.c:it can recover from the damage,so it can record the operate.
- inode.c:wc know every file has a inode,inlcudes file and directory.
- file.c:it describe the propetry of the file,include of size,owner and so on.
- syscall.c:This is the POSIX interface,include sys_read,sys_write,sys_close and so on.

###Usage
The whole structures like above.If you want to use it,first you should create the disk,and then generate the new file system.And then you can use the file system interface to read or write file.
I wrtie some command in the makefile.And this is the step.

- make makefs:it create the two virtual disk,DIKS0 and DISK1.And it also create some file and write to DISK for later test.
- mkae syscall_test:This is the example which I write for test.You also can run "make ide_test" or "make bio_test".And you can also write you own test cases.
- ./a.out:You can obeserve the output.

###Others
1.The deafult the DISK is DISK1,but it can modify.The params in the params.h(ROOTDEV).
2.I modified from the mit xv6 os,so this code which you can study,and this is also the reason why I put the code in th github,but copytights is belong to mit.
3.If you have any questions,welcome to email to me(zbh24@126.com).





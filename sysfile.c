//
// File-system system calls.
// Mostly argument checking, since we don't trust
// user code, and calls into file.c and fs.c.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "defs.h"
#include "param.h"
#include "stat.h"
//#include "mmu.h"
#include "proc.h"
#include "fs.h"
#include "file.h"
#include "fcntl.h"

// Fetch the nth word-sized system call argument as a file descriptor
// and return both the descriptor and the corresponding struct file.
/*
static int
argfd(int n, int *pfd, struct file **pf)
{
  int fd;
  struct file *f;

  if(argint(n, &fd) < 0)
    return -1;
  if(fd < 0 || fd >= NOFILE || (f=proc->ofile[fd]) == 0)
    return -1;
  if(pfd)
    *pfd = fd;
  if(pf)
    *pf = f;
  return 0;
}
*/

void 
initproc() {
	int fd;
	proc = malloc(sizeof(struct proc));
	for(fd = 0; fd < NOFILE; fd++)
		proc->ofile[fd] = 0;
}

struct file *
fdlookup(int fd)
{	
	if(fd < 0 || fd >= NOFILE)
		printf("The fd is worng\n");
	return proc->ofile[fd];
}

// Allocate a file descriptor for the given file.
// Takes over file reference from caller on success.
static int
fdalloc(struct file *f)
{
  int fd;

  for(fd = 0; fd < NOFILE; fd++){
    if(proc->ofile[fd] == 0){
      proc->ofile[fd] = f;
      return fd;
    }
  }
  return -1;
}

/*
int
sys_dup(void)
{
  struct file *f;
  int fd;
  
  if(argfd(0, 0, &f) < 0)
    return -1;
  if((fd=fdalloc(f)) < 0)
    return -1;
  filedup(f);
  return fd;
}
*/

//Done
int
sys_read(int fd ,char *p,int n)
{
  struct file *f;;

  f = fdlookup(fd);
  return fileread(f, p, n);
}

//Done
int
sys_write(int fd,char *p,int n)
{
  struct file *f;

  f = fdlookup(fd);
  return filewrite(f, p, n);
}

//Done
int
sys_close(int fd)
{
  struct file *f;
 
  f = fdlookup(fd); 
  proc->ofile[fd] = 0;
  fileclose(f);
  return 0;
}

//Done
int
sys_fstat(struct file *f,struct stat *st)
{
  return filestat(f, st);
}

// Create the path new as a link to the same inode as old.
//Done
int
sys_link(char *old,char *new)
{
  char name[DIRSIZ];
  struct inode *dp, *ip;

  if((ip = namei(old)) == 0){
    return -1;
  }

  begin_trans();

  ilock(ip);
  if(ip->type == T_DIR){
    iunlockput(ip);
    commit_trans();
    return -1;
  }

  ip->nlink++;
  iupdate(ip);
  iunlock(ip);

  if((dp = nameiparent(new, name)) == 0)
    goto bad;
  ilock(dp);
  if(dp->dev != ip->dev || dirlink(dp, name, ip->inum) < 0){
    iunlockput(dp);
    goto bad;
  }
  iunlockput(dp);
  iput(ip);

  commit_trans();

  return 0;

bad:
  ilock(ip);
  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);
  commit_trans();
  return -1;
}

// Is the directory dp empty except for "." and ".." ?
//Done!
static int
isdirempty(struct inode *dp)
{
  int off;
  struct dirent de;

  for(off=2*sizeof(de); off<dp->size; off+=sizeof(de)){
    if(readi(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
      printf("isdirempty: readi");
    if(de.inum != 0)
      return 0;
  }
  return 1;
}

//PAGEBREAK!
//Done
int
sys_unlink(char *path)
{
  struct inode *ip, *dp;
  struct dirent de;
  char name[DIRSIZ];
  uint off;

  if((dp = nameiparent(path, name)) == 0){
    return -1;
  }

  begin_trans();
  ilock(dp);

  // Cannot unlink "." or "..".
  if(namecmp(name, ".") == 0 || namecmp(name, "..") == 0)
    goto bad;

  if((ip = dirlookup(dp, name, &off)) == 0)
    goto bad;
  ilock(ip);

  if(ip->nlink < 1)
    printf("unlink: nlink < 1");
  if(ip->type == T_DIR && !isdirempty(ip)){
    iunlockput(ip);
    goto bad;
  }

  memset(&de, 0, sizeof(de));
  if(writei(dp, (char*)&de, off, sizeof(de)) != sizeof(de))
    printf("unlink: writei");
  if(ip->type == T_DIR){
    dp->nlink--;
    iupdate(dp);
  }
  iunlockput(dp);

  ip->nlink--;
  iupdate(ip);
  iunlockput(ip);

  commit_trans();

  return 0;

bad:
  iunlockput(dp);
  commit_trans();
  return -1;
}

//创建一个文件，如hello，这样创建/home/zbh/hello
//Done!
static struct inode*
create(char *path, short type, short major, short minor)
{
  uint off;
  struct inode *ip, *dp;
  char name[DIRSIZ];

  //dp是父节点的。比如path = /home,则dp = 1
  if((dp = nameiparent(path, name)) == 0)
    return 0;
  ilock(dp);
  //意思是说文件就可以冲掉，但是目录就不会被冲掉
  if((ip = dirlookup(dp, name, &off)) != 0){
    iunlockput(dp);
    ilock(ip);
    if(type == T_FILE && ip->type == T_FILE)
      return ip;
    iunlockput(ip);
    return 0;
  }

  if((ip = ialloc(dp->dev, type)) == 0)
    printf("create: ialloc");

  ilock(ip);
  ip->major = major;
  ip->minor = minor;
  ip->nlink = 1;
  iupdate(ip);

  if(type == T_DIR){  // Create . and .. entries.
    dp->nlink++;  // for ".."
    iupdate(dp);
    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(ip, ".", ip->inum) < 0 || dirlink(ip, "..", dp->inum) < 0)
      printf("create dots");
  }

  if(dirlink(dp, name, ip->inum) < 0)
    printf("create: dirlink");

  iunlockput(dp);

  return ip;
}

//如果一个文件的模式为creat，那么我们就创建这样一个文件。
//设置相应的权限和标志位
//Done!
int
sys_open(char *path,int omode)
{
  
  int fd;
  struct file *f;
  struct inode *ip;

  /*
  if(argstr(0, &path) < 0 || argint(1, &omode) < 0)
    return -1;
  */
  
  if(omode & O_CREATE){
    begin_trans();
    ip = create(path, T_FILE, 0, 0);
    commit_trans();
    if(ip == 0){
      return -1;
    }
  } else {
    if((ip = namei(path)) == 0){
      return -1;
    }
    ilock(ip);
    if(ip->type == T_DIR && omode != O_RDONLY){
      iunlockput(ip);
      return -1;
    }
  }

  if((f = filealloc()) == 0 || (fd = fdalloc(f)) < 0){
    if(f)
      fileclose(f);
    iunlockput(ip);
    return -1;
  }
  iunlock(ip);

  f->type = FD_INODE;
  f->ip = ip;
  f->off = 0;
  f->readable = !(omode & O_WRONLY);
  f->writable = (omode & O_WRONLY) || (omode & O_RDWR);
  //proc->ofile[fd] = f;
  return fd;
}

//Done
int
sys_mkdir(char *path)
{
  struct inode *ip;

  begin_trans;
  if((ip = create(path, T_DIR, 0, 0)) == 0){
    commit_trans();
    return -1;
  }
  iunlockput(ip);
  commit_trans();
  return 0;
}

//这个不用管，因为是创建dev设备
/*
int
sys_mknod(void)
{
  struct inode *ip;
  char *path;
  int len;
  int major, minor;
  
  begin_trans();
  if((len=argstr(0, &path)) < 0 ||
     argint(1, &major) < 0 ||
     argint(2, &minor) < 0 ||
     (ip = create(path, T_DEV, major, minor)) == 0){
    commit_trans();
    return -1;
  }
 iunlockput(ip);
  commit_trans();
  return 0;
}
*/

/*
int
sys_chdir(void)
{
  char *path;
  struct inode *ip;

  begin_trans();
  if(argstr(0, &path) < 0 || (ip = namei(path)) == 0){
    commit_trans();
    return -1;
  }
  ilock(ip);
  if(ip->type != T_DIR){
    iunlockput(ip);
    commit_trans();
    return -1;
  }
  iunlock(ip);
  iput(proc->cwd);
  commit_trans();
  proc->cwd = ip;
  return 0;
}
*/

/*
int
sys_exec(void)
{
  char *path, *argv[MAXARG];
  int i;
  uint uargv, uarg;

  if(argstr(0, &path) < 0 || argint(1, (int*)&uargv) < 0){
    return -1;
  }
  memset(argv, 0, sizeof(argv));
  for(i=0;; i++){
    if(i >= NELEM(argv))
      return -1;
    if(fetchint(uargv+4*i, (int*)&uarg) < 0)
      return -1;
    if(uarg == 0){
      argv[i] = 0;
      break;
    }
    if(fetchstr(uarg, &argv[i]) < 0)
      return -1;
  }
  return exec(path, argv);
}

int
sys_pipe(void)
{
  int *fd;
  struct file *rf, *wf;
  int fd0, fd1;

  if(argptr(0, (void*)&fd, 2*sizeof(fd[0])) < 0)
    return -1;
  if(pipealloc(&rf, &wf) < 0)
    return -1;
  fd0 = -1;
  if((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0){
    if(fd0 >= 0)
      proc->ofile[fd0] = 0;
    fileclose(rf);
    fileclose(wf);
    return -1;
  }
  fd[0] = fd0;
  fd[1] = fd1;
  return 0;
}
*/

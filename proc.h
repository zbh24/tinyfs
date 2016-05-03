// Segments in proc->gdt.
#define NSEGS     7



struct proc {
  uint sz;                     // Size of process memory (bytes)
  struct file *ofile[NOFILE];  // Open files
  char name[16];               // Process name (debugging)
 struct inode* cwd;
};

struct proc *proc;
// Process memory is laid out contiguously, low addresses first:
//   text
//   original data and bss
//   fixed-size stack
//   expandable heap

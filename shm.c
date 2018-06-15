#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct {
  struct spinlock lock;
  struct shm_page {
    uint id;
    char *frame;
    int refcnt;
  } shm_pages[64];
} shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer) {

//you write this
  int i = 0;
 int eid = 0;
  char * freePage;
  for (i = 0; i < 64; i++)
  {
    if (id == shm_table.shm_pages[i].id)
    {
      mappages(myproc()->pgdir,(char *)PGROUNDUP(myproc()->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
      acquire(&(shm_table.lock));
      shm_table.shm_pages[i].refcnt++;
      release(&(shm_table.lock));
      *pointer=(char *)PGROUNDUP(myproc()->sz);
      myproc()->sz = PGROUNDUP(myproc()->sz);
      eid = 1;
    }
    else if (eid == 0)
    {
      acquire(&(shm_table.lock));
      shm_table.shm_pages[i].id = id;
      freePage = kalloc();
      memset(freePage, 0, PGSIZE); 
      shm_table.shm_pages[i].frame = freePage; 
      shm_table.shm_pages[i].refcnt = 1; 
      release(&(shm_table.lock));
      mappages(myproc()->pgdir, (char *)(myproc()->sz), PGSIZE, V2P(shm_table.shm_pages[i].frame), PTE_W|PTE_U);
      *pointer=(char *)(myproc()->sz);
      break;      
    }
  }     


return 0; //added to remove compiler warning -- you should decide what to return
}


int shm_close(int id) {
//you write this too!
  int i = 0;

for (i = 0; i < 64; ++i) {
   if(id == shm_table.shm_pages[i].id) {
     acquire(&(shm_table.lock));
     shm_table.shm_pages[i].refcnt--;
     if (shm_table.shm_pages[i].refcnt == 0) {      
       shm_table.shm_pages[i].id = 0;
       shm_table.shm_pages[i].frame = 0;
     }
     release(&(shm_table.lock));
     return 0;
   }
 } 

return 0; //added to remove compiler warning -- you should decide what to return
}

#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

#define check(exp, msg) if(exp) {} else {\
  printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg);\
  exit();}

int
main(int argc, char *argv[])
{
  struct pstat st;

  check(getpinfo(&st) == 0, "getpinfo");
  printf(1, "NPROC = %d\n", NPROC);
  int i, j;
  int pid = getpid();
  int found_self = 0;
//  int k;
//  for(i = 0; k < NPROC; k++) {
//    printf(1, "%d\n" , st.inuse[k]); ;
//     }
  for(i = 0; i < NPROC; i++) {
    if (st.inuse[i]) {
      printf(1, "pid: %d priority: %d\n ", st.pid[i], st.priority[i]);
      if (st.pid[i] == pid) {
        found_self = 1;
      }
      
      for (j = 0; j < 4; j++) {
        printf(1, "\t level %d ticks used %d\n", j, st.ticks[i][j]);  
      }
    }
  }

  if (found_self) {
    printf(1, "TEST PASSED");
  } else {
    printf(1, "TEST FAILED");
  }

  exit();
}

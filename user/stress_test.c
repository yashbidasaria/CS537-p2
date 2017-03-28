#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"

#define check(exp, msg) if(exp) {} else {				\
  printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg); \
  exit();}

int pow2[] = {5, 5, 10, 80000000};

int workload(int n) {
  int i, j = 0;
  for(i = 0; i < n; i++)
    j += i * j + 1;
  return j;
}

int
main(int argc, char *argv[])
{
  sleep(10);
  printf(1,"Entered\n");  
  struct pstat st;
  check(getpinfo(&st) == 0, "getpinfo");
   
  int i, c_pid;
  for (i = 0; i <= NPROC - 4; i++) {
    printf(1, "before fork\n");
    c_pid = fork();
    printf(1, "after fork\n");
    // Child
    if (c_pid > 0) {
      printf(1,"First if, i = %d\n", i);
      continue;

    } else if (c_pid < 0) {
      printf(1, "TEST FAILED");
      exit();

    } else {
      printf(1, "workload\n");
      workload(1000 * i);
      exit();
    }
    
    /*
    check(getpinfo(&st) == 0, "getpinfo");
    int j, k, l;
    for (k = 0; k < 10000; k++) {
      workload(1000);
      for (l = 0; l < NPROC; l++) {
        for (j = 0; j < st.priority[l]; j++) {
          check(st.ticks[l][j] == pow2[j], "incorrect #ticks at this level");
        }
        printf(1, "Ticks[%d, %d]: %d\n", l, j, st.ticks[l][j]);
        check(st.ticks[l][j] < pow2[j],
            "#ticks at this level should not exceeds the maximum allowed");
      }
    }
    */

  }

  for (i = 0; i <= NPROC - 4; i++) {
    printf(1, "wait\n");
    wait();
  }

  for (i = 0; i <= NPROC - 4; i++) {
    c_pid = fork();
    // Child
    if (c_pid > 0) {
      printf(1, "Second if, NPROC = %d\n", NPROC);
      continue;

    } else if (c_pid < 0) {
      printf(1, "TEST FAILED");
      exit();

    } else {
      workload(1000 * i);
      exit();
    }
  }

  printf(1, "TEST PASSED");
  exit();
}


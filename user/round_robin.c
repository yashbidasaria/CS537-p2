#include "types.h"
#include "stat.h"
#include "user.h"
#include "pstat.h"
#define check(exp, msg) if(exp) {} else {				\
  printf(1, "%s:%d check (" #exp ") failed: %s\n", __FILE__, __LINE__, msg); \
  exit();}

//#define DDEBUG 1

#ifdef DDEBUG
# define DEBUG_PRINT(x) printf x
#else
# define DEBUG_PRINT(x) do {} while (0)
#endif

int pow2[] = {5, 5, 10, 80000000};
int time_slices[] = {5, 5, 10, 20};

int workload(int n) {
  int i, j = 0;
  for(i = 0; i < n; i++)
    j += i * j + 1;
  return j;
}

int
main(int argc, char *argv[])
{
  struct pstat st;
  int parent_pid = getpid(), child_pid;
  int parent_idx, child_idx;
  int i;

  workload(30000000);

  // Child
  if ((child_pid = fork()) == 0) {
    child_pid = getpid();

    check(getpinfo(&st) == 0, "getpinfo");

    // find ptable indices
    for (i = 0; i < NPROC; i++) {
      // find parent process index
      if (st.pid[i] == parent_pid) {
        parent_idx = i;

      // child process index
      } else if (st.pid[i] == child_pid) {
        child_idx = i;
      }
    }
    check(parent_idx < NPROC && parent_idx >= 0, 
        "parent index in the process table should be in [0, NPROC)");
    check(child_idx < NPROC && child_idx >= 0, 
        "child index in the process table should be in [0, NPROC)");

    while (st.priority[child_idx] != 3 || st.priority[parent_idx] != 3) {
      workload(1000);
      check(getpinfo(&st) == 0, "getpinfo");
    }


    int prev_parent_ticks = st.ticks[parent_idx][3];
    for (i = 0; i < 100; i++) {
      // check round-robin at the lowest level
      // printf(1, "parent ticks: %d\n", st.ticks[parent_idx][3]);
      check(getpinfo(&st) == 0, "getpinfo");

      // Debug
      DEBUG_PRINT((1, "Parent ticks: %d, %d\n", parent_pid, st.ticks[parent_idx][3]));
      int m;
      for (m = 0; m < 4; m++) {
        DEBUG_PRINT((1, "\t level %d ticks used %d\n", m, st.ticks[parent_idx][m]));
      }

      if (st.ticks[parent_idx][3] != prev_parent_ticks) {
        if (prev_parent_ticks % time_slices[3] == 0 &&
            (st.ticks[parent_idx][3] - prev_parent_ticks) % time_slices[3] != 0) {
          printf(1, "round_robin: prev_parent %d, curr_parent %d\n",
              prev_parent_ticks, st.ticks[parent_idx][3]);
        }

        check(prev_parent_ticks % time_slices[3] != 0 ||
            (st.ticks[parent_idx][3] - prev_parent_ticks) % time_slices[3] == 0,
            "should do round-robin of 20 ticks at the bottom level");
        prev_parent_ticks = st.ticks[parent_idx][3];
      }
      workload(6000000);
    }


    printf(1, "TEST ");
    exit();

  // Parent
  } else {
    check(getpinfo(&st) == 0, "getpinfo");
    for (i = 0; i < NPROC; i++) {
      if (st.pid[i] == parent_pid)
        parent_idx = i; // parent process index
      if (st.pid[i] == child_pid)
        child_idx = i; // child process index
    }
    check(parent_idx < NPROC && parent_idx >= 0, 
        "parent index in the process table should be in [0, NPROC)");
    check(child_idx < NPROC && child_idx >= 0, 
        "child index in the process table should be in [0, NPROC)");

    while (st.priority[child_idx] != 3 || st.priority[parent_idx] != 3) {
      workload(1000);
      check(getpinfo(&st) == 0, "getpinfo");
    }


    int prev_child_ticks = st.ticks[child_idx][3];
    for (i = 0; i < 100; i++) {
      // check round-robin at the lowest level
      /* printf(1, "child ticks: %d\n", st.ticks[child_idx][3]); */
      /* printf(1, "parent ticks: %d\n", st.ticks[parent_idx][3]); */
      check(getpinfo(&st) == 0, "getpinfo");
      if (st.ticks[child_idx][3] != prev_child_ticks) {
        /* printf(1, "prev_child %d, curr_child %d\n",  */
        /*        prev_child_ticks, st.ticks[child_idx][3]); */
        if (prev_child_ticks % time_slices[3] == 0 &&
            (st.ticks[child_idx][3] - prev_child_ticks) % time_slices[3] != 0) {
          printf(1, "round_robin %d: prev_child %d, curr_child %d\n",
              i, prev_child_ticks, st.ticks[child_idx][3]);
        }

        check(prev_child_ticks % time_slices[3] != 0 ||
            (st.ticks[child_idx][3] - prev_child_ticks) % time_slices[3] == 0,
            "should do round-robin of 20 ticks at the bottom level");
        prev_child_ticks = st.ticks[child_idx][3];
      }
      workload(6000000);
    }


    while (st.ticks[parent_idx][3] % time_slices[3] != 0) {
      workload(1000);
      check(getpinfo(&st) == 0, "getpinfo");
    }
    wait();
  }

  printf(1, "PASSED\n");    
  exit();
}

#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"
#include "pstat.h"

int 
main(int argc, char *argv[]) {

  struct pstat* ps;
  ps = (pstat*)malloc(sizeof(pstat));
  
  int status = getpinfo(ps);













   exit();

}

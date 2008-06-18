#ifndef _util_sig_h
#define _util_sig_h

#include <signal.h>

typedef struct sigdef {
  const char *name;
  int sig;
  const char *desc;
  void *user[4];
} sigdef;

sigdef *sigdef_by_name(const char *signame);
sigdef *sigdef_by_sig(int sig);
sigdef *sigdef_begin();
sigdef *sigdef_end();
sigdef *sigdef_min_sig();
sigdef *sigdef_max_sig();

#endif

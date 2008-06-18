#ifndef __rcs_id__
#ifndef __rcs_id_ll_posix_c__
#define __rcs_id_ll_posix_c__
static const char __rcs_id_ll_posix_c[] = "$Id: posix.c,v 1.12 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"

#include <stdlib.h>
#include <unistd.h>
#include <errno.h> /* errno */
#include <sys/types.h>
#include <sys/wait.h> /* wait, waitpid */
#include <sys/stat.h> /* umask() */
#include <time.h> /* time() */


ll_define_primitive(object, posixCerrno, _0(), _0())
{
  ll_return(ll_make_fixnum(errno));
}
ll_define_primitive_end


ll_define_primitive(object, posixCabort, _0(), _0())
{
  abort();
}
ll_define_primitive_end


ll_define_primitive(object, posixCexit, _0(), _0())
{
  exit(0);
}
ll_define_primitive_end


ll_define_primitive(fixnum, posixCexit, _1(rtncode), _0())
{
  exit(ll_unbox_fixnum(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(string, posixCsystem, _1(cmd), _0())
{
  const char *str = ll_THIS->_array;
  ll_return(ll_make_fixnum(system(str)));
}
ll_define_primitive_end


ll_define_primitive(fixnum, posixCumask, _1(val), _0())
{
  int v = ll_UNBOX_fixnum(ll_ARG_0);
  ll_return(ll_make_fixnum(umask(v)));
}
ll_define_primitive_end


ll_define_primitive(string, posixCchdir, _1(dir), _0())
{
  const char *str = ll_THIS->_array;
  ll_return(ll_make_fixnum(chdir(str)));
}
ll_define_primitive_end


ll_define_primitive(object, posixCgetcwd, _0(), _0())
{
  char buf[1024]; /* FIXME */
  if ( getcwd(buf, sizeof(buf) - 1) == 0 ) {
    ll_return(errno);
  }
  ll_return(ll_make_string(buf, -1));
}
ll_define_primitive_end


ll_define_primitive(object, posixCgetuid, _0(), _0())
{
  ll_return(ll_make_fixnum(getuid()));
}
ll_define_primitive_end


ll_define_primitive(object, posixCgetgid, _0(), _0())
{
  ll_return(ll_make_fixnum(getgid()));
}
ll_define_primitive_end


ll_define_primitive(object, posixCgetpid, _0(), _0())
{
  ll_return(ll_make_fixnum(getpid()));
}
ll_define_primitive_end


ll_define_primitive(object, posixCgetppid, _0(), _0())
{
  ll_return(ll_make_fixnum(getppid()));
}
ll_define_primitive_end


ll_define_primitive(object, posixCfork, _0(), _0())
{
  ll_return(ll_make_fixnum(fork()));
}
ll_define_primitive_end


ll_define_primitive(object, posixCwait, _0(), _0())
{
  pid_t pid = 0;
  int status = 0;

  pid = wait(&status);

  ll_return(ll_cons(ll_make_fixnum(pid), ll_make_fixnum(status)));
}
ll_define_primitive_end


ll_define_primitive(fixnum, posixCwaitpid, _1(pid), _0())
{
  pid_t pid = ll_UNBOX_fixnum(ll_SELF);
  int status = 0;
  int options = 0; /* | WNOHANG; */

  pid = waitpid(pid, &status, options);

  ll_return(ll_cons(ll_make_fixnum(pid), ll_make_fixnum(status)));
}
ll_define_primitive_end


ll_define_primitive(object, posixCtime, _0(), _0())
{
  time_t t = 0;
  time(&t);
  ll_return(ll_make_fixnum(t));
}
ll_define_primitive_end


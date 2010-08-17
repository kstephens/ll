#!/bin/sh

POST_TEST='(%lcache-all) (append (%lcache-stats) (%mem-stats))'
POST_TEST='(append (%lcache-stats) (%mem-stats))'

LLT="${LLT:-./llt}"
LL_LOAD_PATH="${LL_LOAD_PATH:-`/bin/pwd`/lib/ll}"
export LL_LOAD_PATH

EXEC_WITH=''
VALGRIND="${VALGRIND:-valgrind}"
VALGRIND_OPTS=
#VALGRIND_OPTS="${VALGRIND_OPTS} -q"
VALGRIND_OPTS="${VALGRIND_OPTS} --error-limit=no"
VALGRIND_OPTS="${VALGRIND_OPTS} --track-origins=yes"
VALGRIND_OPTS="${VALGRIND_OPTS} --track-fds=yes"
VALGRIND_OPTS="${VALGRIND_OPTS} --read-var-info=yes"
#VALGRIND_OPTS="${VALGRIND_OPTS} --heap=yes"
if [ -n "${EXEC_WITH_VALGRIND}" ]
then
  EXEC_WITH="${VALGRIND} ${VALGRIND_OPTS} "
fi
EXEC_WITH_GDB="gdb -x ./run.gdb --args"#

#ifneq "$(strip $(EXEC_GDB))" ""
#EXEC_WITH=$(EXEC_WITH_GDB)
#endif

errors=0

tr() {
  echo "t $@"
  (set -x
  ${EXEC_WITH} ${LLT} -p -e "$@"
  )
}
t_passed() {
    echo "t $*: PASSED"
}
t_failed() {
    echo "t $*: FAILED"
    errors=1
    if [ -n "$RETRY_WITH_GDB" ]
    then
      set -x
      gdb -x ./run.gdb --args ${LLT} -p -e "$@"
    fi
    return 1
}

t() {
   if tr "$@"
   then
     t_passed "$@"
     return 0
   else
     t_failed "$@"
     return 1
   fi
}

t_fail() {
   if tr "$@"
   then
     return 1
   else
     t_passed "$@"
     return 0
   fi
}

set -e

###########################################################

t_fail '(10 20)'
t_fail '(load "test/ll_test.scm") (ll:test:assert 5 10)'

t '(load "lib/ll/test/slot_closure.ll")'
t '(load "lib/ll/test/locative.scm")'
t '(load "lib/ll/test/lambda.scm")'
t '(load "lib/ll/test/closed.scm")'
t '(load "lib/ll/test/mycons.scm")'
#t '(set! *debug:compile-emit* 1) (load "lib/ll/test/let.scm")'
t '(load "lib/ll/test/let.scm")'
t '(load "lib/ll/test/string.scm")'
t '(load "lib/ll/test/number.scm")'
t '(load "lib/ll/test/body_define.scm")'
t '(load "lib/ll/type.ll") (type-ancestors <fixnum>)'
t '(posix:chdir "lib/ll/test") (load "test.scm") (test-sc4)'
t '(load "lib/ll/test/tak_test.scm")'
t '(load "pp.scm") (pretty-print nil)'

###########################################################

exit $errors

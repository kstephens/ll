#!/bin/sh

POST_TEST='(%lcache-all) (append (%lcache-stats) (%mem-stats))'
POST_TEST='(append (%lcache-stats) (%mem-stats))'

LLT="${LLT:-./llt}"
LL_LOAD_PATH="${LL_LOAD_PATH:-`/bin/pwd`/lib/ll}"
export LL_LOAD_PATH

EXEC_WITH=''
if [ -n "${VALGRIND}" ]
then
  EXEC_WITH="valgrind -q --error-limit=no "
fi
EXEC_WITH_GDB=gdb -x ./run.gdb --args#

#ifneq "$(strip $(EXEC_GDB))" ""
#EXEC_WITH=$(EXEC_WITH_GDB)
#endif

errors=0

tr() {
  echo "t $@"
  ${EXEC_WITH} ${LLT} -p -e "$@"
}
t_passed() {
    echo "t $*: PASSED"
}
t_failed() {
    errors=1
    gdb -x ./run.gdb ${LLT} --args -p -e "$@"
}

t() {
   if tr "$@"
   then
     t_passed "$@"
     return 0
   else
     errors=1
     echo "t $*: FAILED"
     gdb -x ./run.gdb --args "$@"
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

t '(load "lib/ll/type.ll") (type-ancestors <fixnum>)'
t '(load "lib/ll/test/slot_closure.ll")'
t '(load "test/locative.scm")'
t '(load "test/lambda.scm")'
t '(load "test/closed.scm")'
t '(load "test/mycons.scm")'
t '(load "test/body_define.scm")'
t '(load "test/tak_test.scm")'
t '(posix:chdir "lib/ll/test") (load "test.scm") (test-sc4)'
t '(load "pp.scm") (pretty-print nil)'

###########################################################

exit $errors
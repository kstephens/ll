#!/bin/sh

scm="${1:-lib/ll/test/tak_test.scm}"
cp ${scm} test.exit.scm
echo '(exit)' >> test.exit.scm

IKARUS_ROOT="${IKARUS_ROOT:-$HOME/local/ikarus/trunk}"
LARCENY_ROOT="${LARCENY_ROOT:-$HOME/local/larceny}"
export LARCENY_ROOT
OAKLISP_ROOT="${OAKLISP_ROOT:-$HOME/local/oaklisp}"

run() {
time -f 'T ikarus		%e sec' ${IKARUS_ROOT}/bin/ikarus test.exit.scm

chicken ${scm} -output-file ./test.chicken.c
gcc ./test.chicken.c -o ./test.chicken -lchicken
time -f 'T chicken		%e sec (precompiled)' ./test.chicken

time -f 'T mzscheme    	%e sec' mzscheme -e '(load "./test.exit.scm")'

(echo '(load "'${scm}'")'; echo ',exit') | time -f 'T scheme48		%e' scheme48 

time -f 'T scheme-r5rs 	%e sec' scheme-r5rs ${scm}

time -f 'T guile		%e sec' guile -c '(load "'${scm}'")'

cp test.exit.scm test.exit.oak
time -f 'T oaklisp		%e sec' ${OAKLISP_ROOT}/bin/oaklisp -- --eval '(load "'test.exit.oak'")'

time -f 'T larceny     	%e sec' ${LARCENY_ROOT}/petit-larceny.bin -heap ${LARCENY_ROOT}/petit-larceny.heap -r5rs -- -e '(load "./test.exit.scm")'

time -f 'T LL  		%e sec' ./llt -p -e '(load "'${scm}'")'
}

# run
echo "S ${scm}"
if [ -n "$verbose" ]
then
  run
else
  run </dev/null 2>&1 | egrep -e '^T ' | sed -e 's/^T //' | sort -n -k 2
fi

rm -f test.exit.* test.chicken test.chicken.c


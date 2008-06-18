#!/bin/sh
# $Id: defs.sh,v 1.9 2007/12/20 09:43:31 stephens Exp $
o="$$"
f=''
t=''
tmp="$$.tmp"
opts=
cat /dev/null > "$tmp"

while [ $# -gt 0 ]
do
  case "$1"
  in
    -)
      var="$2"; val="$3"; shift 2
      eval "$var='$val'"
    ;;
    *)
      echo "  " ${CPP} ${CPPFLAGS} "-D$f=$f" "-D$t=$t" "$1"
      if ${CPP} ${CPPFLAGS} "-D$f=$f" "-D$t=$t" "$1" >> "$tmp"
      then
        true
      else 
	echo "${CPP} errors" 1>&2
        rm $tmp
        exit 1
      fi
    ;;
  esac
  shift
done

# echo "$0: file: $tmp" 1>&2
perl defs.pl $opts "$f" "$t" < "$tmp" > "${o}n"

if cmp -s "${o}n" "${o}"
then
  echo "    ${o} unchanged" 
  true
else
  echo "    ${o} changed"
  mv "${o}n" "${o}"
fi
rm -f "${o}n" "$tmp"

exit 0


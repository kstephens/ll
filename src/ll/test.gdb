# $Id: test.gdb,v 1.2 1999/02/19 09:25:59 stephensk Exp $
break main
break abort
break _ll_error
#break exit
#break _ll_init_symbol
run

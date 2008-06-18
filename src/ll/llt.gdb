# $Id: llt.gdb,v 1.9 2008/05/26 06:35:03 stephens Exp $


#break _ll_pf__ir__ir_const_index
#break _ll_pf_byte_code_method_initialize
#break main

#break abort

#break _ll_pf__bmethod_apply_C_object

#break _ll_error

#break _ll_vs_push
#break _ll_ar_push
#break _ll_vs_pop
#break _ll_ar_pop

#break _ll_pf_message__print_backtrace
#break _ll_pf_message__print_frame
#break _ll_pf_debugger_initialize
#break _ll_pf_object_debugger
#break _ll_pf_error_debugger

run

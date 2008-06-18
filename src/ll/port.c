#ifndef __rcs_id__
#ifndef __rcs_id_ll_port_c__
#define __rcs_id_ll_port_c__
static const char __rcs_id_ll_port_c[] = "$Id: port.c,v 1.26 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>


/******************************************************************/
/* (current-output-port) */


ll_define_primitive(object, current_output_port, _0(), _1(no_side_effect,"#t"))
{
  ll_return(ll_g(_current_output_port));
}
ll_define_primitive_end


ll_define_primitive(output_port, set_current_output_port, _1(port), _0())
{
  ll_set_g(_current_output_port, ll_SELF);
}
ll_define_primitive_end


/******************************************************************/
/* (current-input-port) */


ll_define_primitive(object, current_input_port, _0(), _1(no_side_effect,"#t"))
{
  ll_return(ll_g(_current_input_port));
}
ll_define_primitive_end


ll_define_primitive(input_port, set_current_input_port, _1(port), _0())
{
  ll_set_g(_current_input_port, ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(input_port, read_char, _1(port), _0())
{
  ll_call_tail(ll_o(read_char), _1(ll_call(ll_o(port_impl), _1(ll_SELF))));
}
ll_define_primitive_end


ll_define_primitive(input_port, peek_char, _1(port), _0())
{
  ll_call_tail(ll_o(peek_char), _1(ll_call(ll_o(port_impl), _1(ll_SELF))));
}
ll_define_primitive_end


/******************************************************************/
/* (current-error-port) */


ll_define_primitive(object, current_error_port, _0(), _1(no_side_effect,"#t"))
{
  ll_return(ll_g(_current_error_port));
}
ll_define_primitive_end


ll_define_primitive(output_port, set_current_error_port, _1(port), _0())
{
  ll_set_g(_current_error_port, ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(output_port, _output_char, _2(port, char), _0())
{
  char c = ll_unbox_char(ll_ARG_1);
  ll_write_string(ll_SELF, &c, 1);
}
ll_define_primitive_end


ll_define_primitive(output_port, _output_string, _2(port, str), _0())
{
  ll_call_tail(ll_o(_output_string), _2(ll_call(ll_o(port_impl), _1(ll_SELF)), ll_ARG_1));
}
ll_define_primitive_end


/******************************************************************/


ll_define_primitive(port, initialize, __2(port, impl, close_ident), _0())
{
  ll_call_super(ll_o(initialize), ll_f, _1(ll_SELF));
  ll_THIS->_impl = ll_ARG_1;
  ll_THIS->_close = ll_ARGC >= 3 ? ll_ARG_2 : ll_f;
  ll_THIS->_ident = ll_ARGC >= 4 ? ll_ARG_3 : ll_f;
  ll_write_barrier_SELF();
  ll_return(ll_SELF);
}
ll_define_primitive_end


ll_define_primitive(port, port_impl, _1(port), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_impl);
}
ll_define_primitive_end


ll_define_primitive(port, port_ident, _1(port), _1(no_side_effect,"#t"))
{
  ll_return(ll_THIS->_ident);
}
ll_define_primitive_end


ll_define_primitive(port, flush, _1(port), _0())
{
}
ll_define_primitive_end


/******************************************************************/


#define CLOSE  ll_THIS_ISA(port,ll_SELF)->_close
#define IMPL   ll_THIS_ISA(port,ll_SELF)->_impl
#define GET_FP FILE* FP = (FILE*) IMPL


ll_define_primitive(file_port, flush, _1(port), _0())
{
   GET_FP;
   if ( FP ) {
     fflush(FP);
   }
}
ll_define_primitive_end


ll_define_primitive(file_port, _close_port, _1(port), _0())
{
  if ( ll_unbox_boolean(CLOSE) && IMPL ) {
    GET_FP;
    IMPL = 0;
    CLOSE = ll_f;
    ll_write_barrier_SELF();
    if ( FP ) {
      fclose(FP);
    }
  }
}
ll_define_primitive_end


/******************************************************************/


ll_define_primitive(input_file_port, read_char, _1(port), _0())
{
  GET_FP;
  int c;
  ll_return(FP && ((c = fgetc(FP)) != EOF) ? ll_make_char(c) : ll_eos);
}
ll_define_primitive_end


ll_define_primitive(input_file_port, peek_char, _1(port), _0())
{
  GET_FP;
  int c;
  if ( FP && ((c = fgetc(FP)) != EOF) ) {
    ungetc(c, FP);
    ll_return(ll_make_char(c));
  } else {
    ll_return(ll_eos);
  }
}
ll_define_primitive_end


/******************************************************************/


ll_define_primitive(output_file_port, _output_string, _2(port, string), _0())
{
  GET_FP;
  size_t count;
  count = FP ? 
    fwrite(ll_ptr_string(ll_ARG_1),
	   ll_len_string(ll_ARG_1),
	   sizeof(char), 
	   FP) : 
    -1;
  /* FIX ME: write error */
}
ll_define_primitive_end


ll_define_primitive(output_file_port, _output_char, _2(port, c), _0())
{
  GET_FP;
  if ( FP ) 
    putc(ll_unbox_char(ll_ARG_1), FP);
}
ll_define_primitive_end


/******************************************************************/


ll_v ll_write_string(ll_v port, const char *v, size_t l)
{
  if ( ll_EQ(port, ll_undef) ) {
    port = ll_call(ll_o(current_output_port), _0());
  }

  if ( l == (size_t) -1 ) {
    l = strlen(v);
  }
#if 1
  {
    ll_tsa_string _str;
    ll_v str = ll_make_ref(&_str);

    ll_TYPE_ref(str) = ll_type(string);
    _str._length = l;
    _str._array = (void*) v;

    ll_call(ll_o(_output_string), _2(port, str));
  }
#else
  {
    ll_v str;
    str = ll_call(ll_o(make), _3(ll_type(string), ll_make_fixnum(l), ll_BOX_PTR(v)));
    ll_call(ll_o(_output_string), _2(port, str));
  }
#endif

  return port;
}



/******************************************************************/
/* 6.6.1. Ports */


ll_define_primitive(string, _open_file, _2(filename, mode), _0())
{
  const char *filename, *mode;
  FILE *fp;
  ll_v impl;
  ll_v type;

  filename = ll_ptr_string(ll_SELF);
  mode = ll_ptr_string(ll_ARG_1);
  type = strchr(mode, 'w') ? ll_type(output_file_port) : ll_type(input_file_port);

  if ( (fp = fopen(filename, mode)) ) {
    impl = (ll_v) fp;
  } else {
    ll_return(_ll_error(ll_re(file_open), 
			4, 
			ll_s(name), ll_SELF, 
			ll_s(mode), ll_ARG_1, 
			ll_s(code), ll_make_fixnum(errno),
			ll_s(reason), ll_make_string(strerror(errno), -1)
			));
  }

  ll_call_tail(ll_o(make), _4(type, impl, ll_t, ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(string, open_input_file, _1(filename), _0())
{
  ll_v mode = ll_make_string("rb", 2);
  ll_call_tail(ll_o(_open_file), _2(ll_SELF, mode));
}
ll_define_primitive_end


ll_define_primitive(string, open_output_file, _1(filename), _0())
{
  ll_v mode = ll_make_string("wb", 2);
  ll_call_tail(ll_o(_open_file), _2(ll_SELF, mode));
}
ll_define_primitive_end


ll_define_primitive(input_port, close_input_port, _1(port), _0())
{
  ll_call_tail(ll_o(_close_port), _1(ll_SELF));
}
ll_define_primitive_end


ll_define_primitive(output_port, close_output_port, _1(port), _0())
{
  ll_call_tail(ll_o(_close_port), _1(ll_SELF));
}
ll_define_primitive_end



/***************************************************************************/
/* These could probably be defined in scheme. 
 * IMPLEMENT: use unwind-protect.
 */


ll_define_primitive(string, call_with_input_file, _2(file, proc), _0())
{
  ll_v result = ll_undef;

  ll_v port = ll_call(ll_o(open_input_file), _1(ll_SELF));

  result = ll_call(ll_ARG_1, _1(port));

  ll_call(ll_o(close_input_port), _1(port));

  ll_return(result);

}
ll_define_primitive_end


ll_define_primitive(string, call_with_output_file, _2(file, proc), _0())
{
  ll_v result = ll_undef;

  ll_v port = ll_call(ll_o(open_output_file), _1(ll_SELF));

  result = ll_call(ll_ARG_1, _1(port));

  ll_call(ll_o(close_output_port), _1(port));

  ll_return(result);

}
ll_define_primitive_end



/******************************************************************/
/* null ports */


ll_define_primitive(null_input_port, read_char, _1(port), _0())
{
  ll_return(ll_eos);
}
ll_define_primitive_end


ll_define_primitive(null_input_port, peek_char, _1(port), _0())
{
  ll_return(ll_eos);
}
ll_define_primitive_end


ll_define_primitive(null_output_port, _output_string, _2(port, string), _0())
{
}
ll_define_primitive_end


ll_define_primitive(null_output_port, _output_char, _2(port, c), _0())
{
}
ll_define_primitive_end


/***************************************************************************/
/* string as input port */


ll_define_primitive(mutable_string, read_char, _1(port), _0())
{
  if ( ll_THIS->super_string._length ) {
    ll_THIS->super_string._length --;
    ll_return(ll_make_char(*(ll_THIS->super_string._array ++)));
  } else {
    ll_return(ll_eos);
  }
}
ll_define_primitive_end


ll_define_primitive(mutable_string, peek_char, _1(port), _0())
{
  if ( ll_THIS->super_string._length ) {
    ll_return(ll_make_char(*(ll_THIS->super_string._array)));
  } else {
    ll_return(ll_eos);
  }
}
ll_define_primitive_end


/***************************************************************************/
/* string as (port-impl output-port) */


ll_define_primitive(mutable_string, _output_string, _2(port, string), _0())
{
  ll_len_string(ll_ARG_1); /* typecheck */
  ll_call_tail(ll_o(string_appendE), _2(ll_SELF, ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(mutable_string, _output_char, _2(port, char), _0())
{
  ll_call_tail(ll_o(append_oneE), _2(ll_SELF, ll_ARG_1));
}
ll_define_primitive_end


/******************************************************************/

ll_define_primitive(object, Scurrent_input_portS, _0(), _0())
{
  ll_return(ll_g(_current_input_port));
}
ll_define_primitive_end

ll_define_primitive(object, Scurrent_output_portS, _0(), _0())
{
  ll_return(ll_g(_current_output_port));
}
ll_define_primitive_end

ll_define_primitive(object, Scurrent_error_portS, _0(), _0())
{
  ll_return(ll_g(_current_error_port));
}
ll_define_primitive_end


/******************************************************************/

ll_INIT(port, 220, "eos-object, stdio ports")
{
  ll_set_g(__eos, ll_call(ll_o(make), _1(ll_type(eof_object))));
  _ll_set_object_nameE(ll_eos, "#<eof-object>");

  ll_set_g(_current_input_port, ll_call(ll_o(make), _4(ll_type(input_file_port), (ll_v) stdin, ll_f, ll_s(SstdinS))));
  _ll_set_object_nameE(ll_g(_current_input_port), "#<<input-file-port> stdin>");

  ll_set_g(_current_output_port, ll_call(ll_o(make), _4(ll_type(output_file_port), (ll_v) stdout, ll_f, ll_s(SstdoutS))));
  _ll_set_object_nameE(ll_g(_current_output_port), "#<<output-file-port> stdout>");

  ll_set_g(_current_error_port, ll_call(ll_o(make), _4(ll_type(output_file_port), (ll_v) stderr, ll_f, ll_s(SstderrS))));
  _ll_set_object_nameE(ll_g(_current_error_port), "#<<output-file-port> stderr>");

  return 0;
}

#undef IMPL
#undef CLOSE
#undef FP


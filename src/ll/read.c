#ifndef __rcs_id__
#ifndef __rcs_id_ll_read_c__
#define __rcs_id_ll_read_c__
static const char __rcs_id_ll_read_c[] = "$Id: read.c,v 1.15 2008/01/06 18:36:33 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include <stdio.h>


#define EOS ll_eos


static int PEEKC(ll_v stream)
{
  ll_v c = ll_call(ll_o(peek_char), _1(stream));
  return ll_EQ(c, ll_eos) ? EOF : ll_unbox_char(c);
}
#define PEEKC PEEKC

static int GETC(ll_v stream)
{
  ll_v c = ll_call(ll_o(read_char), _1(stream));
  return ll_EQ(c, ll_eos) ? EOF : ll_unbox_char(c);
}


#define EQ(X,Y) ll_EQ(X,Y)


static ll_v _ll_make_immutable(ll_v x)
{
  return(ll_call(ll_o(make_immutable), _1(x)));
}


#define CONS(X,Y) ll_cons((X),(Y))
#define IMMUTABLE_CONS(X)_ll_make_immutable(X)
#define NIL ll_nil
#define CAR(X) ll_car(X)
#define SET_CDR(X,V) (ll_CDR(X) = V)
#define SET(X,V) ((X) = (V))

#define SYMBOL(S) ll_s(S)
#define SYMBOL_DOT ll_s(D)

#define VALUE ll_v
#define READ_DECL static VALUE _ll_read(VALUE stream)
#define READ_CALL() _ll_read(stream)
#define RETURN(X) return(X)

#define F ll_f
#define T ll_t
#define U ll_undef

static char error_buf[128]; /* GLOBAL */

#define ERROR(X...) _ll_error(ll_re(read), 1, ll_s(reason), ll_make_copy_string((sprintf(error_buf, X), error_buf), -1))
#define MALLOC(X) ll_malloc(X)
#define REALLOC(X,Y) ll_realloc(X,Y)

#define IMMUTABLE_STRING(X) _ll_make_immutable(X)
#define ESCAPE_STRING(X) ll_call(ll_o(_string_unescape), _1(X))
#define STRING(V, L) ll_make_string((V), (L))

#define LIST_2_VECTOR(X) _ll_make_immutable(ll_call(ll_o(list__vector), _1((X))))
#define IMMUTABLE_VECTOR(X)_ll_make_immutable(X)

#define STRING_2_NUMBER(X,R) ll_call(ll_o(string__number), _2((X), ll_make_fixnum(R)))
#define STRING_2_SYMBOL(S) ll_call(ll_o(string__symbol), _1((S)))
#define MAKE_CHAR(C) ll_make_char(C)


/*************************************************************************/
/* # read macro support. */


ll_define_primitive(char, llCreadCundefined_macro_character, _2(char, port), _0())
{
  int c = ll_UNBOX_char(ll_SELF);
  ERROR("bad macro sequence: #%c", c);
  ll_return(ll_nil);
}
ll_define_primitive_end


ll_define_primitive(char, llCreadCdefine_macro_char, _2(char, proc), _0())
{
  int C = ll_UNBOX_char(ll_SELF);
  ll_call_tail(ll_o(vector_setE), _3(ll_fluid(ll_s(llCreadCmacro_char_table)), ll_make_fixnum(C), ll_ARG_1));
}
ll_define_primitive_end


ll_INIT(read, 240, "read macro table")
{
  ll_v v = ll_make_copy_vector(0, ll_MAX_char + 1);
  ll_call(ll_o(vector_fillE), _2(v, ll_o(llCreadCundefined_macro_character)));
  ll_bind_fluid(ll_s(llCreadCmacro_char_table), v);

  return 0;
}

#define CALL_MACRO_CHAR(C) ll_call(ll_call(ll_o(vector_ref), _2(ll_fluid(ll_s(llCreadCmacro_char_table)), ll_make_fixnum(C))), _2(ll_make_char(C), stream))

/*************************************************************************/

READ_DECL;

/*************************************************************************/

ll_define_primitive(object, read, __0(port), _0())
{
  ll_v port = ll_ARGC >= 1 ? ll_SELF : ll_call(ll_o(current_input_port), _0());
  ll_return(_ll_read(port));
}
ll_define_primitive_end

/*************************************************************************/

#include "lispread.c"

/*************************************************************************/

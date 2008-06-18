#ifndef _ll_char_h
#define _ll_char_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_char_h__
#define __rcs_id_ll_char_h__
static const char __rcs_id_ll_char_h[] = "$Id: char.h,v 1.2 2008/05/26 00:19:34 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#ifndef ll_CHAR_STATIC_ALLOC
#define ll_CHAR_STATIC_ALLOC 1 /* if non-zero, char objects are allocated statically. */
#endif

#include "ll/ll.h" /* ll_v */

#define ll_MAX_char ((int)(unsigned char) ~0)

struct ll_tsa_char;
typedef struct ll_tsa_char_table {
#if ll_CHAR_STATIC_ALLOC
  /* Can't use ll_tsa_char, yet because its not defined yet.
  ** So assume that:
  struct ll_tsa_char {
    ll_v type; 
    ll_v value;
  };
  */
  ll_v _char_s[ll_MAX_char + 1][2]; /* [ <char>, value ] */
#define ll_BOX_char(X)   ll_BOX_ref(&_ll_char_table._char_s[(X) & 0xff])
#else
  ll_v _char_v[ll_MAX_char + 1];
#define ll_BOX_char(X)   (_ll_char_table._char_v[(X) & 0xff])
#endif
} ll_tsa_char_table;

#define _ll_char_table ll_runtime(_char_table)

#define ll_make_char(X)  ll_BOX_char(X)
#define ll_UNBOX_char(X) ll_UNBOX_fixnum(ll_SLOTS(X)[1])
/* #define ll_unbox_char(X)ll_UNBOX_char(X) */
int ll_unbox_char(ll_v x);

#endif


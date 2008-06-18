#ifndef _ll_BCOMPILE_H
#define _ll_BCOMPILE_H


#include "ll.h"
#include "bcode.h"
#include "assert.h"
#include <string.h> /* strncmp() */


extern int _ll_bcc_1;
extern int _ll_bcc_2;


/* A var scope binding is
**
** (<var> <ir> [arg|slot|env|glo] <index> <closed-over?> <referenced?>)
*/
#define var_name(x) ll_car(x)
  /* The name of the binding */

#define var_type(x) ll_cadr(x)
/*
  The type of binding:

  local
    a temporary value located on the stack.  ll_ARGV[-var_index(x)]
    a non-closed over restarg or car-pos lambda's arguments are of this type of binding.

  arg
    an argument value located on the stack.  ll_ARGV[var_index(x)]

  slot
    a slot value named var_name(x).

  exp
    a value in our exported environment vector. exp[var_index(x)]
    closed-over arguments and restargs are placed in the exported environment by the lambda's preamble.

  env
    a value located directly in our imported environment vector.  env[var_index(x)]
    arguments from a parent lambda that are closed-over are located directly in the
    exported environment vector.

  env-loc
    a value referenced by a locative in our imported environment vector. *env[var_index(x)]

  glo
    a global value named var_name(x).
*/

#define var_index(x) ll_caddr(x)
#define set_var_index(x,y) ll_set_caddrE(x,y)
/* The location of the binding:
** 
** arg type: the index is off the frame pointer
** exp or env: the index is off the exported or emported environment vector.
*/

#define var_closed_overQ(x) ll_cadddr(x)
#define set_var_closed_overQ(x,y) ll_set_cadddrE(x,y)
/* The position in our exports vector, #f if not closed-over */

#define var_referencedQ(x) ll_caddddr(x)
#define set_var_referencedQ(x,y) ll_set_caddddrE(x,y)
/* Is the variable referenced? */

#define var_rest_argQ(x) ll_cadddddr(x)
#define set_var_rest_argQ(x,y) ll_set_cadddddrE(x,y)
/* Is the variable a rest arg? */

#endif

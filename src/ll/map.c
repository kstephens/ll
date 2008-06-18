#ifndef __rcs_id__
#ifndef __rcs_id_ll_map_c__
#define __rcs_id_ll_map_c__
static const char __rcs_id_ll_map_c[] = "$Id: map.c,v 1.18 2007/12/18 10:37:00 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ll.h"
#include "call_int.h"

ll_define_primitive(operation, apply, __2(proc, list, lists), _0())
{
  /* 6.4. Control features

     (apply <proc> <arg1> ... <args>)     procedure

     <Proc> must be a procedure and <args> must be a list.  Calls <proc> with
     the elements of the list (append (list <arg1> ...) <args>) as the actual arguments.
  */

  int i = ll_ARGC - 1;
  int l = 0;

  _ll_VS_PROBE(ll_ARGC);

  if ( ! ll_nullQ(ll_ARGV[i]) ) {
    ll_v args;
    
    /* Determine stack probe. */
    
    for ( args = ll_ARGV[i], l = 0; ! ll_nullQ(args); args = ll_cdr(args), l ++ ) {
      /* NOTHING */
    }
    
    _ll_VS_PROBE(l + i);
    
    /* Push <args>'s elements on to the stack in reverse order */
    for ( args = ll_ARGV[i]; ! ll_nullQ(args); args = ll_cdr(args) ) {
      /* Leave <args>'s car on stack */
      _ll_PUSH(args);
      _ll_callv(ll_o(car), 1);
    }
    
    /* Then reverse them */
    {
      ll_v *v = (ll_v *) _ll_val_sp;
      int j, k;
      
      for ( j = l, k = 0; -- j > k; k ++ ) {
	ll_v t = v[j];
	v[j] = v[k];
	v[k] = t;
      }
    }
  } else {
    _ll_VS_PROBE(i);
  }

  /* Now push <arg1> ... in reverse order */
  
  while ( -- i > 0 ) {
    _ll_PUSH(ll_ARGV[i]);
    l ++;
  }

  /* tail-call <proc> */
  {
    ll_v op = ll_SELF;
    _ll_call_tailv(op, l);
  }
}
ll_define_primitive_end


ll_define_primitive(operation, for_each, __2(proc, list, lists), _0())
{
  /* While there are elements in the lists; */
  while ( ! ll_nullQ(ll_ARG_1) ) {
    /*
    ** Push each list's car in reverse order so they are on the stack in
    ** proper argument order.
    */
    _ll_VS_PROBE(ll_ARGC);

    {
      int i;

      for ( i = ll_ARGC; -- i > 0 ; ) {
	/* Get the car of the list; leave result on stack */
	_ll_PUSH(ll_ARGV[i]);
	_ll_callv(ll_o(car), 1);
	
	/* Advance the list */
	ll_ARGV[i] = ll_cdr(ll_ARGV[i]);
      }
    }

    /* Call the proc with the car's of the lists on the stack */
    {
      int argc = ll_ARGC - 1; /* ll_ARGC is side-effected within _ll_callv() */
      ll_v op = ll_SELF;
      _ll_callv(op, argc);
      _ll_POP(); /* discard result */
      _ll_VS_POP(_ll_val_sp); /* DONT DO THIS? */
    }
  }
}
ll_define_primitive_end


ll_define_primitive(operation, map, __2(proc, list, lists), _0())
{
  ll_v result = ll_nil, *xp = &result;

  /* While there are elements in the lists; */
  while ( ! ll_nullQ(ll_ARG_1) ) {
    /*
    ** Push each list's car in reverse order so they are on the stack in
    ** proper argument order.
    */
    _ll_VS_PROBE(ll_ARGC);

    {
      int i;

      for ( i = ll_ARGC; -- i > 0 ; ) {
	/* Get the car of the list; leave result on stack */
	_ll_PUSH(ll_ARGV[i]);
	_ll_callv(ll_o(car), 1);
	
	/* Advance the list */
	ll_ARGV[i] = ll_cdr(ll_ARGV[i]);
      }
    }

    /* Call the proc with the car's of the lists on the stack */
    {
      int i = ll_ARGC - 1; /* ll_ARGC is side-effected within _ll_callv()! */
      ll_v op = ll_SELF;
      _ll_callv(op, i);
    }

    /* Cons the result */
    {
      ll_v x = _ll_POP();
      _ll_VS_POP(_ll_val_sp); /* DONT DO THIS? */
      
      *xp = ll_cons(x, ll_nil);
    }

    xp = &ll_CDR(*xp);
  }

  ll_return(result);
}
ll_define_primitive_end


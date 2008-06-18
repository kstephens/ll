#ifndef __rcs_id__
#ifndef __rcs_id_ll_sig_c__
#define __rcs_id_ll_sig_c__
static const char __rcs_id_ll_sig_c[] = "$Id: sig.c,v 1.12 2007/12/18 10:37:00 stephens Exp $";
#endif
#endif /* __rcs_id__ */


#include "ll.h"
#include "call_int.h"
#include "util/sig.h"
#include "assert.h"


#include "../util/sig.c"


/****************************************************************************/


#define ll_sig_proc(s) (*(ll_v*)((s)->user))


static volatile int _ll_sig_queue_head, _ll_sig_queue_tail; /*@ Index into %_ll_sig_queue%. */
volatile int _ll_sig_pending; /*@ True if signals are pending service. */


#ifndef _ll_sig_queue_len
#define _ll_sig_queue_len 32
#endif


static volatile sigdef * _ll_sig_queue[_ll_sig_queue_len]; /*@ Queue for signals to be serviced. */


volatile int _ll_sig_disabled; /*@ True if signals are being ignored. */


static
void _ll_sig_handler(int sig)
{
  sigdef *s = sigdef_by_sig(sig);
  int i;

#ifdef SIG_UNBLOCK
  if ( sig == SIG_UNBLOCK )
    abort();
#endif

  if ( _ll_sig_disabled )
    return;

  if ( ! s ) {
    fprintf(stderr, "_ll_sig_handler(%d): unknown signal\n", sig);
    return;
  }

  /* Check queue for pre-existing signal. */
  for ( i = _ll_sig_queue_head; i != _ll_sig_queue_tail; i = (i + 1) % _ll_sig_queue_len ) {
    if ( _ll_sig_queue[i] == s ) {
      fprintf(stderr, "_ll_sig_handler(SIG%s): already queued\n", s->name);
#ifdef SIGSEGV
      if ( sig == SIGSEGV )
	abort();
#endif
      return;
    }
  }

  fprintf(stderr, "_ll_sig_handler(SIG%s)\n", s->name);

  /* Append to tail of queue. */
  _ll_sig_disabled ++; /* Avoid reentry. */

  _ll_sig_queue[_ll_sig_queue_tail] = s;
  _ll_sig_queue_tail = (_ll_sig_queue_tail + 1) % _ll_sig_queue_len;
  _ll_sig_pending ++;

  _ll_sig_disabled --;

  /* Check for queue overrun. */
  ll_assert_msg(sig,_ll_sig_queue_tail !=  _ll_sig_queue_head, ("signal queue overrun (length %d)", _ll_sig_queue_len));
}


void _ll_sig_service()
{
  if ( _ll_sig_disabled ) 
    return;

  while ( _ll_sig_queue_head != _ll_sig_queue_tail ) {
    sigdef *s;

    /* Remove from head of queue */
    _ll_sig_disabled ++; /* Avoid signal during queue processing */

    s = (sigdef *) _ll_sig_queue[_ll_sig_queue_head];
    _ll_sig_queue[_ll_sig_queue_head] = 0;
    _ll_sig_queue_head = (_ll_sig_queue_head + 1) % _ll_sig_queue_len;
    _ll_sig_pending --;

    _ll_sig_disabled --;

    /* Apply the signal handler proc */
    if ( s ) {
      fprintf(stderr, "\n_ll_sig_service(): SIG%s\n", s->name);
      ll_call(ll_sig_proc(s), _1(ll_make_fixnum(s->sig)));
    }
  }
}


/****************************************************************************/


ll_define_primitive(fixnum, llCsignalCget, 
		    _1(sig), 
		    _1(doc, "Gets the procedure that receives signal."))
{
  sigdef *s = sigdef_by_sig(ll_UNBOX_fixnum(ll_SELF));
  ll_return(s ? ll_sig_proc(s) : ll_f);
}
ll_define_primitive_end


ll_define_primitive(string, llCsignalCget, 
		    _1(sig), 
		    _1(doc, "Gets the procedure that receives the signal."))
{
  sigdef *s = sigdef_by_name(ll_THIS->_array);
  ll_return(s ? ll_sig_proc(s) : ll_f);
}
ll_define_primitive_end


ll_define_primitive(symbol, llCsignalCget, 
		    _1(sig), 
		    _1(doc, "Gets the procedure that receives the signal."))
{
  ll_SELF = ll_THIS->_name;
  __ll_call_tailv(ll_o(llCsignalCget), 1);
}
ll_define_primitive_end


/****************************************************************************/

static
ll_v _ll_signal_setE(sigdef *s, ll_v proc)
{
  ll_v prev;

  if ( s ) {
    void *handler;

    prev = ll_sig_proc(s);
    ll_sig_proc(s) = ll_ARG_1;
    if ( ll_unbox_boolean(ll_ARG_1) ) {
      handler = _ll_sig_handler;
    } else {
      handler = SIG_DFL;
    }
    signal(s->sig, handler);
  } else {
    prev = ll_f;
  }

  return prev;
}


ll_define_primitive(fixnum, llCsignalCsetE, 
		    _2(sig, proc), 
		    _1(doc, "Sets handler for signal."))
{
  ll_return(_ll_signal_setE(sigdef_by_sig(ll_UNBOX_fixnum(ll_SELF)), ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(string, llCsignalCsetE, 
		    _2(sig, proc), 
		    _1(doc, "Sets handler for signal."))
{
  ll_return(_ll_signal_setE(sigdef_by_name(ll_THIS->_array), ll_ARG_1));
}
ll_define_primitive_end


ll_define_primitive(symbol, llCsignalCsetE, 
		    _2(sig, proc), 
		    _1(doc, "Sets handler for signal."))
{
  ll_SELF = ll_THIS->_name;
  __ll_call_tailv(ll_o(llCsignalCsetE), 2);
}
ll_define_primitive_end


/****************************************************************************/


ll_define_primitive(object,llCsignalCerror, 
		    _1(sig), 
		    _1(doc, "Default signal handler; raises a signal error."))
{
  ll_return(_ll_error(ll_ee(signal), 1,
		     ll_s(signal), ll_SELF));
}
ll_define_primitive_end


ll_INIT(sig,330,"initialize signal handlers")
{
  int sigs[] = {
    /* ANSI C: B9. Signals. */
#ifdef SIGABRT
    SIGABRT,
#endif
#ifdef SIGFPE
    SIGFPE,
#endif
#ifdef SIGINT
    SIGINT,
#endif
#ifdef SIGSEGV
    SIGSEGV,
#endif
#ifdef SIGTERM
    SIGTERM,
#endif
    -1
  };
  int i;

  for ( i = 0; sigs[i] >= 0; i ++ ) {
    ll_call(ll_o(llCsignalCsetE), _2(ll_BOX_fixnum(sigs[i]), ll_o(llCsignalCerror)));
  }

  return 0;
}

/****************************************************************************/

#ifndef _ll_sig_h
#define _ll_sig_h

#ifndef __rcs_id__
#ifndef __rcs_id_ll_sig_h__
#define __rcs_id_ll_sig_h__
static const char __rcs_id_ll_sig_h[] = "$Id: sig.h,v 1.3 2001/01/15 03:56:38 stephens Exp $";
#endif
#endif /* __rcs_id__ */

void _ll_sig_service();

extern volatile int _ll_sig_pending;
#define ll_sig_service() ( _ll_sig_pending ? _ll_sig_service() : (void) 0 )

#endif

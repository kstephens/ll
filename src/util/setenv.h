#ifndef _util_setenv_h
#define _util_setenv_h

#ifndef __rcs_id__
#ifndef __rcs_id_util_setenv_h__
#define __rcs_id_util_setenv_h__
static const char __rcs_id_util_setenv_h[] = "$Id: setenv.h,v 1.3 1999/04/05 19:04:22 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#ifndef setenv
#define setenv ion_setenv
#endif

void setenv (const char *name, const char *value);

#endif


#ifndef _kslib_ccont_h
#define _kslib_ccont_h

#include <setjmp.h>	/* setjmp(), jmp_buf */

/*
** A C continuation
*/
struct CCont_s; /* opaque */
typedef struct CCont_s CCont;

/* Initialize the continuation system */
void CCont_init(char ***argvp);

/* Begin a continuation block */
extern int CCont_INIT(CCont **);
#define CCont_begin(_CC_) {\
  CCont *_CC_ = 0;\
  CCont_INIT(&_CC_);\
  if ( ! setjmp(*(jmp_buf*)_CC_) ) {

/* The call code block */
#define CCont_called() } else {

/* End a continuation block */
extern void CCont_END(void);
#define CCont_end() }\
  CCont_END();\
}

/* Save a continuation for re-entry */
void CCont_ASSIGN(CCont **x, CCont *y, int zero);
#define CCont_assign(x,y) CCont_ASSIGN(&(x),y,0)

/* Free a continuation for re-entry */
void CCont_FREE(CCont **x, int zero);
#define CCont_free(x) CCont_FREE(&(x),0)

/* Call a continuation */
void CCont_call(CCont *c, void *data);

/* Get the args to the call */
void *CCont_args(CCont *c);

/* Associate user-data with a continuation */
void *CCont_data(CCont *c);
void  CCont_setData(CCont *c, void *data);

#endif

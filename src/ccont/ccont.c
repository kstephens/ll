#ifndef __rcs_id__
#ifndef __rcs_id_ccont_ccont_c__
#define __rcs_id_ccont_ccont_c__
static const char __rcs_id_ccont_ccont_c[] = "$Id: ccont.c,v 1.7 2007/09/17 16:10:19 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "ccont.h"
#include <setjmp.h> /* jmp_buf, setjmp() */
#include <stdlib.h> /* malloc(), free() */
#include <string.h> /* memcpy() */
#include <assert.h>


#define stackTop stack
#define stackBottom parent->stack - 1

/* Stack grows down */
#define stackBegin stackTop
#define stackEnd stackBottom

/* Allocation */
#ifndef CCONT_MALLOC
#define CCONT_MALLOC malloc
#endif
void *(*_CCont_malloc)(size_t) = CCONT_MALLOC;

#ifndef CCONT_FREE
#define CCONT_FREE free
#endif
void (*_CCont_free)(void*) = CCONT_FREE;


/* Wrapper for struct assignment */
typedef struct {
  jmp_buf _;
} __jmpbuf;


struct CCont_s {
  jmp_buf jb;
  /* jb contains the environment for the CCont_begin() block */

  struct CCont_s *parent;
  /*
  ** parent points to the parent continuation of this continuation
  ** (all continuations have a parent, except the root continuation
  ** which is a bootstrap and never seen by the user.)
  */

  unsigned int refCount[2];
  /*
  ** refCount[0] is the number of references to the continuation by user
  ** code.
  ** refCount[1] is the number of references to the continuation by the system
  ** or other continuations.
  ** This is decremented by CCont_free()
  ** which frees stackCopy and the continuation object when both reach 0.
  */

  char *stack;
  /*
  ** The top of the stack (SP) at CCont_begin().
  */

  unsigned long stackSize;
  /*
  ** The size of the stackCopy.
  */

  void *stackCopy;
  /*
  ** The copy of the stack range.
  */

  void *args;
  /*
  ** Call data.
  */

  void *data;
  /*
  ** User data
  */
};


/*
** THE CURRENT CONTINUATION.
** We're fucked if this thing has multiple threads!!!
*/
static CCont *cc = 0;

void CCont_init(char ***argvp)
{
  static CCont c;

  if ( cc )
    return;

  c.refCount[0] =
  c.refCount[1] = 2; /* Never free */
  c.stackTop = (void *) argvp;

  cc = &c;
}


void *CCont_args(CCont *c)
{
  assert(c);
  return c->args;
} 


void *CCont_data(CCont *c)
{
  assert(c);
  return c->data;
}
void  CCont_setData(CCont *c, void *data)
{
  assert(c);
  c->data = data;
}


/*
** assign() and free() modify the system refCount in this file.
*/
#undef CCont_assign
#define CCont_assign(x,y) CCont_ASSIGN(&(x),y,1)
#undef CCont_free
#define CCont_free(x)  CCont_FREE(&(x),1)

#define c (*_c)


int CCont_INIT(CCont **_c)
{
#define SP (char*)&_c

  assert(_c);
  assert(cc);
  c = _CCont_malloc(sizeof(*c));
  assert(c);

  c->refCount[0] =
  c->refCount[1] = 0;

  c->parent = 0;
  CCont_assign(c->parent,cc);

  c->stackTop = SP;
  c->stackSize = c->stackEnd - c->stackBegin;

  c->stackCopy = 0;

  c->args = 0;
  c->data = 0;

  CCont_assign(cc,c);

#undef SP
  return 0 /* setjmp(cc->jb) */;
}


void CCont_FREE(CCont **_c, int i)
{
  assert(_c);
  if ( c == 0 )
    return;

  c->refCount[i] --;

  if ( c->refCount[0] == 0 && c->refCount[1] == 0 ) {
    CCont_FREE(&(c->parent), 1);

    if ( c->stackCopy ) {
      _CCont_free(c->stackCopy);
      c->stackCopy = 0;
    }

    _CCont_free(c);
  }

  c = 0;
}


void CCont_ASSIGN(CCont **_c, CCont *y, int i)
{
  assert(_c);
  if ( c != y ) {
    if ( y )
      y->refCount[i] ++;
    if ( c )
      CCont_FREE(_c, i);
    c = y;
  }
}


#undef c


static
void CCont_jmp(CCont *c)
{
  /*
  ** At this point the jump point is below the current SP.
  */
  assert(c->stackTop >= (char*)&c);

  /*
  ** Use copy on jmpbuf because longjmp() (may) modify it.
  */
  {
    __jmpbuf jbcopy;

    jbcopy = *(__jmpbuf*)&(c->jb);

    /*
    ** Set current continuation to new continuation
    */
    CCont_assign(cc, c);
    
    longjmp(jbcopy._, 1);
  }
}

/*
** How to wind the C stack in a (mostly) portable fashion.
** Keep calling recursively to make the stack grow
** until enough room is available for
** the stack for continuation c.
** CCont_wind(c,j) is the entry point.
**
** Implementation Notes:
** 1. Grow the stack in largest powers of two.
** 2. Pass the fill array to the recursion function so the compiler
** cannot (hopefully) optimize it away as an unreferenced variable.
*/
static void CCont_wind(CCont *c, CCont *j, void *dummy);

#define WIND(x) \
static void CCont_wind_##x(CCont *c, CCont *j) { \
  auto char fill[x]; \
  CCont_wind(c, j, fill); \
}
#include "wind.h"

static void CCont_wind(CCont *c, CCont *j, void *dummy)
{
#define SP (char*)&c

  if ( SP <= c->stackTop ) {
    /*
    ** Restore the environment from the join point to the new continuation.
    */
    {
      CCont *x;

      for ( x = c; x != j; x = x->parent ) {
	assert(x->stackCopy);
	memcpy(x->stackBegin, x->stackCopy, x->stackSize);
      }
    }

    /*
    ** Jump back to c
    */
    CCont_jmp(c);
  } else {
    unsigned long sdelta = c->stackTop - SP;
#define WIND1(x) if ( sdelta >= x ) { CCont_wind_##x(c, j); }
#define WIND(x) else if ( sdelta & x ) { CCont_wind_##x(c, j); }
#include "wind.h"
  }
#undef SP
}

#define CCont_wind(c,j) CCont_wind(c,j,0)

static
CCont *findJoinPoint(CCont *a, CCont *b)
{
  CCont *c1, *c2;

  for ( c1 = a; c1; c1 = c1->parent )
  for ( c2 = b; c2; c2 = c2->parent )
    if ( c2 == c1 )
      return c2;

  abort();
  return 0;
}


void CCont_END(void)
{
  assert(cc);

  /*
  ** A continuation is returning.
  ** If the user saved a reference to it (for future reentry)
  ** save the stack block.
  */
  if ( cc->refCount[0] ) {
    if ( ! cc->stackCopy ) {
      cc->stackCopy = _CCont_malloc(cc->stackSize);
      assert(cc->stackCopy);
    }
    memcpy(cc->stackCopy, cc->stackBegin, cc->stackSize);
  }

  /*
  ** Restore the previous continuation as the current continuation.
  */
  CCont_assign(cc, cc->parent);

  assert(cc);
}


void CCont_call(CCont *c, void *args)
{
  CCont *j;

  assert(c);
  assert(cc);
  c->args = args;
  j = findJoinPoint(cc, c);

  /*
  ** if unwinding
  ** Save all stack blocks from current continuation to the join point.
  ** Optimization Note:
  ** Only save stack blocks below a continuation that may be jumped back into.
  ** Only a continuation with more than one user reference may be
  ** jumped back into.
  */
  if ( j != cc ) {
    CCont *x;
    int save = 0;

    for ( x = cc; x != j; x = x->parent ) {
      if ( x->refCount[0] ) {
        save = 1;
      }
      if ( save ) {
        if ( ! x->stackCopy ) {
          x->stackCopy = _CCont_malloc(x->stackSize);
          assert(x->stackCopy);
        }
        memcpy(x->stackCopy, x->stackBegin, x->stackSize);
      }
    }
  }

  /*
  ** If only unwinding, simply jump back up the stack.
  */
  if ( j == c ) {
    CCont_jmp(c);
  }
  /*
  ** If winding, wind the stack, restore the stack and jump.
  */
  else {
    CCont_wind(c, j);
  }
}


#ifdef UNIT_TEST

/* Test cases */
#include <stdio.h>

CCont *c = 0;

void a(int x)
{
  static int n = 0;

  CCont_begin(cc); 
  {
    printf("started %p\n", cc);
    n = 0;
    CCont_assign(c, cc);
  } CCont_called() {
    printf("called  %p=%p\n", cc, c);
    if ( n == 5 )
      CCont_free(c);
  } 
  CCont_end();

  printf("exiting %p n=%d, x=%d\n", c, n ++, x);
}

int main(int argc, char **argv)
{
  CCont_init(&argv);

  printf("\n\nmain()\n");

  {
  int x;

  for ( x = 0; x < 10; x ++ ) {
    a(x);

    while ( c )
      CCont_call(c, 0);
  }
  }
  return 0;
}

#endif /* UNIT_TEST */



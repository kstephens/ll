#include "refcntptr.hh"

#ifdef UNIT_TEST

#include <stdio.h>

class Cons;

inline void __ref(Cons *t);
inline void __deref(Cons *t);

typedef refcntptr<Cons> ref;

#define Ref const ref &

class Cons {
private:
  friend void __ref(Cons *t);
  friend void __deref(Cons *t);
  unsigned long _refcnt;
  void __ref() {
#ifdef _refcntptr_DEBUG
    fprintf(stderr,"(Cons*)%p->__ref()\n", this);
#endif
    ++ _refcnt;
  }
  void __deref() {
#ifdef _refcntptr_DEBUG
    fprintf(stderr,"(Cons*)%p->__deref()\n", this);
#endif
    if ( -- _refcnt <= 0 ) {
      delete this;
    }
  }

  ref _car;
  ref _cdr;
public:
  Cons() : _refcnt(0) { };
  Cons(const ref &car, const ref &cdr) : _refcnt(0), _car(car), _cdr(cdr) { };
  ~Cons() { 
#ifdef _refcntptr_DEBUG
    fprintf(stderr, "delete (Cons*)%p\n");
#endif
}

  void _print(Ref x)
  {
    if ( x ) {
      x->print();
    } else {
      fprintf(stderr, "nil");
    }
  }
  void print()
  {
    fprintf(stderr, "(");
    _print(_car);
    fprintf(stderr, " . ");
    _print(_cdr);
    fprintf(stderr, ")");
  }

};
inline void __ref(Cons *t) { if ( t ) t->__ref(); }
inline void __deref(Cons *t) { if ( t ) t->__deref(); }



int main(int argc, char **argv)
{
  ref x;
  ref y = new Cons();
  ref z = new Cons(x, y);

  z->print();

  return 0;
}

#endif


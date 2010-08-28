#ifndef _util_REFCNTPTR_HH
#define _util_REFCNTPTR_HH


#ifndef refcntptr_DEBUG
#define refcntptr_DEBUG 0
#endif


#if refcntptr_DEBUG
#define refcntptr_DEBUG_PRINT(X) printf X
#else
#define refcntptr_DEBUG_PRINT(X)(void)0
#endif


#include <assert.h> /* assert(_refcnt) */


/* Class with refcnt support. */
#define refcntptr_STRINGTIZE_(T)#T
#define refcntptr_STRINGTIZE(T)refcntptr_STRINGTIZE_(T)
#define refcntptr_(T)T
#define refcntptr_PASTE_(A,B)A##B
#define refcntptr_PASTE(A,B)refcntptr_PASTE_(A,B)

/* Declare ref count manipulation glue. */
#define refcntptr_DECL(T) \
  void __ref(refcntptr_(T)*); \
  void __deref(refcntptr_(T)*); \
  extern "C" refcntptr_(T)* refcntptr_PASTE(T,_ref)(refcntptr_(T)* __p); \
  extern "C" void refcntptr_PASTE(T,_deref)(refcntptr_(T)* __p);


/* Defines support for refcntptr<T> in T. */
#define refcntptr_CLASS(T) \
  private: \
  unsigned long _refcnt; \
  public: \
  virtual void __deref_delete(void); \
  void __ref(void) { ++ _refcnt; assert(_refcnt); } \
  void __deref(void) \
  { \
    if ( _refcnt <= 1 ) { \
      refcntptr_DEBUG_PRINT(("(%s*)%p delete\n",  refcntptr_STRINGTIZE(T), this)); \
      this->__deref_delete(); \
    } else { \
      -- _refcnt; \
    } \
  }


/* Defines ref count manipulation glue. */
#define refcntptr_DEF(SCOPE,T) \
  SCOPE void __ref(refcntptr_(T)* __p) { if ( __p ) __p->__ref(); } \
  SCOPE void __deref(refcntptr_(T)* __p) { if ( __p ) __p->__deref(); } \
  refcntptr_(T)* refcntptr_PASTE(T,_ref)(refcntptr_(T)* __p) { __ref(__p); return __p; } \
  void refcntptr_PASTE(T,_deref)(refcntptr_(T)* __p) { __deref(__p); }


template<class T> class refcntptr {
private:
  T *_p;

public:
  refcntptr () : _p(0) {  };
  refcntptr (T* ptr) { __ref(_p = ptr); }
  refcntptr (const refcntptr &x) { __ref(_p = x._p); }
  ~refcntptr () { __deref(_p); }
  
  refcntptr& operator = (T* p) { __ref(p); __deref(_p); _p = p; return *this; }
  refcntptr& operator = (const refcntptr& x) { return (*this) = x._p; }

  operator void* () const { return (void*) _p; }
  operator int () const { return _p != 0; }
  operator T* () const { return _p; }
  int operator ! () const { return _p == 0; }

  T& operator *() const { return *_p; }
  T* operator ->() const { return _p; }

  int operator == (T* ptr) const { return _p == ptr; }
  int operator == (const refcntptr &x) const { return *this == x._p; }

  int operator != (T* ptr) const { return _p != ptr; }
  int operator != (const refcntptr &x) const { return *this != x._p; }

  int operator < (T* ptr) { return _p < ptr; }
  int operator < (const refcntptr &x) const { return *this < x._p; }
};


#endif

#include "bitfield.h"

#define static

#define GET(SIZE,OFFSET) \
static \
int _u_get_##SIZE##_##OFFSET(void *ptr, void *r) { \
  struct { unsigned int offset : OFFSET; unsigned int x : SIZE; } *q = ptr; \
  * (unsigned int *) r = q->x; return 0; \
} \
static \
int _i_get_##SIZE##_##OFFSET(void *ptr, void *r) { \
  struct { int offset : OFFSET; int x : SIZE; } *q = ptr; \
  * (int *) r = q->x; return 0; \
}

#define SET(SIZE,OFFSET) \
static \
int _u_set_##SIZE##_##OFFSET(void *ptr, void *v) { \
  struct { unsigned int offset : OFFSET; unsigned int x : SIZE; } *q = ptr; \
  q->x = * (unsigned int *) v; return 0; \
} \
static \
int _i_set_##SIZE##_##OFFSET(void *ptr, void *v) { \
  struct { int offset : OFFSET; int x : SIZE; } *q = ptr; \
  q->x = * (int *) v; return 0; \
}


/* 0 offset functions */
#define GET0(SIZE) \
static \
int  _u_get_##SIZE##_0(void *ptr, void *r) { \
  struct { unsigned int x : SIZE; } *q = ptr; \
  * (unsigned int *) r = q->x; return 0; \
} \
static \
int _i_get_##SIZE##_0(void *ptr, void *r) { \
  struct { int x : SIZE; } *q = ptr; \
  * (int *) r = q->x; return 0; \
}

#define SET0(SIZE) \
static \
int _u_set_##SIZE##_0(void *ptr, void *v) { \
  struct { unsigned int x : SIZE; } *q = ptr; \
  q->x = * (unsigned int *) v; return 0; \
} \
static \
int _i_set_##SIZE##_0(void *ptr, void *v) { \
  struct { int x : SIZE; } *q = ptr; \
  q->x = * (int *) v; return 0; \
}

#define F0(S) GET0(S) SET0(S)
#define F(S,O) GET(S,O) SET(S,O)

#define G(S) \
F0(S)   F(S, 1) F(S, 2) F(S, 3) \
F(S, 4) F(S, 5) F(S, 6) F(S, 7) \
F(S, 8) F(S, 9) F(S,10) F(S,11) \
F(S,12) F(S,13) F(S,14) F(S,15) \
F(S,16) F(S,17) F(S,18) F(S,19) \
F(S,20) F(S,21) F(S,22) F(S,23) \
F(S,24) F(S,25) F(S,26) F(S,27) \
F(S,28) F(S,29) F(S,30) F(S,31)

#define DO_G \
      G( 1) G( 2) G( 3) \
G( 4) G( 5) G( 6) G( 7) \
G( 8) G( 9) G(10) G(11) \
G(12) G(13) G(14) G(15) \
G(16) G(17) G(18) G(19) \
G(20) G(21) G(22) G(23) \
G(24) G(25) G(26) G(27) \
G(28) G(29) G(30) G(31)

DO_G

#undef F0
#undef F

_ct_bf_op _ct_bf_setter[32][32][2];
_ct_bf_op _ct_bf_getter[32][32][2];

void _ct_init_bitfield_accessors() {

#define F(S,O) \
  _ct_bf_getter[0][S][0] = _u_get_##S##_##O; \
  _ct_bf_getter[0][S][1] = _i_get_##S##_##O; \
  _ct_bf_setter[0][S][0] = _u_set_##S##_##O; \
  _ct_bf_setter[0][S][1] = _i_set_##S##_##O;

#define F0(S) F(S,0)

DO_G

}

#undef F
#undef F0

#undef DO_G

#undef G
#undef F
#undef GET
#undef SET
#undef GET0
#undef SET0


#ifndef _ct_bitfield_h
#define _ct_bitfield_h

typedef int (*_ct_bf_op)(void *s, void *v);

extern _ct_bf_op _ct_bf_setter[32][32][2];
extern _ct_bf_op _ct_bf_getter[32][32][2];

void _ct_init_bitfield_accessors();

#endif

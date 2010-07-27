
#define __ll_VEC_NAME(VEC,NAME)ll_##NAME##_##VEC
#define _ll_VEC_NAME(VEC,NAME)__ll_VEC_NAME(VEC,NAME)

ll_v _ll_VEC_NAME(_ll_VEC,make)(_ll_VEC_ELEM_TYPE *v, size_t s);
_ll_VEC_ELEM_TYPE *_ll_VEC_NAME(_ll_VEC,ptr)(ll_v x);

size_t _ll_VEC_NAME(_ll_VEC,len)(ll_v x);
void _ll_VEC_NAME(_ll_VEC,set_len)(ll_v x, size_t size);

ll_v _ll_VEC_NAME(_ll_VEC,make_copy)(const _ll_VEC_ELEM_TYPE *v, size_t s);

#undef _ll_VEC
#undef _ll_VEC_ELEM_TYPE
#undef _ll_VEC_NAME
#undef __ll_VEC_NAME
#undef _ll_VEC_ELEM_TYPE




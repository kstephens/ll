/* $Id: cops.h,v 1.2 1999/12/04 01:40:27 stephensk Exp $ */
#ifndef ROP
#define ROP(X,Y)BOP(X,Y)
#endif
#ifndef UOP
#define UOP(X,Y)BOP(X,Y)
#endif

#ifdef INT_OPS
UOP(bnot,~)
BOP(bor,|)
BOP(band,&)
BOP(bxor,^)
#undef INT_OPS
#else
BOP(ADD,+)
BOP(SUB,-)
BOP(MUL,*)
#ifndef DIV_SKIP
BOP(DIV,/)
#endif
UOP(NEG,-)

ROP(EQ,==)
ROP(NE,!=)
ROP(LT,<)
ROP(GT,>)
ROP(LE,<=)
ROP(GE,>=)
#endif

#undef BOP
#undef UOP
#undef ROP

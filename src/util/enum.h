/* Copyright (c) 1998, 1999 Kurt A. Stephens and Ion, Inc. */

#ifndef _util_enum_h
#define _util_enum_h

#ifndef __rcs_id__
#ifndef __rcs_id_util_enum_h__
#define __rcs_id_util_enum_h__
static const char __rcs_id_util_enum_h[] = "$Id: enum.h,v 1.7 2000/01/13 13:58:11 stephensk Exp $";
#endif
#endif /* __rcs_id__ */


#include <stddef.h> /* size_t */

typedef struct C_enum {
  const char *name;
  long value;
} C_enum;

int C_enum_str_to_value(const C_enum *table, const char *name, size_t name_len, long *result);
int C_enum_str_to_or_value(const C_enum *table, const char *name, size_t name_len, long *resultp);

const char * C_enum_value_to_str(const C_enum *table, long value);
const char * C_enum_or_value_to_str(const C_enum *table, long value, char *buf, size_t buf_len);

#endif

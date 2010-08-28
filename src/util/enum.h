/* Copyright (c) 1998, 1999 Kurt A. Stephens and Ion, Inc. */

#ifndef _util_enum_h
#define _util_enum_h

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

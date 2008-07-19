#ifndef _ct_ct_sym_h
#define _ct_ct_sym_h

#include "ct.h"

typedef struct ct_sym {
  void *addr;
  const char *name;
  int is_data;
  ct_t type;
  const char *file;
  int line;
  char *buf;
} ct_sym;

ct_sym *ct_sym_for_address(void *addr);
ct_sym *ct_sym_for_name(const char *name);

int ct_sym_load_for_exe(const char *argv0);

#endif

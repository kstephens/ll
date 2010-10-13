#include <stdio.h>
#include <a.out.h>
#include <string.h>
#include <stdlib.h>
#include <sys/param.h> /* MAXPATHLEN */
#include <bfd.h>

int ct_stab_parse(const char *ofile, int option, ...)
{
  int rtn_code = 0;
  bfd *bfd;

#define return(X) do{rtn_code = (X); goto rtn;}while(0)

  if ( ! (bfd = bfd_openr(ofile, "")) ) {
    fprintf(stderr, "ct_stab_parse: cannot open '%s'\n", ofile);
    return(-1);
    
  }

#define Pm(x) fprintf(stderr, "%s\n", (char *) (x));
#define Pu(x) fprintf(stderr, "%s = %lu\n", #x, (unsigned long) (x))
#define Px(x) fprintf(stderr, "%s = %08lx\n", #x, (unsigned long) (x))
#define Ps(x) fprintf(stderr, "%s = \"%s\"\n", #x, (char *) (x))
#define Pss(x,s) fprintf(stderr, "%s = \"%*s\"\n", #x, (int) (s), (char *) (x))

  {
    long storage_needed;
    asymbol **symbol_table;
    long number_of_symbols;
    long i;
     
    storage_needed = bfd_get_symtab_upper_bound (bfd);
    
    if (storage_needed < 0) {
      fprintf(stderr, "ct_stab_parse: cannot get symbol table size\n");
      return(-1);
    }
     
    if (storage_needed == 0) {
      return(-1);
    }
    symbol_table = (asymbol **) malloc (storage_needed);

    number_of_symbols = bfd_canonicalize_symtab (bfd, symbol_table);
     
    if (number_of_symbols < 0) {
      fprintf(stderr, "ct_stab_parse: cannot get symbol table\n");
      return(-1);
    }
    for (i = 0; i < number_of_symbols; i++) {
      asymbol *s = symbol_table[i];
      Pm("");
      Ps(s->name);
      Px(s->value);
      Px(s->flags);
      Ps(s->section->name);
    }

    free(symbol_table);
  }

 rtn:
  bfd_close(bfd);

  return rtn_code;
}

int main(int argc, char **argv)
{
  fprintf(stderr, "argv[0] = '%s'\n", argv[0]);
  ct_stab_parse(argv[0], 0);
  return 0;
}

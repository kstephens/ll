
#ifndef __rcs_id__
#ifndef __rcs_id_ct_stabraw_c__
#define __rcs_id_ct_stabraw_c__
static const char __rcs_id_ct_stabraw_c[] = "$Id: stabraw.c,v 1.2 1999/02/19 09:25:28 stephensk Exp $";
#endif
#endif /* __rcs_id__ */


#include <stdio.h>
#include <a.out.h>
#include <string.h>
#include <sys/param.h> /* MAXPATHLEN */

int ct_stab_parse(const char *ofile, int option, ...)
{
  FILE *fp;

  if ( ! (fp = fopen(ofile, "rb")) ) {
#ifdef __CYGWIN32__
    const char *suffixes[] = { ".exe", ".com", 0 };
    int i;

    for ( i = 0; suffixes[i]; i ++ ) {
      char op[MAXPATHLEN + 1];

      strcpy(op, ofile);
      strcat(op, suffixes[i]);

      if ( (fp = fopen(op, "rb")) )
	break;
    }
    if ( ! suffixes[i] ) 
#endif
    {
      fprintf(stderr, "ct_stab_parse: cannot open '%s'\n", ofile);
      return -1;
    }
  }


#define Pm(x) fprintf(stderr, "%s\n", (char *) (x));
#define Pu(x) fprintf(stderr, "%s = %lu\n", #x, (unsigned long) (x))
#define Px(x) fprintf(stderr, "%s = %08lx\n", #x, (unsigned long) (x))
#define Ps(x) fprintf(stderr, "%s = \"%s\"\n", #x, (char *) (x))
#define Pss(x,s) fprintf(stderr, "%s = \"%*s\"\n", #x, (int) (s), (char *) (x))

#if 0 
  {
    struct external_filehdr fh;

    fseek(fp, 0, 0);
    fread(&fh, sizeof(fh), 1, fp);

    Pm("external_filehdr");
    Px(fh.f_magic);
    Px(fh.f_nscns);
    Px(fh.f_timdat);
    Px(fh.f_symptr);
    Px(fh.f_nsyms);
    Px(fh.f_opthdr);
    Px(fh.f_flags);

  }

  {
    AOUTHDR fh;

    fseek(fp, 0, 0);
    fread(&fh, sizeof(fh), 1, fp);

    Pm("AOUTHDR");
    Px(fh.magic);
    Px(fh.vstamp);
    Px(fh.tsize);
    Px(fh.dsize);
    Px(fh.bsize);
    Px(fh.entry);
    Px(fh.text_start);
    Px(fh.data_start);
  }
#endif
  
  {
    FILHDR fh;

    fseek(fp, 0, 0);
    fread(&fh, sizeof(fh), 1, fp);

    Pm("FILHDR");
    Px(fh.e_magic);
    Px(fh.e_cblp);
    Px(fh.e_cp);
    Px(fh.e_crlc);
    Px(fh.e_cparhdr);
    Px(fh.e_minalloc);
    Px(fh.e_maxalloc);
    Px(fh.e_ss);
    Px(fh.e_sp);
    Px(fh.e_csum);
    Px(fh.e_ip);
    Px(fh.e_cs);
    Px(fh.e_lfarlc);
    Px(fh.e_ovno);
    /* fh.e_res[4][2] */
    Px(fh.e_oemid);
    Px(fh.e_oeminfo);
    /* fh.e_res2[10][2] */
    Px(fh.e_lfanew);
    /* fh.dos_message[16][4] */
    Px(fh.nt_signature);

    Pm("From standard header");
    Px(fh.f_magic);
    Px(fh.f_nscns);
    Px(fh.f_timdat);
    Px(fh.f_symptr);
    Px(fh.f_nsyms);
    Px(fh.f_opthdr);
    Px(fh.f_flags);
    
    //   exit (1);

    fseek(fp, fh.f_opthdr, SEEK_CUR);

    {
      int i;

      Pm("Sections");
      for ( i = 0; i < fh.f_nscns; i ++ ) {
	SCNHDR sh;

	fread(&sh, sizeof(sh), 1, fp);
	Pm("");
	Pss(sh.s_name, sizeof(sh.s_name));
	Px(sh.s_paddr);
	Px(sh.s_vaddr);
	Pu(sh.s_size);
#if 0
	Px(sh.s_scnptr);
	Px(sh.s_relptr);
	Pu(sh.s_nreloc);
	Pu(sh.s_nlnno);
	Px(sh.s_flags);
#endif

      }
    }

    if ( fh.f_opthdr == sizeof(AOUTHDR) ) {
      AOUTHDR aoh;

      Pm("AOUTHDR");
      fread(&aoh, sizeof(aoh), 1, fp);
      Px(aoh.magic);
      Px(aoh.vstamp);
      Px(aoh.tsize);
      Px(aoh.dsize);
      Px(aoh.bsize);
      Px(aoh.entry);
      Px(aoh.text_start);
      Px(aoh.data_start);
    }

    exit(1);

    {
      int i;
      
      fseek(fp, fh.f_symptr, 0);
    
      Pm("Reading symbols");
      for ( i = 0; i < fh.f_nsyms; i ++ ) {
	SYMENT se;

	fread(&se, sizeof(se), 1, fp);
	// Ps(se.e.e_name);
	if ( se.e.e.e_zeroes ) {
	  Ps(se.e.e_name);
	} else {
	  Px(se.e.e.e_zeroes);
	  Px(se.e.e.e_offset);
	}
	Px(se.e_value);
	Px(se.e_scnum);
	Px(se.e_type);
	Px(se.e_sclass[0]);
	Px(se.e_numaux[1]);
      }
    }
  }
  
  fclose(fp);
  return 0;
}

int main(int argc, char **argv)
{
  fprintf(stderr, "argv[0] = '%s'\n", argv[0]);
  ct_stab_parse(argv[0], 0);
  return 0;
}

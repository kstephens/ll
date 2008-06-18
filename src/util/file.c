#ifndef __rcs_id__
#ifndef __rcs_id_util_file_c__
#define __rcs_id_util_file_c__
static const char __rcs_id_util_file_c[] = "$Id: file.c,v 1.6 2003/02/21 21:18:42 stephens Exp $";
#endif
#endif /* __rcs_id__ */

#include "util/file.h"
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include "util/path.h"


static void write_error(const char *s)
{
   write(2, s, strlen(s));
}


static void *xmalloc(size_t s)
{
  void *ptr = malloc(s);
  if ( ! ptr ) {
    write_error("out of memory in ");
    write_error(__FILE__);
    write_error("\n");
    abort();
  }
  return ptr;
}


time_t file_mtime(const char *fname)
{
  struct stat s;
#if 0
  fprintf(stderr, "file_mtime(\"%s\")\n", fname);
#endif
  return stat(fname, &s) == 0 ? (time_t) s.st_mtime : (time_t) 0;
}

int file_mode_masked(const char *fname, int euid, int egid)
{
  struct stat s;
  int mask;

  if ( euid < 0 )
    euid = geteuid();
  
  if ( egid < 0 )
    egid = getegid();
     
  if ( stat(fname, &s) )
    return 0;
  
  mask  = euid == s.st_uid ? S_IRWXU : 0;
  mask |= egid == s.st_gid ? S_IRWXG : 0;
  mask |= (euid != s.st_uid && egid != s.st_gid) ? S_IRWXO : 0;
  
  return s.st_mode & mask;
}


int file_executable(const char *fname)
{
#if defined(WIN32)
  {
    const char *suffix = path_suffix(fname);

    return suffix && 
      (strcasecmp(suffix, ".exe") == 0 || 
       strcasecmp(suffix, ".con") == 0 || 
       strcasecmp(suffix, ".bat") == 0 || 
       strcasecmp(suffix, ".dll") == 0);
  }

#else
  return file_mode_masked(fname, -1, -1) & (S_IXUSR|S_IXGRP|S_IXOTH);
#endif
}


static
int progname_try(char *buf)
{
#if 0
  fprintf(stderr, "progname_try(\"%s\")\n", buf);
#endif

#if defined(_WIN32) || defined(WINNT)
  /* Try .exe */
  strcat(buf, ".exe");
  if ( file_mtime(buf) ) {
    return 1;
  }

  /* Try .com */
  strcpy(strchr(buf, 0) - 4, ".com");
  if ( file_mtime(buf) ) {
    return 1;
  }

  /* Try .dll */
  strcpy(strchr(buf, 0) - 4, ".dll");
  if ( file_mtime(buf) ) {
    return 1;
  }

  /* Try .bat */
  strcpy(strchr(buf, 0) - 4, ".bat");
  if ( file_mtime(buf) ) {
    return 1;
  }

  *(strchr(buf, 0) - 4) = 0;
#else
  /* Try filename alone */
  if ( file_executable(buf) ) {
    return 1;
  }
#endif

  return 0;
}


int file_in_path(const char *argv0, const char *PATH, char *buf)
{
  char dir_sep[] = "/";

  if ( ! (argv0[0] == '/' || argv0[0] == '\\') ) {

    if ( (PATH || (PATH = getenv("PATH"))) && *PATH ) {
      const char *p, *pe, *pn;
      int path_sep = ':';
      
      if ( strchr(PATH, ';') )
	path_sep = ';';
      
      for ( p = PATH, pe = PATH; *p; p = pn) {
	if ( (pe = strchr(p, path_sep)) ) {
	  pn = pe + 1;
	} else {
	  pe = strchr(p, '\0');
	  pn = "";
	}
	
	if ( *p ) {
	  strncpy(buf, p, pe - p);
	  buf[pe - p] = 0;
	  strcat(buf, dir_sep);
	  strcat(buf, argv0);
	  
	  if ( progname_try(buf) )
	    return 1;
	}

#if 0
	fprintf(stderr, "pn = '%s'\n", pn);
#endif
      }
    }
  }
  
  strcpy(buf, argv0);
  if ( progname_try(buf) )
    return 1;

  strcpy(buf, argv0);
  return 0;
}


size_t file_size(const char *file)
{
  struct stat s;
  return stat(file, &s) ? 0 : s.st_size;
}


int file_touch(const char *file)
{
  int fd;

  if ( (fd = open(file, O_RDWR, 0644)) >= 0 ) {
    char buf[1];
    size_t bytes = sizeof(buf);

    buf[0] = 0;

    lseek(fd, 0, SEEK_SET);
    bytes = read(fd, buf, bytes);

    lseek(fd, 0, SEEK_SET);
    bytes = write(fd, buf, bytes);

    lseek(fd, 0, SEEK_END);

    close(fd);

    return 0;
  }

  return -1;
}


char *FILE_get_line_at_pos(FILE *fp, int linesize, pos_t pos, size_t *lenp, pos_t *line_pos)
{
  /*
  ** Read a buffer twice the expected maximum linesize to handle
  ** hopefully to get a beginning and ending newline bounding the
  ** file position (pos) wanted.
  */
  size_t bufsize = linesize * 2 + 3; /* The buffer read. */
  char *buf = xmalloc(bufsize + 1);   /* The size of the buffer read. */
  pos_t readpos;                     /* Where the read began. */

  do {
    /* Where to begin reading lines from. */
    if ( pos < linesize ) {
      /* Dont read before beginning of file! */
      readpos = 0;
    } else {
      readpos = pos - linesize;
    }
    
    /* Read a buffer and terminate it. */
    fseek(fp, readpos, SEEK_SET);
    bufsize = fread(buf, 1, bufsize, fp);
    buf[bufsize] = '\0'; /* Guard. */
    
    {
      /* Buffer begin/end */
      const char *bb = buf, *be = buf + bufsize;
      /* Position wanted within the buffer. */
      const char *pp = bb + (pos - readpos);
      /* Line begin/end */
      const char *lb = bb, *le;
      
      /* Find beginning of first line. */
      if ( readpos > 0 ) {
	while ( *lb != '\n' ) {
	  if ( lb == be ) {
	    /* Beginning of first line not found. */
	    goto bigger;
	  }
	  ++ lb;
	}
	/* Skip newline. */
	++ lb;
      }

      do {
	/* Find end of current line. */
	le = lb;
	while ( *le != '\n' ) {
	  if ( le == be ) {
	    /* End of line not found. */
	    goto bigger;
	  }
	  ++ le;
	}
	/* Skip newline. */
	++ le;

	/*
	** If found end of line and 
	** the position wanted is within the line found.
	*/
	if ( lb <= pp && pp < le ) {
	  /* Return beginning of line position. */
	  if ( line_pos )
	    *line_pos = readpos + (lb - bb);
      
	  /* 
	  ** Got the line.
	  ** Shrink buffer.
	  */
	  bufsize = le - lb;
	  memmove(buf, lb, bufsize);
	  buf = realloc(buf, bufsize + 1);
	  buf[bufsize] = '\0';

	  #if 0
	  fprintf(stderr, "line = '%s'\n", buf);
	  #endif

	  /* Return buffer and length. */
	  if ( lenp ) {
	    *lenp = bufsize;
	  }

	  return buf;
	}

	/* Try next line. */
	lb = le;
      } while ( lb != be );
   
    }

    /* Try again with a larger buffer. */
  bigger:
    bufsize *= 2;
    buf = realloc(buf, bufsize);
  } while ( 1 );
  
  /* Give up. */
  free(buf);
  
  return 0;
}


int FILE_print_line_at_pos(FILE *fp, int linesize, pos_t pos, pos_t *line_pos, FILE *out)
{
  int result = 0;
  size_t len;
  pos_t new_line_pos = line_pos ? *line_pos : -1;
  char *line = FILE_get_line_at_pos(fp, linesize, pos, &len, &new_line_pos);

  if ( ! line ) {
    result = -1;
  } else {
    /* Do not print any lines that we all ready printed. */
    if ( ! (line_pos && *line_pos == new_line_pos) ) {
      fprintf(out, "%lu\t", (unsigned long) new_line_pos);
      fwrite(line, 1, len, out);
    }
    free(line);
  }

  if ( line_pos )
    *line_pos = new_line_pos;

  return result;
}


int file_print_line_at_pos(const char *file, int linesize, pos_t pos, pos_t *line_pos, FILE *out)
{
  int result = 0;
  FILE *fp;

  if ( ! (fp = fopen(file, "r")) ) {
    result = -1;
  } else {
    result = FILE_print_line_at_pos(fp, linesize, pos, line_pos, out);
    fclose(fp);
  }

  return result;
}


static const char *_file = "<unknown>"; /* for debugging. */

#if 0
#define PDEBUG(X) fprintf X
#else
#define PDEBUG(X)
#endif


char *FILE_read_from_end(FILE *fp, size_t *buflenp, pos_t *posp)
{
  static const char _funcname[] = "FILE_read_from_end";
  char *buf;
  size_t buflen = 1024; /* Arbitrary bufsize. */
  long savepos;

  if ( buflenp && *buflenp ) {
    buflen = *buflenp;
  }

  buf = (char*) xmalloc(sizeof(buf[0]) * (buflen + 1));
  memset(buf, 0, sizeof(buf[0]) * (buflen + 1));

  /* Save current pos. */
  savepos = ftell(fp);

  /*
  ** If buflen is bigger than the file's size,
  **   read from beginning as much as possible.
  */
  if ( fseek(fp, - (long) buflen, SEEK_END) ) {
#if 0
    fprintf(stderr, "%s('%s'): fseek(%ld, SEEK_END) error.\n", _funcname, _file,
          - (long) buflen);
#endif
    clearerr(fp);
    fseek(fp, 0, SEEK_SET);
  }

  /* Remember where read started from. */
  if ( posp ) {
    *posp = (pos_t) ftell(fp);
  }

  /* Read and remember the actual size read. */
  buflen = fread(buf, sizeof(buf[0]), buflen, fp);

  /* Check for read error. */
  if ( buflen == 0 && ferror(fp) ) {
    fprintf(stderr, "%s('%s'): read error.\n", _funcname, _file);
    clearerr(fp);
    free(buf);
    buf = 0;
  }

  /* Restore fp pos. */
  fseek(fp, savepos, SEEK_SET);

  if ( buf ) {
    /* Realloc buf. */
    buf = realloc(buf, sizeof(buf[0]) * (buflen + 1));

    /* Null terminate. */
    buf[buflen] = '\0';
  }

#if 0
  fprintf(stderr, "%s('%s'): return '%s', buflen %ld.\n", _funcname, _file,
        buf,
        (long) buflen
        );
#endif

  if ( buflenp )
    *buflenp = buflen;

  return buf;
}


char *file_read_from_end(const char *file, size_t *buflenp, pos_t *posp)
{
  FILE *fp;
  char *buf;

  _file = file;

  if ( ! (fp = fopen(file, "r")) )
    return 0;

  buf = FILE_read_from_end(fp, buflenp, posp);

  fclose(fp);

  return buf;
}


char *FILE_get_line_at_end(FILE *fp, size_t *buflenp, pos_t *posp)
{
  char *buf = 0;
  size_t buflen = 1024;
  char *line = 0; /* The line found. */
  pos_t pos = 0; /* The position in the file the line was found. */
  char *b, *e; /* Search range in buffer. */
  static const char _funcname[] = "FILE_get_line_at_end";

  if ( buflenp && *buflenp ) {
    buflen = *buflenp;
  }

  do {
    if ( buf )
      free(buf);

    buf = FILE_read_from_end(fp, &buflen, &pos);
    if ( ! buf )
      break;

    /*
    ** If buflen is 0,
    **   the file is empty.
    */
    if ( buflen == 0 ) {
      PDEBUG((stderr, "%s('%s'): file is empty\n", _funcname, _file));
      line = 0;
      break;
    }

    /* Begin scanning backwards for a newline before last line. */
    e = buf + buflen; /* end of buffer (hence, end of possible line). */
    b = e - 1; /* beginning of possible line. */

    /* Skip trailing newline only if it exists. */
    if ( *b == '\n' ) {
      b --;
    } else {
      PDEBUG((stderr, "%s('%s'): no trailing newline found.\n", _funcname, _file));
    }

    /* Find newline before trailing newline. */
    while ( b >= buf ) {
      if ( *b == '\n' ) {
      line = b + 1;
      PDEBUG((stderr, "%s('%s'): found previous newline.\n", _funcname, _file));
      break;
      }
      -- b;
    }

    /*
    ** If no newline was found and
    ** if buf starts at beginning of file,
    **   the buf is the first line.
    */
    if ( (! line) && pos == 0 ) {
      PDEBUG((stderr, "%s('%s'): read entire file.\n", _funcname, _file));
      line = buf;
      break;
    }

    /*
    ** Try growing buflen until a complete line is read.
    */
  } while ( (! line) && (buflen *= 2) );

  if ( line ) {
    assert(buf);

    /*
    ** The line's start position is offset from
    ** the buffer's start position by the offset of the line
    ** in the buffer.  Got it?
    */
    pos += (line - buf);

    /*
    ** Resize buf to only contain the line.
    */
    buflen = e - line;
    memmove(buf, line, sizeof(buf[0]) * buflen);
    buf = realloc(buf, sizeof(buf[0]) * (buflen + 1));
    buf[buflen] = '\0';
    line = buf;

    /*
    ** Pass back bufsize and line pos.
    */
    if ( buflenp )
      *buflenp = buflen;

    if ( posp )
      *posp = pos;
  } else {
    /* Done with buf. */
    if ( buf )
      free(buf);
  }

  PDEBUG((stderr, "%s('%s'): return '%s': buflen %ld pos %ld\n", _funcname, _file,
        line ? line : "<null>",
        (long) buflen,
        (long) pos));

  return line;
}


char *file_get_line_at_end(const char *file, size_t *buflenp, pos_t *posp)
{
  FILE *fp;
  char *buf;

  _file = file; /* for debugging. */

  if ( ! (fp = fopen(file, "r")) ) {
    fprintf(stderr, "file_get_line_at_end('%s'): cannot open: %s\n", file,
          strerror(errno));
    fflush(stderr);
    return 0;
  }

  buf = FILE_get_line_at_end(fp, buflenp, posp);

  fclose(fp);

  _file = "<unknown>"; /* for debugging. */

  return buf;
}

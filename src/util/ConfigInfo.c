#include "ConfigInfo.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h> /* isspace() */
#include <unistd.h>
#include <sys/types.h>
#include <sys/file.h>
#include <sys/stat.h>
#ifdef __NeXT__
#include <libc.h> /* open, read, close */
#endif
#include <fcntl.h>
#include <stdarg.h>
#include <assert.h>

#ifdef MEMDEBUG
#include "memdebug/memdebug.h"
#endif

ConfigInfoError
 ConfigInfoError_OK = "OK",
 ConfigInfoError_CannotStat = "Cannot stat file",
 ConfigInfoError_CannotAlloc = "Cannot allocate memory",
 ConfigInfoError_CannotOpen = "Cannot open file",
 ConfigInfoError_CannotRead = "Cannot read file",
 ConfigInfoError_;

ConfigInfoError ConfigInfo_dump(ConfigInfo *c, FILE *fp, ConfigInfo *style, int depth)  /* <DOC FUNC>Dumps the config ^c^ to ^fp^, using ^style^'s lexical controls.  Recurses to ^depth^, or to bottom of config chain if ^depth^ < 0. */
{
  if ( ! style ) {
    static ConfigInfo defaultStyle;
    defaultStyle.fieldSep = "\t";
    defaultStyle.recordSep = "\n";
    defaultStyle.commentBegin = "#";
    defaultStyle.commentEnd = defaultStyle.recordSep;
    style = &defaultStyle;
  }

  while ( c ) {
    int i;

    fprintf(fp, "%sfrom %s%s", 
	    style->commentBegin ? style->commentBegin : "", 
	    c->from_where ? c->from_where : "",
	    style->commentEnd ? style->commentEnd : "");

    for ( i = 0; i < c->nconfigs; i ++ ) {
      int j;
      
      for ( j = 0; j < c->nfields[i]; ) {
	fwrite(c->configs[i][j], 1, strlen(c->configs[i][j]), fp);
	if ( ++ j < c->nfields[i] ) {
	  fwrite(style->fieldSep, 1, strlen(style->fieldSep), fp);
	}
      }
      
      fwrite(style->recordSep, 1, strlen(style->recordSep), fp);
    }
    
    if ( depth > 0 ) {
      depth --;
    }
    if ( depth == 0 )
      break;
    c = c->chain;
  } 

  return ConfigInfoError_OK;
}

ConfigInfoError ConfigInfo_destroy(ConfigInfo *c) /* <DOC FUNC>Destroys storage assocated with ^c^. */
{
  if ( ! c )
    return ConfigInfoError_OK;

  if ( c->from_where ) {
    free(c->from_where);
    c->from_where = 0;
  }

  if ( c->configs ) {
    int i;
    
    for ( i = 0; i < c->nconfigs; i ++ ) {
      int j;

      /* Free externally allocated config data */
      for ( j = 0; j < c->nfields[i]; j ++ ) {
#if 1
	if ( ! (c->buf <= c->configs[i][j] && c->configs[i][j] < c->buf + c->bufsize) ) {
	  fprintf(stderr, "ConfigInfo_destroy(%p): freeing external string at [%d][%d] '%s'\n", c, i, j, c->configs[i][j]);
	  free(c->configs[i][j]);
	}
#endif
	c->configs[i][j] = 0;
      }

      free(c->configs[i]);
      c->configs[i] = 0;
      c->nfields[i] = 0;
    }
    
    free(c->configs);
    c->configs = 0;
  }
  c->nconfigs = 0;

  if ( c->nfields ) {
    free(c->nfields);
    c->nfields = 0;
  }
  
  if ( c->buf ) {
    memset(c->buf, 0, c->bufsize);
    free(c->buf);
    c->buf = 0;
  }
  c->bufsize = 0;

  c->instance_id = 0;

  return ConfigInfoError_OK;
}

ConfigInfoError ConfigInfo_free(ConfigInfo *c, int free_chain) /* <DOC FUNC>Destroy storage assocated with ^c^ and frees ^c^.  Recurses on all chains if ^free_chain^ is non-zero. */
{
  while ( c ) {
    ConfigInfo *chain;

    chain = c->chain;

    ConfigInfo_destroy(c);

    memset(c, 0, sizeof(*c));
    free(c);

    if ( ! free_chain ) {
      break;
    }

    c = chain;
  }

  return ConfigInfoError_OK;
}

static int strprefix(const char *str, const char *pre)
{
  if ( ! pre )
    return 0;
    
  while ( *pre ) {
    if ( *str != *pre )
      return 0;
    str ++;
    pre ++;
  }
  return 1;
}

ConfigInfoError ConfigInfo_initFromBuffer(ConfigInfo *c, char *buf, size_t bufsize)
{
  ConfigInfoError result = ConfigInfoError_OK;
  int commentEndIsRecordSep;
  int i, j;
  char *s, *next_line;
  char *bufend;

#define return(X) do { result = (X); goto rtn; } while(0)

  c->buf = buf;
  c->bufsize = bufsize != (size_t) -1 ? bufsize : strlen(buf) + 1;
  c->chain = 0;
  bufend = c->buf + c->bufsize;

  if ( ! c->recordSep ) {
    c->recordSep = "\n";
  }
  if ( ! c->fieldSep ) {
    c->fieldSep = "\t";
  }
  if ( ! c->commentEnd ) {
    c->commentEnd = c->recordSep;
  }
  commentEndIsRecordSep = ! strcmp(c->commentEnd, c->recordSep);
   
  
  /* Count the number of lines so we can guesstimate the length of c->configs */
  for ( s = c->buf, i = 0; *s; ) {
    if ( strprefix(s, c->recordSep) ) {
      s += strlen(c->recordSep);
      i ++;
    } else {
      s ++;
    }
  }
  
  /* Allocate the config buffer */
  if ( (c->configs = calloc(sizeof(c->configs[0]), (i + 1))) == 0 ) {
    return ConfigInfoError_CannotAlloc;
  }

  /* Allocate the nfields buffer */
  if ( (c->nfields = calloc(sizeof(c->nfields[0]), (i + 1))) == 0 ) {
    return ConfigInfoError_CannotAlloc;
  }
  
  /* Skip header records */
  for ( s = c->buf, i = c->skipHeaderRecords; i && s < bufend; ) {
    if ( strprefix(s, c->recordSep) ) {
      s += strlen(c->recordSep);
      i --;
    } else {
      s ++;
    }
  }

  /* Begin parsing each config */
  for ( i = 0; *s && s < bufend; s = next_line ) {
    char *t;

    /* Skip leading whitespace */
    if ( c->skipLeadingWhiteSpace ) {
      while ( *s && isspace(*s) ) 
	s ++;
    }

    /* Find next_line */
    next_line = s;
    while ( *next_line && next_line < bufend ) {
      if ( strprefix(next_line, c->recordSep) ) {
	next_line += strlen(c->recordSep);
	break;
      }
      next_line ++;
    }

    /* Skip trailing whitespace */
    if ( c->skipTrailingWhiteSpace ) {
      t = next_line;
      t -= strlen(c->recordSep);
      while ( t > s ) {
	if ( isspace(*t) ) {
	  char *dst = t;
	  const char *src = c->recordSep;
	  while ( *src )
	    *(dst ++) = *(src ++);
	  t --;
	} else {
	  break;
	}
      }
    }

    /* Skip comment only lines */
#define SKIP_COMMENTS(s,E) if ( strprefix(s, c->commentBegin) ) { (void) (E); s += strlen(c->commentBegin); while ( *s && ! strprefix(s, c->commentEnd) ) s ++; if ( *s && ! commentEndIsRecordSep ) { s += strlen(c->commentEnd); } }

    SKIP_COMMENTS(s,0)
    
    /* Skip blank lines */
    if ( c->skipBlankRecords && strprefix(s, c->recordSep) ) {
      s += strlen(c->recordSep);
      continue;
    }
    
    /* Count the number of field separators to g */
    for ( t = s, j = 1; *t && ! strprefix(t, c->recordSep); ) {
      SKIP_COMMENTS(t,0) else
      if ( strprefix(t, c->fieldSep) ) {
        t += strlen(c->fieldSep);
	j ++;
      } else {
        t ++;
      }
    }
    if ( c->maxFields && j > c->maxFields ) {
      j = c->maxFields;
    }
    c->nfields[i] = j;

    /* Allocate an array for the fields */
    if ( (c->configs[i] = calloc(sizeof(c->configs[0][0]), (j + 1))) == 0 ) {
      return(ConfigInfoError_CannotAlloc);
    }

    /* Parse out the fields */
    for (
      t = s, j = 1, c->configs[i][0] = t;
      *t && ! strprefix(t, c->recordSep) ;
      ) {
      
      if ( j == c->nfields[i] ) {
	while ( *t && ! strprefix(t, c->recordSep) ) {
	  SKIP_COMMENTS(t,*t = '\0') else
	  t ++;
	}
	break;
      }
      
      SKIP_COMMENTS(t,*t = '\0')
      
      if ( strprefix(t, c->fieldSep) ) {
	*t = '\0';
        t += strlen(c->fieldSep);
	c->configs[i][j ++] = t;
      } else {
        t ++;
      }
    }
    
    if ( strprefix(t, c->recordSep) ) {
      *t = '\0';
      t += strlen(c->recordSep);
    }
        
    s = t;
    c->configs[i][j] = 0;
    c->nfields[i] = j;
    i ++;
  }
  
  c->nconfigs = i;

  if ( c->from_where ) {
    c->from_where = strcpy(ConfigInfo_MALLOC(strlen(c->from_where) + 1), c->from_where);
  }

  {
    static int instance_id = 0;
    c->instance_id = ++ instance_id;
  }

  /*
  ConfigInfo_dump(c, stderr, 0, 0);
  */

  rtn:
  if ( result != ConfigInfoError_OK ) {
    ConfigInfo_destroy(c);
  }

#undef return  
  return result;
}

ConfigInfoError ConfigInfo_initFromFile(ConfigInfo *c, const char *file)
{
  ConfigInfoError result = ConfigInfoError_OK;
  int fd = -1;
  char *buf;
  size_t bufsize;
  
#define return(X) do { result = (X); goto rtn; } while(0)

  /* Determine the size of the parse buffer */
  {
    struct stat s;
    
    if ( stat(file, &s) == -1 ) {
      return(ConfigInfoError_CannotStat);
    }
    
    bufsize = s.st_size;
  }
  
  /* Allocate the parse buffer */
  if ( (buf = malloc(sizeof(buf[0]) * (bufsize + 1))) == 0 ) {
    return(ConfigInfoError_CannotAlloc);
  }
 
  /* Open the file and read into the parse buffer */
  if ( (fd = open(file, O_RDONLY)) >= 0 ) {
    size_t r;
    size_t bufi = 0;
    size_t bufs = bufsize;

    bufsize = 0;
    while ( (r = read(fd, buf + bufi, bufs)) > 0 ) {
      bufi += r;
      bufs -= r;
      bufsize += r;
    }

    if ( bufsize > 0 ) {
      buf[bufsize] = '\0';
    } else {
      return(ConfigInfoError_CannotRead);
    }
    close(fd);
    fd = -1;
  } else {
    return(ConfigInfoError_CannotOpen);
  }
  
  c->from_where = (char*) file;
  result = ConfigInfo_initFromBuffer(c, buf, bufsize + 1);

  rtn:
  if ( result != ConfigInfoError_OK ) {
    if ( fd != -1 )
      close(fd);
  }

#undef return  
  return result;
}

ConfigInfoError ConfigInfo_initFromValueArray(ConfigInfo *c, const char **values)
{
  char *buf;
  size_t bufsize;
  char *s, *t;
  char **v;

  bufsize = 0;
  v = (char **) values;
  while ( (t = *(v ++)) ) {
    bufsize += strlen(t) + 3;
    while ( (t = *(v ++)) ) {
      bufsize += strlen(t) + 3;
    }
  }
  
  c->fieldSep = "\02";
  c->recordSep = "\01";
  c->commentBegin = c->commentEnd = 0;
  c->maxFields = 0;

  if ( (buf = malloc(sizeof(buf[0]) * (bufsize + 1))) == 0 ) {
    return(ConfigInfoError_CannotAlloc);
  }
  
  s = buf;

  v = (char **) values;
  while ( (t = *(v ++)) ) {
    assert(s < buf + bufsize);
    strcpy(s, t);
    while ( (t = *(v ++)) ) {
      assert(s < buf + bufsize);
      strcat(s, c->fieldSep);
      strcat(s, t);
      s = strchr(s, '\0');
   }
   strcat(s, c->recordSep);
   s = strchr(s, '\0');
  }

  assert(s < buf + bufsize);
  *s = '\0';

  if ( ! c->from_where )
    c->from_where = "values";

  return ConfigInfo_initFromBuffer(c, buf, -1);
}

ConfigInfoError ConfigInfo_initFromValues(ConfigInfo *c, ...)
{
  const char *t;
  va_list vap;
  int nvalues = 0;
  const char **values = 0;
  ConfigInfoError error = ConfigInfoError_OK;

  va_start(vap, c);
  while ( (t = va_arg(vap, char*)) ) {
    nvalues ++;
    while ( (t = va_arg(vap, char*)) ) {
      nvalues ++;
    }
    nvalues ++; /* null term */
  }
  va_end(vap);
  nvalues ++; /* null term */

  values = malloc(sizeof(values[0]) * nvalues);

  va_start(vap, c);
  nvalues = 0;
  while ( (t = va_arg(vap, char*)) ) {
    values[nvalues ++] = t;
    while ( (t = va_arg(vap, char*)) ) {
      values[nvalues ++] = t;
    }
    values[nvalues ++] = t;
  }
  va_end(vap);
  values[nvalues ++] = t;

  error = ConfigInfo_initFromValueArray(c, values);

  free(values);

  return error;
}

ConfigInfoError ConfigInfo_initFromEnvironment(ConfigInfo *c, const char **env)
{
  ConfigInfoError error = ConfigInfoError_OK;
  char *buf;
  size_t bufsize;
  int i;
  char *s;
  
  bufsize = 1;
  
  for ( i = 0; env[i]; i ++ ) {
    bufsize += strlen(env[i]) + 1;
  }
  
  if ( (buf = malloc(sizeof(buf[0]) * (bufsize + 1))) == 0 ) {
    return(ConfigInfoError_CannotAlloc);
  }

  c->commentBegin = 
  c->commentEnd = 0;
  c->fieldSep = "=";
  c->recordSep = "\01";
  c->maxFields = 2;

  for ( s = buf, i = 0; env[i]; i ++ ) {
    assert(s < buf + bufsize);
    strcpy(s, env[i]);
    strcat(s, c->recordSep);
    s = strchr(s, '\0');
  }
  
  assert(s < buf + bufsize);
  assert(s == buf + bufsize - 1);

  c->from_where = "environment";

  error = ConfigInfo_initFromBuffer(c, buf, bufsize);

  return error;
}

const char 
	*ConfigInfo_ARGV0 = "*argv0*",
	*ConfigInfo_ARGV = "*argv*",
	*ConfigInfo_ARGV0_BASENAME = "*argv0-basename*";

ConfigInfoError ConfigInfo_initFromArgv(ConfigInfo *c, int argc, const char **argv, const char *opts)
{
  char *buf;
  size_t bufsize;
  int i;
  char *s;
  const char *argv0_basename;
  
  if ( ! opts )
    opts = "";

  argv0_basename = (argv0_basename = strrchr(argv[0], '/')) ? argv0_basename + 1 : argv[0];

  bufsize = 0;

  bufsize += strlen(ConfigInfo_ARGV) + 3;
  bufsize += strlen(ConfigInfo_ARGV0) + strlen(argv[0]) + 3;
  bufsize += strlen(ConfigInfo_ARGV0_BASENAME) + strlen(argv0_basename) + 3;
  
  for ( i = 0; i < argc && argv[i]; i ++ ) {
    bufsize += (strlen(argv[i]) + 3) * 2;
  }
  
  if ( (buf = malloc(sizeof(buf[0]) * (bufsize + 1))) == 0 ) {
    return(ConfigInfoError_CannotAlloc);
  }

  c->fieldSep = "\02";
  c->recordSep = "\01";
  c->commentBegin = c->commentEnd = 0;
  c->maxFields = 0;

  s = buf;

  strcpy(s, ConfigInfo_ARGV0);
  strcat(s, c->fieldSep);
  strcat(s, argv[0]);
  strcat(s, c->recordSep);
  s = strchr(s, '\0');

  strcpy(s, ConfigInfo_ARGV0_BASENAME);
  strcat(s, c->fieldSep);
  strcat(s, argv0_basename);
  strcat(s, c->recordSep);
  s = strchr(s, '\0');
  
  for ( i = 1; i < argc && argv[i]; i ++ ) {
    const char *arg = argv[i];
    const char *t;

    assert(s < buf + bufsize);

    /* Does the arg start with '-'? */
    if ( *arg != '-' )
      break;
      
    /* It might be a switch */
    /* Skip extra '-' */
    arg ++;
    if ( *arg == '-' )
      arg ++;
    
    /* Not a switch */
    if ( ! *arg )
      break;
 
    /* Look for "--[" ... "]" */
    if ( arg[0] == '[' && arg[1] == '\0' ) {
      i ++;
      while ( i < argc && argv[i] && strcmp(argv[i], "]") != 0 ) {
	strcat(s, argv[i]);
	if ( ++ i < argc ) {
	  strcat(s, c->fieldSep);
	}
      }
    } else
    /* Look for "--?option=value" */
    if ( (t = strchr(arg, '=')) ) {
      strncpy(s, arg, t - arg);
      s += t - arg;
      strcpy(s, c->fieldSep);
      strcat(s, t + 1);
    } else {
      strcpy(s, arg);
      strcat(s, c->fieldSep);
      
      /* Is a a single op char? */
      if ( arg[1] == '\0' && strchr(opts, arg[0]) ) {
	strcat(s, "1");
      } else {

	/* Must be "--?option" "value" */
	i ++;
	if ( i < argc && argv[i] ) {
	  strcat(s, argv[i]);
	}
      }
    }
    strcat(s, c->recordSep);
    s = strchr(s, '\0');
  }

  /* Store remaining args in *argv* */
  strcpy(s, ConfigInfo_ARGV);
  s = strchr(s, '\0');    
  for ( ; i < argc && argv[i]; i ++ ) {
    assert(s < buf + bufsize);

    strcpy(s, c->fieldSep);
    strcat(s, argv[i]);
    s = strchr(s, '\0');
  }
  strcpy(s, c->recordSep);
  s = strchr(s, '\0');
  assert(s < buf + bufsize);

  c->from_where = "argv";

  return ConfigInfo_initFromBuffer(c, buf, -1);
}


int ConfigInfo_getRowIndexForKey(ConfigInfo *c, int keyfield, const char *key, ConfigInfo **fc)
{
  if ( fc ) {
    *fc = 0;
  }

  if ( key ) while ( c ) {
    int i;

    for ( i = 0; i < c->nconfigs; i ++ ) {
      if ( keyfield < c->nfields[i] && strcmp(c->configs[i][keyfield], key) == 0 ) {
	if ( fc )
	  *fc = c;
        return i;
      }
    }

    c = c->chain; 
  }

  return -1;
}

const char **ConfigInfo_getRowForKey(ConfigInfo *c, int keyfield, const char *key)
{
  ConfigInfo *fc = 0;
  int i = ConfigInfo_getRowIndexForKey(c, keyfield, key, &fc);
  if ( i >= 0 && fc ) {
    return (const char **) fc->configs[i];
  }
  return 0;
}

const char *ConfigInfo_getValue1(ConfigInfo *c, const char *key)
{
  if ( key && c ) {
    int i;
    int keyfield = 0;

    for ( i = 0; i < c->nconfigs; i ++ ) {
      if ( keyfield < c->nfields[i] && strcmp(c->configs[i][keyfield], key) == 0 ) {
        return c->configs[i][1];
      }
    }
  }
  return 0;
}

const char *ConfigInfo_getValue(ConfigInfo *c, const char *key)
{
   const char **row;
   
   return (const char *) ((row = ConfigInfo_getRowForKey(c, 0, key)) ? row[1] : 0);
}

const char *ConfigInfo_getValueOr(ConfigInfo *c, const char *key, const char *alt)
{
  const char *value = ConfigInfo_getValue(c, key);
  return value ? value : alt;
}

int ConfigInfo_strToIntValue(const char *value)
{
  if ( value ) {
    int c;
    
    while ( *value && isspace(*value) )
      value ++;
    
    c = tolower(value[0]);
    if ( c == 'y' || c == 't' ) {
      return -1;
    } else
    if ( c == 'n' || c == 'f' ) {
      return 0;
    } else if ( *value ) {
      return atoi(value);
    }
  }
  
  return 0;
}

int ConfigInfo_strToBooleanValue(const char *str)
{
  return ConfigInfo_strToIntValue(str);
}

int ConfigInfo_getIntValue(ConfigInfo *c, const char *key)
{
  return ConfigInfo_strToIntValue(ConfigInfo_getValue(c, key));
}

double ConfigInfo_getDoubleValue(ConfigInfo *c, const char *key)
{
  const char *value;
  return (value = ConfigInfo_getValue(c, key)) ? atof(value) : 0;
}

int ConfigInfo_getBooleanValue(ConfigInfo *c, const char *key)
{
  return ConfigInfo_strToBooleanValue(ConfigInfo_getValue(c, key));
}

/*********************************************************************/

ConfigInfoError ConfigInfo_initFromDefaults(ConfigInfo *options, ConfigInfo *defaults, int argc, char **argv, char **env, ConfigInfo **configp)
{
  ConfigInfo *config = 0;
  ConfigInfo *defaultConfig = 0, *envConfig = 0, *argvConfig = 0, *fileConfig = 0;
  static char error_buf[1024];
  ConfigInfoError error = ConfigInfoError_OK;
  const char *opts = ConfigInfo_getValue(options, "opts");

  *configp = 0;

  /* Config search should be Argv -> Configfile -> Env -> Defaults */

  /* Defaults */
  defaultConfig = defaults;

  /* Environment */
  envConfig = ConfigInfo_MALLOC(sizeof(*envConfig));
  memset(envConfig, 0, sizeof(*envConfig));
  error = ConfigInfo_initFromEnvironment(envConfig, (const char **) env);
  if ( error != ConfigInfoError_OK ) {
    goto rtn;
  }
  envConfig->chain = defaults;
  config = envConfig;

  /* Config chain is now: env, defaults */
   
  /* Argv */
  argvConfig = ConfigInfo_MALLOC(sizeof(*argvConfig));
  memset(argvConfig, 0, sizeof(*argvConfig));
  error = ConfigInfo_initFromArgv(argvConfig, argc, (const char **) argv, opts);
  if ( error != ConfigInfoError_OK ) {
    goto rtn;
  }

  /* Config chain: argv, environment, defaults */
  argvConfig->chain = envConfig;
  config = argvConfig;
   
  /* config file */
  {
    const char *configfile;
    const char *basename;
    const char *template;
    char filename[1024];
 
    /* Check for option configfile name override */
    configfile = ConfigInfo_getValue(config, "configfile");
    /* fprintf(stderr, "configfile = '%s'\n", configfile); */
    if ( ! (configfile && *configfile) ) {
      /* Try to get the name of a config file */
      basename = ConfigInfo_getValueOr(options, "configfile/basename", ConfigInfo_ARGV0_BASENAME);
      basename = ConfigInfo_getValueOr(config, basename, "");
      
      template = ConfigInfo_getValueOr(options, "configfile/template", "configfile");
      template = ConfigInfo_getValueOr(config, template, "");
      
      sprintf(filename, template, basename);
      configfile = filename;
    }

    if ( *configfile ) {
      if ( ConfigInfo_getIntValue(config, "debug") ) {
	fprintf(stderr, "Loading config... \"%s\" ", configfile);
	fflush(stderr);
      }

      /* Allocate */
      fileConfig = ConfigInfo_MALLOC(sizeof(*fileConfig));
      memset(fileConfig, 0, sizeof(*fileConfig));

      /* Fill in scanning options and defaults */
      fileConfig->recordSep = ConfigInfo_getValueOr(options, "configfile/recordSep", "\n");
      fileConfig->fieldSep = ConfigInfo_getValueOr(options, "configfile/fieldSep", "\t");

      fileConfig->commentBegin = ConfigInfo_getValueOr(options, "configfile/commentBegin", "#");
      fileConfig->commentEnd = ConfigInfo_getValueOr(options, "configfile/commentEnd", fileConfig->recordSep);

      fileConfig->skipLeadingWhiteSpace = ConfigInfo_strToBooleanValue(ConfigInfo_getValueOr(options, "configfile/skipLeadingWhiteSpace", "1"));
      fileConfig->skipTrailingWhiteSpace = ConfigInfo_strToBooleanValue(ConfigInfo_getValueOr(options, "configfile/skipTrailingWhiteSpace", "1"));

      fileConfig->skipBlankRecords = ConfigInfo_strToBooleanValue(ConfigInfo_getValueOr(options, "configfile/skipBlankRecords", "1"));

      fileConfig->maxFields = ConfigInfo_strToIntValue(ConfigInfo_getValueOr(options, "configfile/maxFields", "0"));

      if ( (error = ConfigInfo_initFromFile(fileConfig, configfile)) != ConfigInfoError_OK ) {
	sprintf(error_buf, "%s: Cannot read configfile \"%s\"\n", error, configfile);
	error = error_buf;
	goto rtn;
      }

      fileConfig->chain = envConfig;
      argvConfig->chain = fileConfig;
      config = argvConfig;
      /* Config chain is now: argv, configfile, environment, defaults */

      if ( ConfigInfo_getIntValue(config, "debug") ) {
	fprintf(stderr, "done.\n");
	fflush(stderr);
      }
    }
  }

 rtn:
  if ( error != ConfigInfoError_OK ) {
    ConfigInfo_free(argvConfig, 0);
    ConfigInfo_free(envConfig, 0);
    ConfigInfo_free(defaultConfig, 0);
    ConfigInfo_free(fileConfig, 0);
    config = 0;
  }

  *configp = config;

  return error;
}

/* EOF */




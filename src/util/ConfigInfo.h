#ifndef _ConfigInfo_h
#define _ConfigInfo_h

#ifndef __rcs_id__
#ifndef __rcs_id_util_ConfigInfo_h__
#define __rcs_id_util_ConfigInfo_h__
static const char __rcs_id_util_ConfigInfo_h[] = "$Id: ConfigInfo.h,v 1.4 1999/04/05 19:04:21 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

#include <stddef.h> /* size_t */
#include <stdio.h> /* FILE */

#define ConfigInfo_MALLOC(X) malloc(X)
#define ConfigInfo_REALLOC(X,Y) realloc(X,Y)
#define ConfigInfo_FREE(X) free(X)

typedef struct ConfigInfo { /* <DOC STRUCT>Contains the state for a ConfigInfo node. */

  /* <DOC><DT><BR><B>Configuration Data</B><DD></DOC> */
  char ***configs;            /* <DOC>Configurations are records containing any number of fields; generally configs[i][0] is the key for finding a configuration value. */
  int *nfields;               /* <DOC>The number of fields for a configs[i]. */
  int nconfigs;               /* <DOC>The number of rows in configs */
  void **user;                /* <DOC>User-row data */
  
  /* <DOC><DT><BR><B>Memory</B><DD></DOC> */
  char *buf;	              /* <DOC>Buffer allocated for initialization; all config values are within this buffer. */
  size_t bufsize;             /* <DOC>The size of buf. */
  
  /* <DOC><DT><BR><B>Lexical Controls</B><DD></DOC> */
  const char *commentBegin;   /* <DOC>The string that begins a comment. */
  const char *commentEnd;     /* <DOC>The string that ends a comment. */
  const char *fieldSep;       /* <DOC>The string that delimits a field. */
  const char *recordSep;      /* <DOC>The string that delimits a record. */
  
  int skipLeadingWhiteSpace;  /* <DOC>If !0, skip any leading whitespace chars. */
  int skipTrailingWhiteSpace; /* <DOC>If !0, skip any leading whitespace chars. */
  int skipBlankRecords;       /* <DOC>If !0, skip any records with no fields. */
  int skipHeaderRecords;      /* <DOC>If !0, skip this number of records at the beginning. */
  int maxFields;              /* <DOC>Specifies the max number of fields. */
  
  /* <DOC><DT><BR><B>Search Chain</B><DD></DOC> */
  struct ConfigInfo *chain;   /* <DOC>The next node to search if a search failed. ConfigInfo nodes can be chained to allow different configuration sources, like command line options, environment variables, default configurations and configuration files seem like one configuration. */

  /* <DOC><DT><BR><B>Initialization</B><DD></DOC> */
  char *from_where;           /* <DOC>Where is this ConfigInfo from? */

  int instance_id;            /* <DOC>Each ConfigInfo struct gets a unique id when initialized.  Use this to determine if a configuration has been reinitialized. */
} ConfigInfo; /* </DOC STRUCT> */

typedef const char *ConfigInfoError;
extern ConfigInfoError ConfigInfoError_OK,
 ConfigInfoError_CannotStat,
 ConfigInfoError_CannotAlloc,
 ConfigInfoError_CannotOpen,
 ConfigInfoError_CannotRead;

#if 0
ConfigInfoError ConfigInfo_setUserData(ConfigInfo *c, int row, void *ptr);
void *ConfigInfo_userData(ConfigInfo *c, int row, void *ptr);
#endif

ConfigInfoError ConfigInfo_dump(ConfigInfo *c, FILE *fp, ConfigInfo *style, int depth);

ConfigInfoError ConfigInfo_destroy(ConfigInfo *c);

ConfigInfoError ConfigInfo_free(ConfigInfo *c, int free_chain);
ConfigInfoError ConfigInfo_initFromBuffer(ConfigInfo *c, char *buf, size_t bufsize);

ConfigInfoError ConfigInfo_initFromValueArray(ConfigInfo *c, const char **values);

ConfigInfoError ConfigInfo_initFromValues(ConfigInfo *c, ...);

ConfigInfoError ConfigInfo_initFromFile(ConfigInfo *c, const char *file);
ConfigInfoError ConfigInfo_initFromEnvironment(ConfigInfo *c, const char **env);

extern const char 
	*ConfigInfo_ARGV0,
	*ConfigInfo_ARGV;
ConfigInfoError ConfigInfo_initFromArgv(ConfigInfo *c, int argc, const char **argv, const char *opts);

int ConfigInfo_getRowIndexForKey(ConfigInfo *c, int keyfield, const char *key, ConfigInfo **cfound);
const char **ConfigInfo_getRowForKey(ConfigInfo *c, int keyfield, const char *key);

const char *ConfigInfo_getValue1(ConfigInfo *c, const char *key); 

const char *ConfigInfo_getValue(ConfigInfo *c, const char *key); /* same as ConfigInfo_getRowForKey(c, 0, key)[1] */
const char *ConfigInfo_getValueOr(ConfigInfo *c, const char *key, const char *or); /* same as ConfigInfo_getRowForKey(c, 0, key)[1] */

ConfigInfoError ConfigInfo_initFromDefaults(ConfigInfo *options, ConfigInfo *defaults, int argc, char **argv, char **env, ConfigInfo **configp);

int ConfigInfo_strToIntValue(const char *str);
#define ConfigInfo_strToInt ConfigInfo_strToIntValue
int ConfigInfo_strToBooleanValue(const char *str);
#define ConfigInfo_strToBoolean ConfigInfo_strToBooleanValue

int ConfigInfo_getIntValue(ConfigInfo *c, const char *key);
double ConfigInfo_getDoubleValue(ConfigInfo *c, const char *key);
int ConfigInfo_getBooleanValue(ConfigInfo *c, const char *key);

#endif

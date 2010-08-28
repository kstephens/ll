#include "ConfigInfo.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h> /* isspace() */

#include "sqldummy.c"

int main(int argc, char **argv, char **env)
{
  ConfigInfo *config;
  
  passthru_config_init(argc, argv, env);
  
  config = passthru_config();
  
  {
    char buf[1024];
    
    while ( gets(buf) ) {
      char *key = buf;
      char **rec;
      
      if ( key[0] == 'p' ) {
      	char *user, *password;
	
      	while ( ! isspace(*key) ) key ++;
	while ( isspace(*key) ) key ++;
	user = key;
	while ( ! isspace(*key) ) key ++;
	*(key ++) = '\0';
	while ( isspace(*key) ) key ++;
	password = key;
	while ( ! isspace(*key) ) key ++;
	*(key ++) = '\0';
	printf("user = '%s', password = '%s'\n", user, password);
	rec = (char **) passthru_map_login(user, password);
      } else {
      	rec = ConfigInfo_getRowForKey(config, 0, key);
      }
      
      if ( rec ) {
	int i;
	
	for ( i = 0; rec[i]; i ++ ) {
	  printf("\"%s\"\t", rec[i]);
	}
	printf("\n");
      } else {
	fprintf(stderr, "No record for '%s'\n", buf);
      }
    }
  }
  
  return 0;
}

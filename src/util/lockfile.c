/* $Id: lockfile.c,v 1.6 2000/01/04 06:22:24 stephensk Exp $ */

#include "lockfile.h"
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <pwd.h>
#include <stdlib.h>

#include "errlist.h"
#ifdef MEMDEBUG
#include "memdebug/memdebug.h"
#endif

/* NOT THREAD-SAFE */

const char *lockfile_progname = "<unknown>"; /* The program name for lockfile contents. */

/* A lockfile list entry. */
typedef struct lockfile_stat {
  struct lockfile_stat *next;
  char *file;
  int locks;
} lockfile_stat;

/* A list of lockfiles aquired by this process. */
static lockfile_stat *lockfile_stats;

/* Has lockfile_atexit been installed into atexit? */
static int lockfile_atexit_installed;

void lockfile_release_all()
{
  lockfile_stat *l = lockfile_stats;
  while ( l ) {
    lockfile_stat *ln = l->next;
    unlink(l->file);
    free(l->file);
    free(l);
    l = ln;
  }
}

int lockfile_unlock_at_exit = 1;

void lockfile_atexit()
{
  if ( lockfile_unlock_at_exit ) {
    lockfile_release_all();
  }
}

static void lockfile_stat_add(const char *file)
{
  lockfile_stat *l;

  l = malloc(sizeof(*l));
  l->file = strcpy(malloc(strlen(file) + 1), file);
  l->locks = 1;
  l->next = lockfile_stats;
  lockfile_stats = l;

  /* Install an atexit handler to unaquire all lockfiles this process has created. */
  if ( ! lockfile_atexit_installed ) {
    lockfile_atexit_installed ++;
    atexit(lockfile_atexit);
  }
}

static lockfile_stat **lockfile_stat_find(const char *file)
{
  lockfile_stat **l = &lockfile_stats;

  while ( *l ) {
    if ( strcmp(file, (*l)->file) == 0 ) {
      return l;
    }
    l = &((*l)->next);
  }

  return 0;
}

int lockfile_mode = 0666;

/* The error handler. */
int lockfile_error_default(const char *file, const char *msg)
{
  fprintf(stderr, "lockfile '%s': %s\n", file, msg);
  return 0;
}

int (*lockfile_error)(const char *file, const char *msg) = lockfile_error_default;

int lockfile_acquire(const char *file, int retries, int wait)
{
  lockfile_stat **l;
  int rtnval = 0;
  int nretries = 0;

  if ( retries < 0 ) {
    retries = 10;
  }
  if ( wait < 0 ) {
    wait = 60;
  }

  /* Has this process already acquired the lockfile? */
  if ( (l = lockfile_stat_find(file)) ) {
    /* Increment lock count. */
    (*l)->locks ++;

    /* Success. */
    return 0;
  } else {
    int fd;
    char buf[1024];
    int lockfile_read = 0;
    char msg[1024];
    int broke_lock = 0;

    buf[0] = '\0';
    
    while ( 1 ) {
      /* Attempt to create the file atomically. */
      fd = open(file, O_RDWR|O_CREAT|O_EXCL, 0666);
      if ( fd < 0 ) {
	/* Couldn't create the file atomically. */

	/* It already existed and was not created by our process id. */
	if ( errno == EEXIST ) {

	  /* Read the contents of the lockfile for the error msg. */
	  if ( ! lockfile_read ) {
	    size_t rlen;
	    
	    fd = open(file, O_RDONLY);
	    rlen = read(fd, buf, sizeof(buf) - 1);

	    /* Remove newline. */
	    if ( rlen > 0 && rlen != (size_t) -1 )
	      buf[rlen - 1] = '\0';
	   
	    close(fd);
	    lockfile_read = 1;

	    sprintf(msg, "acquired by '%s'", buf);
	    if ( (rtnval = lockfile_error(file, msg)) )
	      return rtnval;
	  }

	  /* Sleep if retries are left. */
	  if ( -- retries > 0 ) {
	    nretries ++;
	    
	    if ( nretries == 1 ) {
	      sprintf(msg, "sleeping for %d secs for %d times", wait, retries + 1);
	      if ( (rtnval = lockfile_error(file, msg)) )
		return rtnval;
	    }

	    /* Sleep for a while then retry */
	    sleep(wait);
	  }

	  /* If out of retries, attempt to break the lock. */
	  if ( retries <= 0 ) {
	    if ( ! broke_lock ) {
	      if ( (rtnval = lockfile_error(file, "attempting to break lock")) )
		return rtnval;
	      
	      /* Attempt one retry after breaking the lock. */
	      broke_lock ++;
	      retries = 1;
	      
	      unlink(file);
	    } else {
	      /* Out of retries. */
	      if ( (rtnval = lockfile_error(file, "giving up")) )
		return rtnval;

	      /* Failure! */
	      return -1;
	    }
	  }
	} else {
	  /* Failed to create the lockfile for some other reason. */
	  sprintf(msg, "cannot create: %s", sys_errlist[errno]);
	  if ( (rtnval = lockfile_error(file, msg)) )
	    return rtnval;

	  /* Failure! */
	  return -1;
	}
      } else {
	/* Fill the lockfile with hostname, username, programname, and process id */
	char hostname[64];
	char username[64];
	struct passwd *pw;
	
	strcpy(hostname, "localhost");
	gethostname(hostname, sizeof(hostname) - 1);
	
	strcpy(username, "<unknown>");
	if ( (pw = getpwuid(geteuid())) ) {
	  memset(username, 0, sizeof(username));
	  strncpy(username, pw->pw_name, sizeof(username) - 1);
	}
	
	sprintf(buf, "%s %s %s %d\n", hostname, username, lockfile_progname, getpid());
	
	write(fd, buf, strlen(buf));
	
	/* Add it to our process's list of lockfiles */
	lockfile_stat_add(file);
	
	close(fd);
	
	/* Chmod to default mode */
	chmod(file, lockfile_mode);
	
	/* Success! */
	return 0;
      }
    } 
  }  
}

int lockfile_release(const char *file)
{
  lockfile_stat **lp;

  /* Has this process acquired the lockfile? */
  if ( (lp = lockfile_stat_find(file)) ) {
    lockfile_stat *l = *lp;

    /* Decrement lock count. */
    if ( -- l->locks == 0 ) {
      lockfile_stat *ln = l->next;

      /* Delete the lockfile. */
      unlink(l->file);

      /* Free and remove the lockfile list entry. */
      free(l->file);
      free(l);
      *lp = ln;
    }
  } else {
    int rtnval;

    /* This process has not acquired the lockfile, so it cannot release it. */
    if ( (rtnval = lockfile_error(file, "not acquired")) )
      return rtnval;

    return -1;
  }

  return 0;
}


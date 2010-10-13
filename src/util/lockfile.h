#ifndef _util_lockfile_h
#define _util_lockfile_h

/* Options */
extern const char *lockfile_progname;
extern int lockfile_mode; /* Lockfile creation perms */
extern int lockfile_unlock_at_exit; /* Release lockfiles at exit? */

/* Error handling. */
extern int (*lockfile_error)(const char *file, const char *msg);
int lockfile_error_default(const char *file, const char *msg); /* Prints "lockfile '<file>': <msg>" to stderr; returns 0. */

/* Acquire/Release. */
int lockfile_acquire(const char *file, int retries, int wait);
int lockfile_release(const char *file);

/* Release all lockfiles. */
void lockfile_release_all();
void lockfile_atexit(); /* lockfile_release_all() if lockfile_unlock_at_exit. */

#endif

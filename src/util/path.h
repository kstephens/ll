#ifndef _util_path_h

#define _util_path_h

extern const char *path_sep; /* defaults to "/" */
extern const char *path_cwd; /* defaults to "." */

const char *path_file(const char *s);
const char *path_dir(const char *s, char *buf);
const char *path_absolute(const char *s, char *buf);
const char *path_suffix(const char *s);
const char *path_no_suffix(const char *s, char *buf);
const char *path_2_dos(const char *s, char *buf);

#endif

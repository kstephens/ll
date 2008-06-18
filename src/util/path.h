#ifndef _util_path_h

#ifndef __rcs_id__
#ifndef __rcs_id_util_path_h__
#define __rcs_id_util_path_h__
static const char __rcs_id_util_path_h[] = "$Id: path.h,v 1.2 1999/02/19 09:26:47 stephensk Exp $";
#endif
#endif /* __rcs_id__ */

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

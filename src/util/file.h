#ifndef _util_file_h
#define _util_file_h

#ifndef __rcs_id__
#ifndef __rcs_id_util_file_h__
#define __rcs_id_util_file_h__
static const char __rcs_id_util_file_h[] = "$Id: file.h,v 1.4 2001/02/01 06:22:18 ion Exp $";
#endif
#endif /* __rcs_id__ */


#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h> /* FILE */
#include <sys/types.h> /* time_t */

time_t file_mtime(const char *fname);
size_t file_size(const char *file);

typedef long pos_t;

int file_touch(const char *file);

int file_mode_masked(const char *fname, int euid, int egid);
int file_executable(const char *fname);
int file_in_path(const char *argv, const char *PATH, char *buf);

char *FILE_get_line_at_pos(FILE *fp, int linesize, pos_t pos, size_t *lenp, pos_t *line_pos);
int FILE_print_line_at_pos(FILE *fp, int linesize, pos_t pos, pos_t *line_pos, FILE *out);
int file_print_line_at_pos(const char *file, int linesize, pos_t pos, pos_t *line_pos, FILE *out);

char *FILE_read_from_end(FILE *file, size_t *buflenp, pos_t *posp);
char *file_read_from_end(const char *file, size_t *buflenp, pos_t *bufposp);

char *FILE_get_line_at_end(FILE *fp, size_t *bufsize, pos_t *lineposp);
char *file_get_line_at_end(const char *file, size_t *bufsize, pos_t *lineposp);

#ifdef __cplusplus
}
#endif

#endif

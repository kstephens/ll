#ifndef _util_file_h
#define _util_file_h

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

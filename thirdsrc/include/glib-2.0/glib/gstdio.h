/* gstdio.h - GFilename wrappers for C library functions
 *
 * Copyright 2004 Tor Lillqvist
 *
 * GLib is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * GLib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with GLib; see the file COPYING.LIB.  If not,
 * write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifndef __G_STDIO_H__
#define __G_STDIO_H__

#include <glib/gprintf.h>

#include <sys/stat.h>

G_BEGIN_DECLS

#if defined (_MSC_VER) && !defined(_WIN64)

/* Make it clear that we mean the struct with 32-bit st_size and
 * 32-bit st_*time fields as that is how the 32-bit GLib DLL normally
 * has been compiled. If you get a compiler warning when calling
 * g_stat(), do take it seriously and make sure that the type of
 * struct stat the code in GLib fills in matches the struct the type
 * of struct stat you pass to g_stat(). To avoid hassle, to get file
 * attributes just use the GIO API instead which doesn't use struct
 * stat.
 *
 * Sure, it would be nicer to use a struct with 64-bit st_size and
 * 64-bit st_*time fields, but changing that now would break ABI. And
 * in MinGW, a plain "struct stat" is the one with 32-bit st_size and
 * st_*time fields.
 */

typedef struct _stat32 GStatBuf;

#else

typedef struct stat GStatBuf;

#endif

#if defined(G_OS_UNIX) && !defined(G_STDIO_NO_WRAP_ON_UNIX)

/* Just pass on to the system functions, so there's no potential for data
 * format mismatches, especially with large file interfaces. 
 * A few functions can't be handled in this way, since they are not defined
 * in a portable system header that we could include here.
 */

#ifndef __GTK_DOC_IGNORE__
#define g_chmod   chmod
#define g_open    open
#define g_creat   creat
#define g_rename  rename
#define g_mkdir   mkdir
#define g_stat    stat
#define g_lstat   lstat
#define g_remove  remove
#define g_fopen   fopen
#define g_freopen freopen
#define g_utime   utime
#endif

int g_access (const gchar *filename,
	      int          mode);

int g_chdir  (const gchar *path);

int g_unlink (const gchar *filename);

int g_rmdir  (const gchar *filename);

#else /* ! G_OS_UNIX */

/* Wrappers for C library functions that take pathname arguments. On
 * Unix, the pathname is a file name as it literally is in the file
 * system. On well-maintained systems with consistent users who know
 * what they are doing and no exchange of files with others this would
 * be a well-defined encoding, preferably UTF-8. On Windows, the
 * pathname is always in UTF-8, even if that is not the on-disk
 * encoding, and not the encoding accepted by the C library or Win32
 * API.
 */

int g_access    (const gchar *filename,
		 int          mode);

int g_chmod     (const gchar *filename,
		 int          mode);

int g_open      (const gchar *filename,
                 int          flags,
                 int          mode);

int g_dup		(int fd);

int g_close		(int _FileHandle);

int g_creat     (const gchar *filename,
                 int          mode);

int g_rename    (const gchar *oldfilename,
                 const gchar *newfilename);

int g_mkdir     (const gchar *filename,
                 int          mode);

int g_chdir     (const gchar *path);

int g_stat      (const gchar *filename,
                 GStatBuf    *buf);

int g_lstat     (const gchar *filename,
                 GStatBuf    *buf);

int g_unlink    (const gchar *filename);

int g_remove    (const gchar *filename);

int g_rmdir     (const gchar *filename);

FILE *g_fopen   (const gchar *filename,
                 const gchar *mode);

FILE *g_freopen (const gchar *filename,
                 const gchar *mode,
                 FILE        *stream);
int g_write(int _Filehandle, const void * _Buf, unsigned int _MaxCharCount);
int g_read(int _FileHandle, void * _DstBuf, unsigned int _MaxCharCount);
long g_lseek(int _FileHandle, long _Offset, int _Origin);
gint64 g_lseek64(int fd, gint64 offset, int ori);
gint64 g_tell64(_In_ int _FileHandle);
#ifdef G_OS_WIN32

int g_stati64(const gchar * _Name, void* _Stat);
int g_fstati64(int _FileDes, void* _Stat);
int g_fstat(int _Desc, void * _Stat);
int g_fstat32(int _Desc, void * _Stat);
#endif

#ifdef G_OS_WIN32
#define HAVE_FLOCKFILE 1

int getc_unlocked (FILE* stream);
int ungetc_unlocked(int Ch, FILE* stream);
void flockfile(FILE* f);
void funlockfile(FILE* f);

#endif

int g_filbuf(FILE * _File );
int g_flsbuf(int _Ch, FILE * _File);

FILE * g_fsopen(const gchar * _Filename, const gchar * _Mode, int _ShFlag);

void g_clearerr(FILE * _File);

int g_fclose(FILE * _File);
int g_fcloseall(void);
FILE * g_fdopen(int _FileHandle, const gchar * _Mode);


int g_feof(FILE * _File);
int g_ferror(FILE * _File);
int g_fflush(FILE * _File);
int g_fgetc(FILE * _File);
int g_fgetpos(FILE * _File ,off_t * _Pos);
char *  g_fgets(char * _Buf, int _MaxCount, FILE * _File);
int  g_flushall(void);
int  g_fputc(int _Ch, FILE * _File);
int  g_fputs(const gchar * _Str, FILE * _File);
size_t g_fread(void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File);


int  g_fsetpos(FILE * _File, const off_t * _Pos);
int  g_fseek(FILE * _File, long _Offset, int _Origin);
long  g_ftell(FILE * _File);

int  g_fseeki64(FILE * _File, gint64 _Offset, int _Origin);
gint64  g_ftelli64(FILE * _File);

size_t  g_fwrite(const void * _Str, size_t _Size, size_t _Count, FILE * _File);
int  g_getc(FILE * _File);
int  g_getw(FILE * _File);
int  g_ungetc(int _Ch, FILE * _File);

int  g_pclose(FILE * _File);
FILE *  g_popen(const gchar * _Command, const gchar * _Mode);

int  g_putc(int _Ch, FILE * _File);
int  g_putw(int _Word, FILE * _File);
void  g_rewind(FILE * _File);
void  g_setbuf(FILE * _File, gchar * _Buffer);
int  g_setvbuf(FILE * _File, gchar * _Buf, int _Mode, size_t _Size);
FILE *  g_tmpfile(void);
int  g_fprintf_p(FILE * _File, const gchar * _Format, ...);
int  g_vfprintf_p(FILE * _File, const gchar * _Format, va_list _ArgList);


int  g_fclose_nolock(FILE * _File);
int  g_fflush_nolock(FILE * _File);
size_t  g_fread_nolock(void * _DstBuf, size_t _ElementSize, size_t _Count, FILE * _File);
size_t  g_fread_nolock_s(void * _DstBuf, size_t _DstSize, size_t _ElementSize, size_t _Count, FILE * _File);
int  g_fseek_nolock(FILE * _File, long _Offset, int _Origin);
long  g_ftell_nolock(FILE * _File);
int  g_fseeki64_nolock(FILE * _File, gint64 _Offset, int _Origin);
gint64  g_ftelli64_nolock(FILE * _File);
size_t  g_fwrite_nolock(const void * _DstBuf, size_t _Size, size_t _Count, FILE * _File);
int  g_ungetc_nolock(int _Ch, FILE * _File);


FILE* g_stdout();
FILE* g_stdin();
FILE* g_stderr();




struct utimbuf;			/* Don't need the real definition of struct utimbuf when just
				 * including this header.
				 */

int g_utime     (const gchar    *filename,
		 struct utimbuf *utb);

#endif /* G_OS_UNIX */

G_END_DECLS

#endif /* __G_STDIO_H__ */

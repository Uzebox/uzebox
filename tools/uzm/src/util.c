/*
 * uzm -- small portable helpers: errors, memory, files with UTF-8 names (Windows included),
 * 64-bit offsets, latin-1 for the header's text.
 */
#ifndef _WIN32
#define _FILE_OFFSET_BITS 64
#endif
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "uzm.h"

#ifdef _WIN32
#include <windows.h>
#endif

void die(const char *fmt, ...)
{
	va_list ap;
	fflush(stdout);
	fputs("uzm: ", stderr);
	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(1);
}

void *xmalloc(size_t n)
{
	void *p = malloc(n ? n : 1);
	if (!p) die("out of memory (%llu bytes)", (unsigned long long)n);
	return p;
}

void *xrealloc(void *p, size_t n)
{
	p = realloc(p, n ? n : 1);
	if (!p) die("out of memory (%llu bytes)", (unsigned long long)n);
	return p;
}

char *xstrdup(const char *s)
{
	size_t n = strlen(s) + 1;
	return memcpy(xmalloc(n), s, n);
}

#ifdef _WIN32
static wchar_t *wide(const char *s)
{
	int n = MultiByteToWideChar(CP_UTF8, 0, s, -1, NULL, 0);
	wchar_t *w = xmalloc(n * sizeof *w);
	MultiByteToWideChar(CP_UTF8, 0, s, -1, w, n);
	return w;
}
#endif

FILE *ufopen(const char *path, const char *mode)
{
#ifdef _WIN32
	wchar_t *wp = wide(path), *wm = wide(mode);
	FILE *f = _wfopen(wp, wm);
	free(wp);
	free(wm);
	return f;
#else
	return fopen(path, mode);
#endif
}

int ufseek(FILE *f, int64_t off, int whence)
{
#ifdef _WIN32
	return _fseeki64(f, off, whence);
#else
	return fseeko(f, (off_t)off, whence);
#endif
}

int64_t uftell(FILE *f)
{
#ifdef _WIN32
	return _ftelli64(f);
#else
	return (int64_t)ftello(f);
#endif
}

int64_t ufsize(const char *path)
{
#ifdef _WIN32
	struct _stat64 st;
	wchar_t *wp = wide(path);
	int r = _wstat64(wp, &st);
	free(wp);
	return r ? -1 : (int64_t)st.st_size;
#else
	struct stat st;
	return stat(path, &st) ? -1 : (int64_t)st.st_size;
#endif
}

const char *base_name(const char *path)
{
	const char *b = path;
	for (const char *p = path; *p; p++)
		if (*p == '/'
#ifdef _WIN32
		    || *p == '\\' || *p == ':'
#endif
		   )
			b = p + 1;
	return b;
}

/* UTF-8 to latin-1, at most cap - 1 characters, '?' for code points above 255 (Python's
 * encode("latin-1", "replace")); a malformed byte counts as one character, as '?' */
void to_latin1(char *dst, size_t cap, const char *s)
{
	const unsigned char *u = (const unsigned char *)s;
	size_t n = 0;
	while (*u && n + 1 < cap) {
		unsigned cp;
		int len;
		if (u[0] < 0x80) { cp = u[0]; len = 1; }
		else if ((u[0] & 0xE0) == 0xC0 && (u[1] & 0xC0) == 0x80) {
			cp = (u[0] & 0x1F) << 6 | (u[1] & 0x3F); len = 2;
		} else if ((u[0] & 0xF0) == 0xE0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80) {
			cp = 0x100; len = 3;
		} else if ((u[0] & 0xF8) == 0xF0 && (u[1] & 0xC0) == 0x80 && (u[2] & 0xC0) == 0x80 &&
		           (u[3] & 0xC0) == 0x80) {
			cp = 0x100; len = 4;
		} else { cp = 0x100; len = 1; }
		dst[n++] = (char)(cp < 0x100 ? cp : '?');
		u += len;
	}
	dst[n] = 0;
}

void buf_add(Buf *b, const void *p, size_t n)
{
	if (b->n + n > b->cap) {
		b->cap = (b->n + n) * 2 + 65536;
		b->p = xrealloc(b->p, b->cap);
	}
	memcpy(b->p + b->n, p, n);
	b->n += n;
}

void buf_free(Buf *b)
{
	free(b->p);
	b->p = NULL;
	b->n = b->cap = 0;
}

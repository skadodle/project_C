#ifndef HIRSCHBERG
#define HIRSCHBERG
#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "format.h"

#define SK_ST(file) fseek(file, 0, SEEK_SET)
#define SK_ED(file) fseek(file, 0, SEEK_END)

typedef int (*costfunc)(char, char);

static int
levenshtein(char a, char b)
{
	return a != b;
}


typedef enum {
	Del = 0,
	Sub = 1,
	Ins = 2,
} editop;


char		*hirschberg(const char *, const char*, costfunc);
static char	*hirschberg_recursive(char *, const char *, size_t, const char *, size_t, costfunc);
static char	*nwalign(char *, const char *, size_t, const char *, size_t, costfunc);
static void	nwlcost(int *, const char *, size_t, const char *, size_t, costfunc);
static void	nwrcost(int *, const char *, size_t, const char *, size_t, costfunc);
static editop	nwmin(int[3], char, char, costfunc);
static void	memrev(void *, size_t);
static void	*tryrealloc(void *, size_t);

bool file_to_str_iscorrect(FILE* file);
float CompareFiles(char* fname1, char* fname2, bool fullCompare);

#endif
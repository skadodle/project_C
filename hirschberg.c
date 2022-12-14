#include "hirschberg.h"

char *
hirschberg(const char *a, const char *b, costfunc f)
{
	char *c, *d;
	size_t m = strlen(a);
	size_t n = strlen(b);
	
	
	c = malloc(m+n+1);
	if (c == NULL)
		return NULL;
	if (m > n) {
		d = hirschberg_recursive(c, b, n, a, m, f);
		c = tryrealloc(c, d-c+1);
		for (d = c; *d != '\0'; d++)
			switch (*d) {
			case '+': *d = '-'; break;
			case '-': *d = '+'; break;
			}
		return c;
	}
	d = hirschberg_recursive(c, a, m, b, n, f);
	return tryrealloc(c, d-c+1);
}

static char *
hirschberg_recursive(char *c, const char *a, size_t m, const char *b, size_t n, costfunc f)
{
	if (n > 1) {
		size_t i, mmid, nmid;
		int lcost[m+1], rcost[m+1], tcost[m+1];
		
		nmid = n / 2;
		nwlcost(lcost, a, m, b, nmid, f);
		nwrcost(rcost, a, m, b+nmid, n-nmid, f);
		
		mmid = 0;
		for (i = 0; i <= m; i++) {
			tcost[i] = lcost[i] + rcost[i];
			if (tcost[i] < tcost[mmid])
				mmid = i;
		}
		
		c = hirschberg_recursive(c, a, mmid, b, nmid, f);
		c = hirschberg_recursive(c, a+mmid, m-mmid, b+nmid, n-nmid, f);
		return c;
	} else
		return nwalign(c, a, m, b, n, f);
}

static char *
nwalign(char *c, const char *a, size_t m, const char *b, size_t n, costfunc f)
{
	char *d;
	size_t i, j;
	int s[m+1][n+1];
	
	s[0][0] = 0;
	for (i = 1; i <= m; i++)
		s[i][0] = s[i-1][0] + f(a[i-1], 0);
	for (j = 1; j <= n; j++)
		s[0][j] = s[0][j-1] + f(0, b[j-1]);
	for (j = 1; j <= n; j++)
		for (i = 1; i <= m; i++) {
			int cost[3] = { s[i-1][j], s[i-1][j-1], s[i][j-1] };
			s[i][j] = cost[nwmin(cost, a[i-1], b[j-1], f)];
		}
	i = m;
	j = n;
	d = c;
	while (i > 0 && j > 0) {
		int cost[3] = { s[i-1][j], s[i-1][j-1], s[i][j-1] };
		switch (nwmin(cost, a[i-1], b[j-1], f)) {
		case Del:
			*d++ = '-';
			i--;
			break;
		case Sub:
			*d++ = a[i-1] == b[j-1] ? '=' : '!';
			i--;
			j--;
			break;
		case Ins:
			*d++ = '+';
			j--;
			break;
		}
	}
	for (; i > 0; i--)
		*d++ = '-';
	for (; j > 0; j--)
		*d++ = '+';
	*d = '\0';
	memrev(c, d-c);
	return d;
}

static void
nwlcost(int *s, const char *a, size_t m, const char *b, size_t n, costfunc f)
{
	size_t i, j;
	int ss, tmp;
	
	s[0] = 0;
	for (i = 1; i <= m; i++)
		s[i] = s[i-1] + f(a[i-1], 0);
	for (j = 1; j <= n; j++) {
		ss = s[0];
		s[0] += f(0, b[j-1]);
		for (i = 1; i <= m; i++) {
			int cost[3] = { s[i-1], ss, s[i] };
			tmp = cost[nwmin(cost, a[i-1], b[j-1], f)];
			ss = s[i];
			s[i] = tmp;
		}
	}
}

static void
nwrcost(int *s, const char *a, size_t m, const char *b, size_t n, costfunc f)
{
	ssize_t i, j;
	int ss, tmp;
	
	s[m] = 0;
	for (i = m-1; i >= 0; i--)
		s[i] = s[i+1] + f(a[i], 0);
	for (j = n-1; j >= 0; j--) {
		ss = s[m];
		s[m] += f(0, b[j]);
		for (i = m-1; i >= 0; i--) {
			int cost[3] = { s[i+1], ss, s[i] };
			tmp = cost[nwmin(cost, a[i], b[j], f)];
			ss = s[i];
			s[i] = tmp;
		}
	}
}

static editop
nwmin(int cost[3], char a, char b, costfunc f)
{
	size_t i;
	
	cost[Del] += f(a, 0);
	cost[Sub] += f(a, b);
	cost[Ins] += f(0, b);
	i = cost[Del] < cost[Sub] ? Del : Sub;
	i = cost[i] < cost[Ins] ? i : Ins;
	return i;
}

static void
memrev(void *ptr, size_t size)
{
	char *p = (char *)ptr;
	size_t i;
	char tmp;
	
	for (i = 0; i < size/2; i++) {
		tmp = p[i];
		p[i] = p[size-1-i];
		p[size-1-i] = tmp;
	}
}

static void *
tryrealloc(void *ptr, size_t size)
{
	void *b;
	
	b = realloc(ptr, size);
	return b != NULL ? b : ptr;
}

bool file_to_str_iscorrect(FILE* file){
	char char_rw;
	
	while ((char_rw = fgetc(file)) != EOF)
		if (char_rw == '\'' || char_rw == '\"' || char_rw == '\t' || char_rw == '\n' || char_rw == ';' || char_rw == ':' || char_rw == '{') 
			return false;
	return true;
}

float CompareFiles(char* fname1, char* fname2, bool fullCompare){
	char *align, *c;
	char char_rw;

	float res = 0;

	int count_matches = 0;
	int count_all = 0;
	
	FILE* first;
	FILE* second;

	if ((first = fopen(fname1, "r")) == NULL){
		printf("Проблема с открытием файла: %s.\n", fname1);
		return -1;
	}
	

	if ((file_to_str_iscorrect(first)) == false){
		fclose(first);
		Formatfile(fname1, "out.txt");
		if ((first = fopen("out.txt", "r")) == NULL){
			return -1;
		}
	}		

	if ((second = fopen(fname2, "r")) == NULL){
		printf("Проблема с открытием файла: %s.\n", fname2);
		return -1;
	}
	

	if ((file_to_str_iscorrect(second)) == false){
		fclose(second);
		Formatfile(fname2, "out2.txt");
		if ((second = fopen("out2.txt", "r")) == NULL){
			return -1;
		}
	}

	SK_ED(first);
	SK_ED(second);

	size_t size_first = ftell(first);
	size_t size_second = ftell(second);

	SK_ST(first);
	SK_ST(second);

	unsigned int iter = 0;
	
	char *a = (char*)malloc(size_first * sizeof(char));
	char *b = (char*)malloc(size_second * sizeof(char));
	

	while ((char_rw = fgetc(first)) != EOF)
		a[iter++] = char_rw;

	iter = 0;
	
	while ((char_rw = fgetc(second)) != EOF)
		b[iter++] = char_rw;
		
	for (int i = 0; i <= fullCompare; i++){
		count_matches = 0;
		count_all = 0;

		if (i == 0)	
			align = hirschberg(a, b, levenshtein);
		if (i == 1)
			align = hirschberg(b, a, levenshtein);

		if (align == NULL)
			return -2;

		for (c = align; *c != '\0'; c++)
			switch (*c) {
			case '-':
			case '!':
			case '=':
				if (*c == '=') count_matches++;
					count_all++;
				break;
			default:
				break;
			}
			
		res += (float)count_matches / count_all;
	}

	res *= 100;

	if (fullCompare)
		res /= 2;

	fclose(first);
	fclose(second);

	free(align);

	free(a);
	free(b);

	return res;
}

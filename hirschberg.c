#include <sys/types.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "format.h"



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

#define SK_ST(file) fseek(file, 0, SEEK_SET)
#define SK_ED(file) fseek(file, 0, SEEK_END)

void man(){
	printf("\n\tПособие по использованию программы:\n\tДля запуска используйте следующую структуру: ./start file1 file2 [-f]");
	printf("\n\tДополнительные параметры: [-f] для сравнения file1 file2 и file2 file1 после чего выводится среднее значение.\n");
	//printf("\n\tПосле проверки если итоговое значение > 60%c, то программа списана, возможно с заменой переменных.\n\n", '%');
}

char file_to_str_iscorrect(FILE* file){
	char char_rw;
	
	while ((char_rw = fgetc(file)) != EOF)
		if (char_rw == '\'' || char_rw == '\"' || char_rw == '\t' || char_rw == '\n' || char_rw == ';' || char_rw == ':' || char_rw == '{') 
			return '0'; // -1
	return '1';
}

int
main(int argc, char *argv[]) {

	if (argc == 1) {
		man();
		return 0;
	}

	if (argc < 3){
		printf("%s!\n", "Dont have enough files to start");
		return -1;
	}

	char *align, *c;
	char char_rw;

	float res = 0;

	short flag = (argc == 4 && (strcmp(argv[3], "-f") == 0));
	if (flag)
		printf("\tФлаг -f обнаружен\n");
	else 
		printf("\tФлаг -f не обнаружен\n");

	int count_matches = 0;
	int count_all = 0;
	
	FILE* first;
	FILE* second;

	if ((first = fopen(argv[1], "r")) == NULL)
		return -1;
	
	printf("\tПервый файл успешно открыт\n");

	if ((file_to_str_iscorrect(first)) == '0'){
		fclose(first);
		Formatfile(argv[1], "out.txt");
		printf("\tТокенизация для первого файла успешна\n");
		if ((first = fopen("out.txt", "r")) == NULL)
			return -1;
	}		

	if ((second = fopen(argv[2], "r")) == NULL)
		return -1;
	
	printf("\tВторой файл успешно открыт\n");

	if ((file_to_str_iscorrect(second)) == '0'){
		fclose(second);
		Formatfile(argv[2], "out2.txt");
		printf("\tТокенизация для второго файла успешна\n");
		if ((second = fopen("out2.txt", "r")) == NULL)
			return -1;
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
		
	for (int i = 0; i <= flag; i++){
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
		if (i == 0)
			printf("\tСравнение file1 file2\n");
		if (i == 1)
			printf("\tСравнение file2 file1\n");
	}

	res *= 100;

	if (flag)
		res /= 2;
	
	printf("\tPlagiat = %.2f%c\n", res, '%');
	
	printf("\tПрограмма завершила выполнение, файлы сравнены, их токенизированная форма в файлах: out.txt и out2.txt\n");

	fclose(first);
	fclose(second);

	free(align);

	free(a);
	free(b);

	return 0;
}

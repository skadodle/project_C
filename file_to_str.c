#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
	char c;
	FILE* fptr;

	if ((fptr = fopen(argv[1], "r")) == NULL)
		return -1;
	FILE* temp = tmpfile();
	
	while((c = getc(fptr)) != EOF)
		if (c != '\t' /*&& c != ' '*/ && c != '\n')
			fprintf(temp, "%c", c);

	fclose(fptr);
	fseek(temp, 0, SEEK_SET);

	if ((fptr = fopen(argv[1], "w")) == NULL)
		return -1;

	
	while((c = getc(temp)) != EOF)
		fprintf(fptr, "%c", c);
	
	fprintf(fptr, "\n");

	fclose(fptr);
	fclose(temp);

	return 0;
}

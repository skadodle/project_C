#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
	
	char c;
	char prev_c;
	char flag = '0';
	FILE* fptr;

	if ((fptr = fopen(argv[1], "r")) == NULL)
		return -1;
	FILE* temp = tmpfile();
	
	while((c = getc(fptr)) != EOF){                                                                       
      		if (flag != '1' && c != '{' && c != '}' && c != '\'' && c != '\"' && c != '(' && c != ')' && c != '['     && c != ']' && c != '\t' && c != '\n' && c != ',' && c != '.' && c != '/' && c != '\\')
 			fprintf(temp, "%c", c);                                                               
                                                                                                                
 		if (c == '/' && prev_c == '/')                                                                
        		flag = '1';                                                                           
                                                                                                                
        	if (c == '\n')                                                                                
        		flag = '0';                                                                           
                                                                                                                
        	prev_c = c;                                                                                   
	}   

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

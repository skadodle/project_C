#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void CopyFile(FILE* from, FILE* to){
    char w;
    while((w = fgetc(from)) != EOF){
        fputc(w, to);
    }
}


void RemoveComments(char* filename){
    
    char letter;
    char prev_letter = '\0';
    bool inLComment = false;
    bool inMLComment = false;
    
    FILE* file = fopen(filename, "r");
    FILE* tmp = tmpfile();

    while((letter = fgetc(file)) != EOF){
        
        if(inMLComment){
            if(letter == '/' && prev_letter == '*'){
                inMLComment = false;
                prev_letter = '\0';
            }
            continue;
        }
        if (inLComment){
            if(letter == '\n'){
                inLComment = false;
                prev_letter = '\0';
            }
            continue;
        }

        if (letter == '/' && prev_letter == '/'){
            inLComment = true;
            continue;
        }
        if (letter == '*' && prev_letter == '/'){
            inMLComment = true;
            continue;
        }
        prev_letter = letter;

        if(prev_letter != '\0'){
            printf("%c", prev_letter);
            fputc(prev_letter, tmp);
        }
    }
    if(prev_letter != '\0'){
        printf("%c", prev_letter);
        fputc(prev_letter, tmp);
    }
    fclose(file);
    file = fopen(filename, "w");
    CopyFile(tmp, file);
    fclose(file);
}


void ReadElement(FILE* input){

    char word[128];
    char letter;

    while((letter = fgetc(input)) != EOF){
        printf("%s\n", word);
    }

}

int main(int argc, char* argv[])
{
    RemoveComments("formatc.c");
    return 0;

}




#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

void CopyFile(FILE* from, FILE* to){
    fseek(from, 0, SEEK_SET);
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

        if(inLComment){
            if(letter == '\n'){
                inLComment = false;
                prev_letter = '\0';
            }
            continue;
        }
        if(inMLComment){
            if(letter == '/' && prev_letter == '*'){
                inMLComment = false;
                prev_letter = '\0';
                continue;
            }
            prev_letter = letter;
            continue;
        }

        if(letter == '/' && prev_letter == '/'){
            inLComment = true;
            continue;
        }
        if(letter == '*' && prev_letter == '/'){
            inMLComment = true;
            continue;
        }

        if(prev_letter != '\0')
            fputc(prev_letter, tmp);
        
        prev_letter = letter;
    }
    if(prev_letter != '\0')
        fputc(prev_letter, tmp);
    
    fclose(file);
    file = fopen(filename, "w");
    CopyFile(tmp, file);
    fclose(file);
    fclose(tmp);
}

void RemoveSymbols(char* filename, char symbols[], size_t symbolsCount){
    FILE* file = fopen(filename, "r");
    char letter;
    FILE* tmp = tmpfile();

    while((letter = fgetc(file)) != EOF){
        bool hit = false;
        for(size_t i = 0; i < symbolsCount; i++){
            if(letter == symbols[i]){
                fputc('\n', tmp);
                hit = true;
                continue;
            }
        }
        if (!hit)
            fputc(letter, tmp);
    }

    fclose(file);
    file = fopen(filename, "w");
    CopyFile(tmp, file);
    fclose(file);
    fclose(tmp);
}

void RemoveSpaces(char* filename){

    FILE* file = fopen(filename, "r");

    char letter;
    char prev_letter = '\0';

    FILE* tmp = tmpfile();

    bool nline = false;


    while((letter = fgetc(file)) != EOF){


        if(letter == '\n' && !nline){
            nline = true;
            fputc(letter, tmp);
            prev_letter = letter;
            continue;
        }

        if (nline){
            if (letter == '\n' || letter == ' ' || letter == '\t'){
                continue;
            }
            else{
                nline = false;
            }
        }
        else if(letter == prev_letter){
            if(letter == '\n' || letter == ' ' || letter == '\t'){
                continue;
            }
        }
        fputc(letter, tmp);
        prev_letter = letter;    
    }

    fclose(file);
    file = fopen(filename, "w");
    CopyFile(tmp, file);
    fclose(file);
    fclose(tmp);

}

void ReadElement(char* filename){

    FILE* file = fopen(filename, "r");

    char word[128];
    char letter;

    while((letter = fgetc(file)) != EOF){
        printf("%s\n", word);
    }

    fclose(file);
}


int main(int argc, char* argv[])
{
    RemoveComments("formatc.c");
    
    const size_t SIZE = 7;
    char symbols[SIZE] = {'{', '}', '(', ')', ';', '=', ','};
    RemoveSymbols("formatc.c", symbols, SIZE);

    RemoveSpaces("formatc.c");
    return 0;

}
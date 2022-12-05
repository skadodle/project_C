#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAXTOKENS 500
#define MAXTOKENLENGHT 3
#define MAXSYMBOLS 32

int MAXTOKENID = 0;


typedef struct{
    char name[MAXSYMBOLS];
    int id;
} Token;

void SetToken(Token* token){
    token -> id = MAXTOKENID;
    MAXTOKENID++;
}

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

void RemoveSymbols(char* filename, char symbols[], size_t symbolsCount, bool tossOnNewLine){
    FILE* file = fopen(filename, "r");
    char letter;
    FILE* tmp = tmpfile();

    while((letter = fgetc(file)) != EOF){
        bool hit = false;
        for(size_t i = 0; i < symbolsCount; i++){
            if(letter == symbols[i]){
                if(tossOnNewLine)
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

char* FormatName(char* name){
    char* newname = strtok(name, " \n");
    
    if(newname == NULL)
        return name;
    return newname;
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

bool isVallidSymbol(char c){
    if(c >= 'A' && c <= 'Z')
        return true;
    if(c >= 'a' && c <= 'z')
        return true;
    if(c >= '0' && c <= '9')
        return true;
    if(c == '_' || c == '[' || c == ']' || c == '\n')
        return true;
    return false;
}

char* CheckOnTypeDeclaration(char* line){
    const size_t SIZE = 9;
    char types[SIZE][15] = {"char", "int", "long", "short", "float", "double", "void", "size_t", "ssize_t"};

    char* word = strtok(line, " ");
    bool hit = false;

    while( word != NULL ){
        if(hit) {
            for (size_t i = 0; i < SIZE; i++){
                if (strcmp(word, types[i]) == 0){
                    continue;        
                }
            }

            size_t len = strlen(word);
            for (size_t i = 0; i < len; i++){
                if(!isVallidSymbol(word[i])){
                    printf("Alarm! That is not a type name: %s, error: %c\n", word, word[i]);
                    exit(-1);
                }
            }

            //printf("This is string with type: %s, type name: %s\n", line, word);
            return word; // return неуместен т.к. дальше может быть еще несколько обьявлений типов
            
        }
        
        for (size_t i = 0; i < SIZE; i++){
            if (strcmp(word, types[i]) == 0){
                //printf("That a type: %s\n", word);
                hit = true;
                continue;        
            }
        }

        word = strtok(NULL, " ");
    }

    return "";
}

bool isValidTypename(char* typename){
    if(strcmp(typename, "") == 0)
        return false;
    if(strcmp(typename, "\n") == 0)
        return false;
    if(strcmp(typename, " ") == 0)
        return false;
    if(strcmp(typename, "main") == 0)
        return false;
    return true;
}

int isInTypenames(char* name, Token tokens[], size_t tokenscount){
    for (size_t i = 0; i < tokenscount; i++)
    {
        if(strcmp(name,tokens[i].name) == 0){
            return i;
        }
    }
    return -1;
}

int AddToken(char* name, Token tokens[], size_t tokenscount){
    int pos = isInTypenames(name, tokens, tokenscount);

    if(pos != -1){
        strcpy(tokens[pos].name, name);
        SetToken(&tokens[pos]);
        return 1;
    } else if(tokenscount < MAXTOKENS){
        strcpy(tokens[tokenscount].name, name);
        SetToken(&tokens[tokenscount]);
        return 0;
    } else {
        printf("Alarm! Not enought MAXNAMES, current count of names: %d, name that stopped program: %s\n",
                (int)tokenscount, name);
        return -1;
    }
    
}

void TokeniseLine(char* line, Token tokens[], size_t tokenscount){
    size_t lenght = strlen(line);

    char* buffer = (char*)malloc(sizeof(char) * (lenght+MAXTOKENLENGHT));

    char* token = strtok(line, " ");
   
    while( token != NULL ) {
        printf("1: %s\n", buffer);
        bool tokened = false;
        for (size_t i = 0; i < tokenscount; i++){
            if(strcmp(FormatName(token), tokens[i].name)){
                char tmptoken[MAXTOKENLENGHT];
                sprintf(tmptoken, "%d", tokens[i].id);
                strcat(buffer, tmptoken);
                tokened = true;
                break;
            }
        }
        if(!tokened){
            strcat(buffer, token);
        }

        strcat(buffer, " ");
        token = strtok(NULL, " ");
    }

    strcpy(line, buffer);
    free(buffer);
}

void TokeniseFile(char* filename){
    FILE * file = fopen(filename, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    Token tokens[MAXTOKENS];

    size_t tokenscount = 0;

    while ((read = getline(&line, &len, file)) != -1) {
        char* typename = CheckOnTypeDeclaration(strdup(line));

        int status = -2;

        if(isValidTypename(typename)){
            FormatName(typename);
            //printf("%s\n", typename);
            
            int status = AddToken(typename, tokens, tokenscount);
            if (status == 0){
                tokenscount++;
            }
            else if (status == -1){
                exit(-1);
            }
        }

        TokeniseLine(line, tokens, tokenscount);
        //printf("%s\n", line);

    }

    printf("----------------\n");

    for (size_t i = 0; i < tokenscount; i++)
    {
        //printf("%s%d\n", tokens[i].name, tokens[i].id);
    }
    

    fclose(file);
    free(line);
}

int main(int argc, char* argv[])
{

    RemoveComments("formatc.c");
    
    const size_t SIZE1 = 7;
    char symbols1[SIZE1] = {'{', '}', '(', ')', ';', '=', ','};
    RemoveSymbols("formatc.c", symbols1, SIZE1, true);

    const size_t SIZE2 = 1;
    char symbols2[SIZE2] = {'*'};
    RemoveSymbols("formatc.c", symbols2, SIZE2, false);

    RemoveSpaces("formatc.c");

    TokeniseFile("formatc.c");

    return 0;

}
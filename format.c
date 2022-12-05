#include "format.h"

int MAXTOKENID = 0;

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
    char* newname = strtok(name, "[] \n");
    
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
    char types[9][15] = {"char", "int", "long", "short", "float", "double", "void", "size_t", "ssize_t"};

    char* word = strtok(line, " ");
    bool hit = false;

    while( word != NULL ){
        if(hit) {
            for (size_t i = 0; i < 9; i++){
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
        
        for (size_t i = 0; i < 9; i++){
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

bool isValidTypename(char* name){
    if(strcmp(name, "") == 0)
        return false;
    if(strcmp(name, "\n") == 0)
        return false;
    if(strcmp(name, " ") == 0)
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

void AddTokenizedLine(char* line, Token tokens[], size_t tokenscount, FILE* file){
    char* token = strtok(line, " \n[]");
    while(token != NULL) {
        bool tokened = false;
        for (size_t i = 0; i < tokenscount; i++){
            if(strcmp(token, tokens[i].name) == 0){
                char tmptoken[MAXSYMBOLS];
                sprintf(tmptoken, "%d", tokens[i].id);
                fputs(tmptoken, file);
                tokened = true;
                break;
            }
        }
        if(!tokened){
            fputs(token, file);
        }

        fputc(' ', file);
        token = strtok(NULL, " \n[]");
    }
    fputc(' ', file);
}

void TokeniseFile(char* fromfilename, char* tofilename){
    FILE * file = fopen(fromfilename, "r");
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    Token tokens[MAXTOKENS];

    size_t tokenscount = 0;

    FILE* tfile = fopen(tofilename, "w");

    while ((read = getline(&line, &len, file)) != -1) {
        char* typename = CheckOnTypeDeclaration(strdup(line));
        int status = -2;

        if(isValidTypename(typename)){
            FormatName(typename);
            
            int status = AddToken(typename, tokens, tokenscount);
            if (status == 0){
                ++tokenscount;
            }
            else if (status == -1){
                printf("Alarm. Error status: -1");
                exit(-1);
            }
        }
        AddTokenizedLine(strdup(line), tokens, tokenscount, tfile);
    }
    fclose(file);
    fclose(tfile);
    free(line);
}

void Formatfile(char* filein, char* fileout){

    FILE* tmp = tmpfile();
    FILE* initial = fopen(filein, "r");

    CopyFile(initial, tmp);

    RemoveComments(filein);

    char symbols1[7] = {'{', '}', '(', ')', ';', '=', ','};
    RemoveSymbols(filein, symbols1, 7, true);

    char symbols2[1] = {'*'};
    RemoveSymbols(filein, symbols2, 1, false);
    RemoveSpaces(filein);

    TokeniseFile(filein, fileout);

    char symbols3[4] = {'\'','\"'};
    RemoveSymbols(fileout, symbols3, 4, false);

    RemoveSpaces(filein);
    
    initial = fopen(filein, "w");
    CopyFile(tmp, initial);
    fclose(tmp);
    fclose(initial);
    MAXTOKENID = 0;
}
#ifndef HEADER_FILE
#define HEADER_FILE

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define MAXTOKENS 500
#define MAXTOKENLENGHT 3
#define MAXSYMBOLS 32

typedef struct{
    char name[MAXSYMBOLS];
    int id;
} Token;

void SetToken(Token* token);

void CopyFile(FILE* from, FILE* to);

void RemoveComments(char* filename);

void RemoveSymbols(char* filename, char symbols[], size_t symbolsCount, bool tossOnNewLine);

char* FormatName(char* name);

void RemoveSpaces(char* filename);

bool isVallidSymbol(char c);

char* CheckOnTypeDeclaration(char* line);

bool isValidTypename(char* name);

int isInTypenames(char* name, Token tokens[], size_t tokenscount);

int AddToken(char* name, Token tokens[], size_t tokenscount);

void AddTokenizedLine(char* line, Token tokens[], size_t tokenscount, FILE* file);

void TokeniseFile(char* fromfilename, char* tofilename);

void Formatfile(char* filein, char* fileout);

 #endif
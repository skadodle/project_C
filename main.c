#include <dirent.h>
#include <regex.h>
#include "hirschberg.h"

#define MAXFILES 512

typedef struct {
    char* name;
    float percent; 
} np;

int compare(const void *a, const void *b) {
  
    np *npA = (np *)a;
    np *npB = (np *)b;
  
    return (npB->percent - npA->percent);
}

void man(){
	printf("\n\tПособие по использованию программы:\n\tДля запуска используйте следующую структуру: ./start file1 file2|dirname [-f]");
	printf("\n\tДополнительные параметры: [-f] для сравнения file1 file2 и file2 file1 после чего выводится среднее значение.");
	printf("\n\tПри сравнении с директорией выводится в порадке убывания все результаты вместе со значением антиплагиата.\n");
	//printf("\n\tПосле проверки если итоговое значение > 60%c, то программа списана, возможно с заменой переменных.\n\n", '%');
}

void PrintDiff(float percent, char* name1, char* name2){
    if(percent < 0)
        return;
    
    printf("Процент плагиата файла '%s' с '%s': %.1f\n", name1, name2, percent);
}

//TODO Remake with regular expressions
bool IsValidFilename(char* name){
    size_t len = strlen(name);
    for (size_t i = 0; i < len-2; i++)
    {
        if(!(isalpha(name[i]) || isdigit(name[i]) || isspace(name[i]) || name[i] == '_' || name[i] == '/')){
            return false;
        }
    }
    if(len > 1){
        if(name[len-2] == '.' && name[len-1] == 'c')
            return true;
    }
    return false;
}

//TODO Remake with regular expressions
bool IsValidDirname(char* name){
    size_t len = strlen(name);
    for (size_t i = 0; i < len; i++)
    {
        if(!(isalpha(name[i]) || isdigit(name[i]) || isspace(name[i]) || name[i] == '_' || name[i] == '/')){
            return false;
        }
    }
    return true;
}

//Return array of names and percent 
np* GetDirDiff(char* filename, char* dirname, bool fullCompare, size_t *size){
    static np res[MAXFILES]; 
    size_t count = 0;

    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            if (strcmp(dir->d_name, ".")  == 0 || strcmp(dir->d_name, "..") == 0)
                continue;
            
            char* sd = strdup(dirname);
            char* tfname = strdup(strcat(strcat(sd, "/"), dir->d_name));
            free(sd);
            res[count].name = strdup(tfname);

            if(IsValidFilename(tfname)){
                res[count].percent = CompareFiles(filename, tfname, fullCompare);
            }
            else{
                res[count].percent = -1;
            }
            count++;
        }
        closedir(d);
    }
    *size = count;
    return res;
}


void ParseArgs(int argc, char* argv[]){
    if (argc == 1){
        man();
    }
    else if (argc < 3) {
        printf("\tНеверный ввод!\n");
        man();
    }
    else if (argc >= 3 && IsValidFilename(argv[1]) && IsValidFilename(argv[2])){
        float percent = 0;
        if (argc == 4 && strcmp(argv[3], "-f") == 0)
            percent = CompareFiles(argv[1], argv[2], true);
        else
            percent = CompareFiles(argv[1], argv[2], false);
        PrintDiff(percent, argv[1], argv[2]);
    }
    else if (argc >= 3 && IsValidFilename(argv[1]) && IsValidDirname(argv[2])){
        np* res;
        size_t size;
        if (argc == 4 && strcmp(argv[3], "-f") == 0)
            res = GetDirDiff(argv[1],argv[2], true, &size);
        else
            res = GetDirDiff(argv[1],argv[2], false, &size);

        qsort(res, size, sizeof(np), compare);

        for (size_t i = 0; i < size; i++)
        {
            PrintDiff(res[i].percent, argv[1], res[i].name);
        }
    }
}

int main(int argc, char* argv[]){
    ParseArgs(argc, argv);
}

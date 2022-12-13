#include <dirent.h>
#include "hirschberg.h"

#define MAXFILES 512

typedef struct {
    char* name1;
    float percent; 
} np;

void man(){
	printf("\n\tПособие по использованию программы:\n\tДля запуска используйте следующую структуру: ./start file1 file2 [-f]");
	printf("\n\tДополнительные параметры: [-f] для сравнения file1 file2 и file2 file1 после чего выводится среднее значение.\n");
	//printf("\n\tПосле проверки если итоговое значение > 60%c, то программа списана, возможно с заменой переменных.\n\n", '%');
}

void PrintDiff(float percent){
    printf("Процент плагиата: %f \n", percent);
}

bool IsValidFilename(char* name){
    //TODO
    return true;
}

bool IsValidDirname(char* name){
    //TODO
    return true;
}

//Return array of names and percent 
np* Megafunction(char* filename, char* dirname, bool fullCompare, int *size){
    static np res[MAXFILES]; 
    int count = 0;

    DIR *d;
    struct dirent *dir;
    d = opendir(dirname);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            char* tfname = strcat(strcat(dirname, "/"), dir->d_name);
            printf("%s\n", tfname);
            res[count].name1 = strdup(tfname);
            res[count].percent = CompareFiles(filename, tfname, fullCompare);
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
        if (argc == 4 && strcmp(argv[3], "-f") == 0)
            PrintDiff(CompareFiles(argv[1], argv[2], true));
        else
            PrintDiff(CompareFiles(argv[1], argv[2], false));
    }
    else if (argc >= 3 && IsValidFilename(argv[1]) && IsValidDirname(argv[2])){
        np* res;
        int size;
        if (argc == 4 && strcmp(argv[3], "-f") == 0)
            res = Megafunction(argv[1],argv[2], true, &size);
        else   
            res = Megafunction(argv[1],argv[2], false, &size);

        for (size_t i = 0; i < size; i++)
        {
            printf("n: %s p: %f \n", res[i].name1, res[i].percent);
        }
        
    }


}

int main(int argc, char* argv[]){
    ParseArgs(argc, argv);
}
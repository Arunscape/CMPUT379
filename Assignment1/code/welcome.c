#include <stdio.h>
#include <unistd.h>

void printDragon(){
    FILE *f;
    char c;
    f=fopen("dragon_welcome","r");

    while((c=fgetc(f))!=EOF){
        printf("%c",c);
    }

    fclose(f);
}
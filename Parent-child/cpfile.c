#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#define SIZ 128

//Copy from source file to target file. Adapted from The C Programming Language
//by Kernighan  and Ritchie

int main(int argc, const char *argv[]) {

    int f1 , f2;

    if (argc != 3){
        printf("Wrong number of command line arguments\n");
        exit(1);
        return 1;
    }

    if ((f1 = open(argv[1], O_RDONLY, 0)) == -1){
        printf("Can't open %s \n", argv[1]);
        return 2;
    }

    if((f2=creat(argv[2], 0644)) == -1){
        printf("Can't create %s \n", argv[2]);
        return 3;
    }

    char buff[SIZ];
    int n;

    while((n=read(f1, buff, SIZ)) > 0)
        if(write(f2, buff, n) != n){
            printf("Can't write file");
            close(f1);
            close(f2);
            return 4;
        }

    close(f1);
    close(f2);

    printf("Success! by Child\n");
        
    return 0;
}
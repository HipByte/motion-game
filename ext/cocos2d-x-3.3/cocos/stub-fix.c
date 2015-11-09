#include <stdio.h> 
#include <unistd.h> 
#include <string.h>
#include <stdlib.h>

int fputs$UNIX2003(const char *res1, FILE *res2){
    return fputs(res1,res2);
}

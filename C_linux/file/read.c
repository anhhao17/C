#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char *argv[]){

    int fd,ret;
    
    char *r=(char *)calloc(100,sizeof(char));
    //printf("%d\n",strlen(r));

    //fd=open("read.txt",O_CREAT|O_RDONLY);

    ret=read(0,r,100);
    printf("size read %d\n",ret);
    printf("read : %s",r);
    

    //close(fd);
    return 0;
}
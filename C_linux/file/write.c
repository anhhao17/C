#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#define buf_sz 16
int main(int argc,char *argv[]){

    int fd[2],ret;
    char *buf1="nguyen ngoc anh hao\n";
    char *buf2=(char *)calloc(100,sizeof(char));
    //char *r=(char *)calloc(100,sizeof(char));
    fd[0]=open("test.txt",O_RDWR);
    fd[1]=open("test.txt",O_RDWR);
    //fd[1]=open("test1.txt",O_CREAT|O_WRONLY);
    if(fd[0]<0){
        perror("open");
    }
    ret=write(fd[0],buf1,strlen(buf1));

    if(fd[1]<0){
        perror("write fd 1");
    }

    ret=write(2,buf2,read(fd[1],buf2,100));
    
    close(fd[0]);
    close(fd[1]);
    return 0;
}
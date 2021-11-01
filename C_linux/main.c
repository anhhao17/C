#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
int main(){

        int fd,ret,b;
        int a[10]={1,2,3,4,5,6,7,8,9,10};
        char buf[10];
        fd=open("seeking",O_RDWR|O_CREAT|O_APPEND,0600);
        if(fd<0){
                perror("open");
                exit(1);
        }
        ret=write(fd,(void*)a,sizeof(a));
        if(fd<0){
                perror("write");
                close(fd);
                exit(1);
        }
        ret=lseek(fd,3*sizeof(int),SEEK_SET);

        if(fd<0){
                perror("seek");
                close(fd);
                exit(1);
        }

        ret=read(fd,&b,sizeof(int));
        if(fd<0){
                perror("read");
                close(fd);
                exit(1);
        }
        printf("b = %d\n",b);
        close(fd);

        
        return 0;
}

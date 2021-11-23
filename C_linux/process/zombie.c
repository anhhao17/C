#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc,char *argv[]){

    pid_t p;
    p=fork();
    if(p==0){
        printf("Im a child proc have PID = %d\n",getpid());
        printf("My parent PID = %d\n",getppid());
    }else{
        
        printf("Im a parent pid = %d\n",getpid());
        printf("my child pid = %d\n",p);
        sleep(3);
    }
    return 0;
}
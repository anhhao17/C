#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *func(void* arg){
    printf("hello world\n");
    //pthread_exit(NULL);
    int *ptr=(int*)malloc(sizeof(int));
    *ptr=9;
    for(int i=0;i<5;i++){
        printf("* ptr= %d \n",++(*ptr));

    }
    pthread_exit(ptr);
}
int main(int argc,char *argv[]){

    int *res;
    pthread_t t1;
    pthread_create(&t1,NULL,&func,NULL);
    //pthread_exit(0);
    pthread_join(t1,(void*)&res);
    printf("res = %d\n",*res);
    return 0;
}

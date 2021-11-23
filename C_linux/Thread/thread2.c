#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *func(void* arg){
    pthread_exit(NULL);
}
int main(int argc,char *argv[]){

    
    pthread_t t1;
    while(1){
        int ret=pthread_create(&t1,NULL,&func,NULL);
        if(ret){
            perror("cannot allocated memory");
            return -1;
        }
        pthread_join(t1,NULL);
    }
    
    //pthread_exit(0);
   // pthread_join(t1,(void*)&res);
    printf("OKE\n");
    return 0;
}

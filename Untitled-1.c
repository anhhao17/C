#include <stdio.h>

int main(){
    int a = 2, b = 9;
    b=(~a&0xf);
    printf("%d %d\n",a,b);
    return 0;
}
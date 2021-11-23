#include <stdio.h>
#include <stdlib.h>

int main(){
	int *myarr = malloc(100*sizeof(int));
	for(int i=0;i<100;i++){
		myarr[i]=i;
	}
	free(myarr);
	printf("DONE\n");
	return 0;



}

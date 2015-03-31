#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(){
	int i;
	for (i=0;i<0;i++)
		printf("%d\n",i);
	printf("first loop's i:%d\n", i);
	for (i=0;i<2;i++)
		if (i==1) break;
	printf("%d\n",i );
}
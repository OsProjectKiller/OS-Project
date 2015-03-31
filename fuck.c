#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void extractname(char out[20],char in[100]){
    char *startpos,*endpos;
    int pos,i,startindex,endindex;
    startpos = strchr(in,'-');
    startindex = startpos - in;
    endpos = strchr(startpos,' ');
    endindex = endpos - startpos;
    strncpy(out, in+startindex, endindex);
}

int main(){
	char out[20];
	char input[] = "addAssignment -Adam 2014-2-3";
	extractname(out,input);
	printf("%s\n",out);
}
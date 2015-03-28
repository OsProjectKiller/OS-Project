#include <stdio.h>
#include <stdlib.h>
//DLLM
char ***createtimeslot(int x,int y,int z){
    int i, j, k;

    char *allElements = malloc(x * y * z * sizeof(char));
    char ***array3D = malloc(x * sizeof(char **));

    for(i = 0; i < x; i++)
    {
        array3D[i] = malloc(y * sizeof(char *));
        for(j = 0; j < y; j++)
        {
            array3D[i][j] = allElements + (i * y * z) + (j * z);
        }
    }
    free(allElements);
    return array3D;
}

void deallocatearr(char ***arr3D,int x){
    int i,j;
    for(i = 0; i < x; i++)
    {
        free(arr3D[i]);
    }
    free(arr3D);
}

int main(int argc, char *argv[])
{
    
    char ***timeslot = createtimeslot(300,20,20);
    timeslot[0][0] = "2014-3-2";
    timeslot[0] = "study 1830 2230";
    printf("%s %s\n",timeslot[0][0],timeslot[0][0]);
    deallocatearr(timeslot,300);
}


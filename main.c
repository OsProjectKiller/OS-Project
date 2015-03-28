#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

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
    int i;
    int pid;
    int p2c_fd[argc-1][2];
    int c2p_fd[argc-1][2];
    int childnum;
    char buf[80];
    char buf2[80];

    for (i = 0;i < argc-1; i++){ //creating pipe
        if (pipe(p2c_fd[i]) < 0 || pipe(c2p_fd[i]) < 0){
            printf("Cannot create pipe\n");
            exit(1);
        }
    }
    for (i = 0; i < argc-1; i++){
        pid = fork();
        if (pid == 0){
            childnum = i;
            break;
        }
    }

    if (pid == 0){ //child process
        read(p2c_fd[childnum][0],buf,100);
        printf("Child %d: %s\n", childnum, buf);
        exit(0);
    }else{ //parent
        char input[100];
        char end[10] = "endProgram";
        int count = 0;

        printf("~~Welcome to AMR~~\n");
        while(1){
            printf("Please enter appoinment:\n");
            scanf("%[^\n]",input);
            write(p2c_fd[1][1],input,100);
            if (strncmp(input,end,0) == 0)
                exit(0);
            printf("-> [Pending]");
        }
    }

    /*char ***calendar = createtimeslot(100,100,20);
    char *input;
    char *date[100]; //for storing the order of date
    int i,j;
    int currentapp[100]; //the current appoinments of each date
    int totaldate = 0; //the current total of date inputted
    int countdatepos = 0; //for counting the position of the date*/

    /*for (i=0;i<100;i++)
        date[i] = malloc(15);
    for (i=0;i<100;i++)
        currentapp[i] = 0;
    date[0] = "2014-3-12";
    totaldate++;
    date[1] = "2014-4-11";
    totaldate++;

    input = "2014-4-13";

    for (i=0; i<totaldate;i++){
        if (date[i] == input){
            countdatepos = i;
            break;
        }
    }

    if (i == totaldate){
        date[totaldate] = input;
        countdatepos = totaldate;
    }

    calendar[countdatepos][currentapp[countdatepos]] = "Study";
    currentapp[countdatepos]++;
    printf("%s\n",calendar[2][0]);
    printf("%d\n", currentapp[countdatepos]);*/

}


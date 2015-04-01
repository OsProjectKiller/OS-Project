#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

void extractname(char out[20],char in[80]){
    char *startpos,*endpos;
    int pos,i,startindex,endindex;
    startpos = strchr(in,'-');
    startindex = startpos - in;
    endpos = strchr(startpos,' ');
    endindex = endpos - startpos;
    strncpy(out, in+startindex+1, endindex-1);
}

void extractdate(char out[20],char in[80]){
    char *startpos,*endpos;
    int pos,i,startindex,endindex;
    startpos = strchr(in,'-');
    startindex = startpos - in;
    endpos = strchr(startpos,' ');
    endindex = endpos - startpos;
    strncpy(out, in+startindex+endindex+1, 10);
}

void extractbatchname(char out[20],char in[80],int max){
    char *startpos;
    int startindex;
    startpos = strchr(in,'-');
    startindex = startpos - in;
    strncpy(out,in+startindex+1,max-startindex-1);
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
            //printf("My child number is: %d\n",i);
            childnum = i;
            break;
        }
    }
    
    if (pid == 0){ //child process        
        close(p2c_fd[childnum][1]); //close parent out
        close(c2p_fd[childnum][0]); //close child in

        char *calendar[100][100];
        char input[20];
        char *date[100]; //for storing the order of date
        int j,n;
        int currentapp[100]; //the current appoinments of each date
        int totaldate=0; //the current total of date inputted
        int countdatepos=0; //for counting the position of the date*/

        for (i=0;i<100;i++)
            date[i] = malloc(20);
        for (i=0;i<100;i++)
            for (j=0;j<100;j++)
                calendar[i][j] = malloc(100);
        for (i=0;i<100;i++)
            currentapp[i] = 0;

        while ((n = read(p2c_fd[childnum][0],buf,80)) > 0){
            buf[n] = 0;
            extractdate(input,buf);
            //fprintf(stderr,"\"%s\"\n", input);
            if (totaldate == 0){
                strcpy(date[0],input);
                countdatepos = 0;
                totaldate = 1;
                printf("First Time\n");
            }else{
                printf("input:%s,totaldate:%d\n",input,totaldate);
                for (i=0; i<totaldate;i++){
                    if (strcmp(date[i],input) == 0){
                        countdatepos = i;
                        break;
                    }
                }
                printf("i:%d\n", i);
                if (i == totaldate){
                    strcpy(date[i],input);
                    countdatepos = i;
                    totaldate++;
                }
            }
            strcpy(calendar[countdatepos][currentapp[countdatepos]],buf);
            currentapp[countdatepos]++;
            printf("%d,%d\n",countdatepos,currentapp[countdatepos]);
            for (i = 0; i < totaldate; i++)
                for (j = 0;j<currentapp[i];j++)
                    printf("%d,%d:%s\n",i,j,calendar[i][j]);
        }

        close(p2c_fd[childnum][0]); //close parent int
        close(c2p_fd[childnum][1]); //close child out
    }else{ //parent
        for (i=0;i<argc-1;i++){
            close(p2c_fd[i][0]); //close parent in
            close(c2p_fd[i][1]); //close child out
        }
        char input[80];
        char tempname[20];
        char end[] = "endProgram";
        int count = 0;
        int tempnum,n;

        printf("~~Welcome to AMR~~\n");
        while(1){
            printf("Please enter appoinment:\n");
            n = read(STDIN_FILENO,buf,80);
            if (buf[0] == 'e') //Not yet complete
                exit(0);
            else if (buf[3] == 'B'){
                FILE *infilep;
                char rub;
                char storing[100];

                printf("-> [Pending]\n");
                buf[--n] = 0;
                extractbatchname(tempname,buf,n);
                infilep = fopen(tempname, "r");
                printf("[%s]\n", tempname);
                if (infilep == NULL){ //check for opening file error
                    printf("Error in opening input file\n");
                    exit(1);
                }
                while (fgets(buf,sizeof(buf),infilep)!=NULL){ //put the input to the array
                    buf[strlen(buf)-1] = 0;
                    extractname(tempname,buf);
                    for (i=1;i<=(argc-1);i++){
                        if (strcmp(argv[i],tempname)==0){
                            tempnum = i;
                            break;
                        }
                    }
                    write(p2c_fd[tempnum-1][1],buf,80);
                }
            }else{
                printf("-> [Pending]\n");
                if (n <= 0) break;
                buf[--n] = 0;
                extractname(tempname,buf);
                for (i=1;i<=(argc-1);i++){
                    if (strcmp(argv[i],tempname)==0){
                        tempnum = i;
                        break;
                    }
                }
            write(p2c_fd[tempnum-1][1],buf,n);
            }
        }
        for (i=0;i<argc-1;i++){
            close(p2c_fd[i][1]); //close parent out
            close(c2p_fd[i][0]); //close child in
        }
        wait(NULL);
    }
    exit(0);
}
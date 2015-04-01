#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
// Student ID: 13067887D
// Student Name: Lau Yi, Louis
int main(int argc, char *argv[])
{
	FILE *infilep, *outfilep;
	int pid;
	int i; //for-loop counter
	int j;
	int totalchild = atoi(argv[1]);
	int totalhandcard = atoi(argv[2]);
	int p2c_fd[totalchild-1][2]; //parent to children pipe
	int c2p_fd[totalchild-1][2]; //children to parent pipe
	int childnumber;
	int n;
	int begin;
	char trumpsuit;
	char buf[80];
	char buf2[80];
	char *card[52]; //input cards
	char rank[] = {
		'2','3','4','5','6','7','8','9','T','J','Q','K'
	};

	for (i = 0;i < totalchild; i++){ //creating pipe
		if (pipe(p2c_fd[i]) < 0 || pipe(c2p_fd[i]) < 0){
			printf("Cannot create pipe\n");
			exit(1);
		}
	}

	for (i = 0;i < totalchild; i++){ //forking children
		pid = fork();
		if (pid == 0){
			childnumber = i+1;
			break;
		}
	}

	if (pid == 0){ //child process
		int j;
		int count;
		char *handcard[totalhandcard];
		close(p2c_fd[childnumber-1][1]); //close parent out
		close(c2p_fd[childnumber-1][0]); //close child in
		pid = getpid();

		for (i = 0; i <totalhandcard; i++)
			handcard[i] = malloc(3);

		sprintf(buf,"%d",pid);
		write(c2p_fd[childnumber-1][1],buf,80); //write pid to parent
		count = 0;
		printf("Child %d, pid %d:", childnumber, pid);
		for (i = 0; i < totalhandcard; i++){
			read(p2c_fd[childnumber-1][0], handcard[i], 3);
			printf(" %s", handcard[i]);
			count++;
		}
		printf("\n");

		if(count == totalhandcard)
			write(c2p_fd[childnumber-1][1], "ready", 80);

		read(p2c_fd[childnumber-1][0],buf2,1);
		trumpsuit = buf2[0];
		close(p2c_fd[childnumber-1][0]); //close parent in
		close(c2p_fd[childnumber-1][1]); //close child out
	}else{ //parent process
		int check;
		for (i = 0;i < totalchild;i++){
			close(p2c_fd[i][0]); //close parent in
			close(c2p_fd[i][1]); //close child out
		}

		infilep = fopen(argv[3], "r");

		if (infilep == NULL){ //check for opening file error
		printf("Error in opening input file\n");
		exit(1);
		}

		for (i = 0;i < 52;i++)
			card[i] = malloc(3); //intital dynamic cards array
			
		i = 0;
		while (!feof(infilep)){ //put the input to the array
			fscanf(infilep, "%s ", card[i]);
			i++;
		}

		printf("Parent: the child players are");

		for (i = 0;i < totalchild;i++){
			read(c2p_fd[i][0],buf2,80); //read from child
			printf(" %s",buf2);
		}
		printf("\n");
		for (i = 0; i < totalchild;i++){ //distribute cards to children
			for (j = i; j < (totalchild*totalhandcard);j += (totalhandcard+1)){
				write(p2c_fd[i][1], card[j], 3);
			}
		}
		j = 0;
		for(i=0;i<totalchild;i++){
			if (check=(read(c2p_fd[i][0], buf, 80))> 0)
				j++;
		}
		if (j == totalchild){
			trumpsuit = card[totalhandcard*totalchild][0];
			printf("Parent: trump suit %c\n", trumpsuit);
			begin = totalhandcard % totalchild;
			if (begin == 0){
				begin = 1;
			}
			printf("Parent: child %d to play first\n", begin);
			for(i = 0; i < totalchild; i++) //write to child
				write(p2c_fd[i][1], (char*) &trumpsuit,1);
			wait(0);
		}
	}
}
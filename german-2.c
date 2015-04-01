#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[]) {
    int childnum;
	int childtotal = atoi(argv[1]);
	int roundtotal = atoi(argv[2]);
    char trumpsuit;
    int ptoc_fd[childtotal][2]; /* for parent-to-child pipes */
	int ctop_fd[childtotal][2]; /* for child-to-parent pipes */
	char buf[10], buf2[10];
    int i, j, k, pid, roundnum;
	char rank[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'k', 'A'};
	
	for(i = 0; i < childtotal; i++)
		if (pipe(ptoc_fd[i]) < 0 || pipe(ctop_fd[i]) < 0) {
			printf("Pipe creation error\n");
			exit(1);
		}
	for(i = 1; i <= childtotal; i++) {
		pid = fork();
		if(pid == 0) {
			childnum = i;
			break;
		}
	}
	if(pid == 0) { /* child */
		char *card[roundtotal];
		char winnersuit;
		char *temp;
		int temprank;
		pid = getpid();
		
		for(i = 0; i < roundtotal; i++)
			card[i] = malloc(3);
		
		close(ptoc_fd[childnum-1][1]);
		close(ctop_fd[childnum-1][0]);
		
		sprintf(buf2, "%d", pid);
		write(ctop_fd[childnum-1][1], buf2, 10);
		
		j = 0;
		printf("Child %d, pid %d:", childnum, pid);
		for(i = 0; i < roundtotal; i++) {
			read(ptoc_fd[childnum-1][0], card[i], 3);
			printf(" %s", card[i]);
			j++;
		}
		printf("\n");
		if(j == roundtotal)
			write(ctop_fd[childnum-1][1], "ready", 10);
		
		read(ptoc_fd[childnum-1][0], buf, 2);
		trumpsuit = buf[0];
		
		while(1) {
			read(ptoc_fd[childnum-1][0], buf, 2);
			temp = card[0];
			temprank = 13;
			if(atoi(buf) == childnum) {
				for(i = 0; i < roundtotal; i++)
					if(card[i][0] != trumpsuit)
						for(j = 0; j < 13; j++) {
							if(card[i][1] == rank[j])
								break;
							if(j < temprank) {
								temp = card[i];
								temprank = j;
							}
						}
				if(temp[0] == trumpsuit)
					for(i = 0; i < roundtotal; i++)
						for(j = 0; j < 13; j++) {
							if(card[i][1] == rank[j])
								break;
							if(j < temprank) {
								temp = card[i];
								temprank = j;
							}
						}
			}
			else {
				read(ptoc_fd[childnum-1][0], buf, 2);
				winnersuit = buf[0];
				for(i = 0; i < roundtotal; i++)
					if(card[i][0] == winnersuit)
						for(j = 0; j < 13; j++) {
							if(card[i][1] == rank[j])
								break;
							if(j < temprank) {
								temp = card[i];
								temprank = j;
							}
						}
				if(temp[0] != winnersuit) {
					temprank = -1;
					if(winnersuit == trumpsuit) {
						for(i = 0; i < roundtotal; i++)
							for(j = 0; j < 13; j++) {
								if(card[i][1] == rank[j])
									break;
								if( j > temprank) {
									temp = card[i];
									temprank = j;
								}
							}
					}
					else {
						for(i = 0; i < roundtotal; i++)
							if(card[i][0] != trumpsuit)
								for(j = 0; j < 13; j++) {
									if(card[i][1] == rank[j])
										break;
									if(j > temprank) {
										temp = card[i];
										temprank = j;
									}
								}
						if(temp[0] == trumpsuit) {
							temprank = 13;
							for(i = 0; i < roundtotal; i++)
								for(j = 0; j < 13; j++) {
									if(card[i][1] == rank[j])
										break;
									if(j < temprank) {
										temp = card[i];
										temprank = j;
									}
								}
						}
					}
				}
			}
			if(k=(read(ptoc_fd[childnum-1][0], buf, 10)) > 0) {
				write(ctop_fd[childnum-1][1], temp, 3);
				printf("Child %d: play %s\n", childnum, temp);
			}
			roundnum++;
			if(roundnum == roundtotal)
				break;		
		}
		
		close(ptoc_fd[childnum-1][0]);
		close(ctop_fd[childnum-1][1]);
	}
	else { /* parent */
		char *card[52];
		char *trick[childtotal];
		int leader, winner, winnerrank;
		int record[childtotal];
		
		for(i = 0; i < 52; i++) {
			card[i] = malloc(3);
			scanf("%s", card[i]);
		}
		
		for(i = 0; i < childtotal; i++)
			trick[i] = malloc(3);
		
		for(i = 0; i < childtotal; i++) {
			close(ptoc_fd[i][0]);
			close(ctop_fd[i][1]);
		}
		
		printf("Parent: the child players are");
		for(i = 0; i < childtotal; i++) {
			read(ctop_fd[i][0], buf, 10);
			printf(" %s", buf);
		}
		printf("\n");
		
		for(i = 0; i < childtotal; i++)
			for(j = i; j < childtotal*roundtotal; j+=childtotal)
				write(ptoc_fd[i][1], card[j], 3);
		
		j = 0;
		for(i = 0; i < childtotal; i++)
			if(k=(read(ctop_fd[i][0], buf, 10)) > 0)
				j++;
		if(j == childtotal) {
			trumpsuit = card[childtotal*roundtotal][0];
			winner = roundtotal%childtotal;
			printf("Parent: trump suit %c\n", trumpsuit);
			for(i = 0; i < childtotal; i++)
				write(ptoc_fd[i][1], (char*)&trumpsuit, 2);
			printf("Parent: child %d to play first\n", winner);
			j = winner;
			while(1) {
				if(j == winner) {
					sprintf(buf2, "%d", winner);
					for(i = 0; i < childtotal; i++)
						write(ptoc_fd[i][1], buf2, 2);
					write(ptoc_fd[j-1][1], "toplay", 10);
					if(k=(read(ctop_fd[j-1][0], trick[j-1], 3)) > 0)
						printf("Parent: child %d: play %s\n", j, trick[j-1]);
					for(i = 0; i < childtotal; i++)
						if(i != j-1)
						write(ptoc_fd[i][1],(char*)&trick[j-1][0],2);
				}
				else {
					write(ptoc_fd[j-1][1], "toplay", 10);
					if(k=(read(ctop_fd[j-1][0], trick[j-1], 3)) > 0)
						printf("Parent: child %d: play %s\n", j, trick[j-1]);
				}
				j++;
				if(j == roundtotal+1)
					j = 1;
				if(j == winner) {
					for(k = 0; k < 13; k++)
						if(trick[j-1][1] == rank[k]) {
							winnerrank = k;
							break;
						}
					for(i = 0; i < childtotal; i++)
						if(i != j-1) {
							if(trick[i][0] != trick[winner-1][0] && trick[i][0] == trumpsuit)
								winner = i+1;
							else {
								for(k = 0; k < 13; k++)
									if(trick[i][1] == rank[k])
										break;
									if(k > winnerrank) {
										winner = i+1;
										winnerrank = k;
									}
							}
						}
					printf("Parent: child %d wins the trick\n", winner);
					record[winner-1]++;
					j = winner;
					roundnum++;
					if(roundnum == roundtotal)
						break;
				}
			}
		}
		else {
			printf("Pipe communication error\n");
			exit(1);
		}
		
		printf("Parent: tricks won = <");
		for(i = 0; i < childtotal; i++)
			printf(" %d", record[i]);
		printf(">\n");
		
		for(i = 0; i < childtotal; i++) {
			close(ptoc_fd[i][1]);
			close(ctop_fd[i][0]);
		}
	}
}
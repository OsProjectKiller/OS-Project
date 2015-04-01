#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(){
	char card[100];
	int n;
	FILE *infilep;
	infilep = fopen("batch.dat", "r");

	while (fgets(card, sizeof(card),infilep) != NULL){ //put the input to the array
		card[strlen(card)-1] = 0;
		printf("%s\n", card);
	}
}
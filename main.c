#include <stdio.h>
#include <stdlib.h>
#include <string.h>

float score(int*, int, int);
int get_file_char_length(FILE*);

void main()
{
	int num_chars;

	FILE *fp = fopen("cipher.txt", "r");
	if (fp == NULL)
		exit(0);

	// Get number of ciphertext characters
	num_chars = get_file_char_length(fp);

	// Dynamically allocate a string to store ciphertext characters
	char *cipher = (char *)malloc((num_chars+1) * sizeof(char));

	// Store ciphertext from file
	fgets(cipher, (num_chars+1), fp);

	printf("%s\n", cipher);

	free(cipher);
}

int get_file_char_length(FILE *file)
{
	char nextChar = getc(file);
	int numCharacters = 0;

	while (nextChar != EOF) {
		
		numCharacters++;
		nextChar = getc(file);
	}

	// Rewind the file pointer to the start of file
	rewind(file);

	return numCharacters;
}
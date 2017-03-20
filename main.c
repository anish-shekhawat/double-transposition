#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void solveSecondTransposition(char *, char *);
void SortKey(int*, int*, char*);
char* BuildIntmdMatrix(char *, int*, int);

int main()
{
	solveSecondTransposition("RHOTNSIDTTREYEHNECIBXTPOCSIEYECLDTERTAOHEITUEISSPSNABRPT", "Secret");
	return 0;
}

void solveSecondTransposition(char *CipherText, char *KeyArray)
{
	int CipherTextLength = strlen(CipherText);
	int KeyLength = strlen(KeyArray);
	int LongColumnCount = CipherTextLength%KeyLength;
	int ShortColumnLength = CipherTextLength / KeyLength;

	int* KeyArrayVal;
	int* KeyIndexVal;
	KeyArrayVal = (int *)malloc(KeyLength * sizeof(int));
	KeyIndexVal = (int *)malloc(KeyLength * sizeof(int));
	// Check for errors
	if (KeyArrayVal == NULL || KeyIndexVal == NULL)
	{
		printf("Can't allocate memory!");
		exit(0);
	}

	SortKey(KeyArrayVal, KeyIndexVal, KeyArray);

	char* FirstMatrix = BuildIntmdMatrix(CipherText, KeyIndexVal, KeyLength);

	free(FirstMatrix);
	free(KeyIndexVal);
	free(KeyArrayVal);
}

void SortKey(int* KeyArrayVal, int* KeyIndexVal, char *KeyArray)
{
	int KeyLength = strlen(KeyArray);
	for (int i = 0; i < KeyLength; i++)
	{
		int CharValue = KeyArray[i];
		if (CharValue > 64 && CharValue < 91)
		{
			CharValue = CharValue + 32;
		}
		*(KeyArrayVal + i) = CharValue;
		*(KeyIndexVal + i) = i;
	}

	int* KeyValCpy = malloc(KeyLength * sizeof(int));
	memcpy(KeyValCpy, KeyArrayVal, KeyLength * sizeof(int));

	for (int i, j = 1; j <= KeyLength - 1; j++) {
		i = j;

		while (i > 0 && *(KeyValCpy +i) < *(KeyValCpy + i -1)) {
			int temp = *(KeyValCpy + i);
			*(KeyValCpy + i) = *(KeyValCpy + i-1);
			*(KeyValCpy + i-1) = temp;
			temp = *(KeyIndexVal + i);
			*(KeyIndexVal + i) = *(KeyIndexVal + i - 1);
			*(KeyIndexVal + i - 1) = temp;
			i--;
		}
	}
}

char* BuildIntmdMatrix(char* CipherText, int* KeyIndexVal, int KeyLength)
{
	int CipherTextLength = strlen(CipherText);
	int LongColumnCount = CipherTextLength%KeyLength;
	int ShortColumnLength = CipherTextLength / KeyLength;
	
	char *IntmdMatrix = (int *)malloc((ShortColumnLength+1) * KeyLength * sizeof(int));
	char *FirstMatrix = (int *)malloc((ShortColumnLength + 1) * KeyLength * sizeof(int));

	int index = 0;
	for (int i = 0; i < KeyLength; i++)
	{
		for (int j = 0; j < (ShortColumnLength + 1); j++)
		{
			if (*(KeyIndexVal+i) >= LongColumnCount && j == ShortColumnLength)
			{
				*(IntmdMatrix + (j*KeyLength) + i) = 45;
				continue;
			}
			*(IntmdMatrix + (j*KeyLength) + i) = *(CipherText + index);
			index++;
		}
	}
	
	for (int i = 0; i < KeyLength; i++)
	{
		for (int j = 0; j < (ShortColumnLength + 1); j++)
		{
			*(FirstMatrix + (j*KeyLength) + *(KeyIndexVal+i)) = *(IntmdMatrix + (j*KeyLength) + i);
		}
	}

	return FirstMatrix;
}
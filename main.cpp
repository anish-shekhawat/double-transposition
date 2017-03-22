#include <iostream>
#include <string.h>
#include <stdlib.h>

using namespace std;
void solveSecondTransposition(char *, char *);
void SortKey(int*, int*, char*);
char** BuildIntmdMatrix(char *, int*, int);

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
	KeyArrayVal = new int[KeyLength];
	int* KeyIndexVal;
	KeyIndexVal = new int[KeyLength];

	SortKey(KeyArrayVal, KeyIndexVal, KeyArray);

	char** FirstMatrix = BuildIntmdMatrix(CipherText, KeyIndexVal, KeyLength);
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
		KeyArrayVal[i] = CharValue;
		KeyIndexVal[i] = i;
	}

	int* KeyValCpy;
	KeyValCpy = new int[KeyLength];
	for (int i = 0; i < KeyLength; i++) {
		KeyValCpy[i] = KeyArrayVal[i];
	}

	for (int i, j = 1; j <= KeyLength - 1; j++) {
		i = j;

		while (i > 0 && KeyValCpy[i] < KeyValCpy[i-1]) {
			int temp = KeyValCpy[i];
			KeyValCpy[i] = KeyValCpy[i - 1];
			KeyValCpy[i-1] = temp;
			temp = KeyIndexVal[i];
			KeyIndexVal[i] = KeyIndexVal[i - 1];
			KeyIndexVal[i - 1] = temp;
			i--;
		}
	}
}

char** BuildIntmdMatrix(char* CipherText, int* KeyIndexVal, int KeyLength)
{
	int CipherTextLength = strlen(CipherText);
	int LongColumnCount = CipherTextLength%KeyLength;
	int ShortColumnLength = CipherTextLength / KeyLength;

	char** IntmdMatrix = new char*[ShortColumnLength+1];
	for (int i = 0; i < ShortColumnLength+1; ++i)
		IntmdMatrix[i] = new char[KeyLength];

	char** FirstMatrix = new char*[ShortColumnLength + 1];
	for (int i = 0; i < ShortColumnLength + 1; ++i)
		FirstMatrix[i] = new char[KeyLength];

	int index = 0;
	for (int i = 0; i < KeyLength; i++)
	{
		for (int j = 0; j < (ShortColumnLength + 1); j++)
		{
			if (KeyIndexVal[i] >= LongColumnCount && j == ShortColumnLength)
			{
				IntmdMatrix[j][i] = '-';
				continue;
			}
			IntmdMatrix[j][i] = CipherText[index];
			index++;
		}
	}

	for (int i = 0; i < KeyLength; i++)
	{
		for (int j = 0; j < (ShortColumnLength + 1); j++)
		{
			FirstMatrix[j][KeyIndexVal[i]] = IntmdMatrix[j][i];
		}
	}

	return FirstMatrix;
}
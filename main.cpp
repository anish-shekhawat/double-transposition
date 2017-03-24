#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
void solveSecondTransposition(const string, const string);
void SortKey(vector<int> &, vector<int> &, const string);
char** BuildIntmdMatrix(const string, vector<int>, int);

void main()
{
	ifstream fp;

	fp.open("cipher.txt");

	string cipher;

	// Store ciphertext from file
	getline(fp, cipher);
	 
	solveSecondTransposition(cipher, "DOUBLETRANSPOSITION");
	
}

void solveSecondTransposition(const string CipherText, const string KeyArray)
{
	int CipherTextLength = CipherText.length();
	int KeyLength = KeyArray.length();
	int LongColumnCount = CipherTextLength%KeyLength;
	int ShortColumnLength = CipherTextLength / KeyLength;

	vector<int> KeyArrayVal(KeyLength);
	vector<int> KeyIndexVal(KeyLength);

	SortKey(KeyArrayVal, KeyIndexVal, KeyArray);

	char** FirstMatrix = BuildIntmdMatrix(CipherText, KeyIndexVal, KeyLength);

	for (int i = 0; i < ShortColumnLength + 1; i++)
	{
		cout << endl;

		for (int j = 0; j < KeyLength; j++)
		{
			cout << FirstMatrix[i][j];
		}
	}
}

void SortKey(vector<int> &KeyArrayVal, vector<int> &KeyIndexVal, const string KeyArray)
{
	int KeyLength = KeyArray.length();
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

	vector<int> KeyValCpy(KeyArrayVal);

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

char** BuildIntmdMatrix(const string CipherText, vector<int> KeyIndexVal, int KeyLength)
{
	int CipherTextLength = CipherText.length();
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
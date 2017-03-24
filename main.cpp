#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <ctype.h>
#include <math.h>
#include "include\rapidjson\document.h"
#include "include\rapidjson\filereadstream.h"

using namespace std;

int get_file_char_length(FILE*);
//void compute_bigram_distribution(string const, unordered_map<string, int> &);
int count_substring(const string, const string);
float score(const string, vector<vector<char>>, const rapidjson::Document&);
float sum_log_freq_bigram(vector<vector<char>>, const rapidjson::Document&, int, int, int);
void likely_neighbours(vector<vector<float>>&, int, vector<float>&);
void solveSecondTransposition(vector<vector<char>>&, const string, const string);
void SortKey(vector<int> &, vector<int> &, const string);
void BuildIntmdMatrix(vector<vector<char>>&, const string, vector<int>, int);
char easyToLower(char in);

void main()
{
	ifstream fp, op;
	string cipher;
	string key;
	float key_score;

	fp.open("cipher.txt");
	
	// Store ciphertext from file
	getline(fp, cipher);

	fp.close();
	
	// Hashmap to store ciphertext bigram frequencies
	//unordered_map<string, int> bigram_dist;

	// Get bigram distribution
	//compute_bigram_distribution(cipher, bigram_dist);

	FILE* fjson = fopen("bigrams.json", "r");

	char readBuffer[17408];
	rapidjson::FileReadStream is(fjson, readBuffer, sizeof(readBuffer));

	rapidjson::Document bigram;
	bigram.ParseStream(is);
	fclose(fjson);

	//cout << endl << "Size: " << bigram_dist.size();

	fp.open("dictionary/w4.txt");

	ofstream outfile("out4.txt");

	while (getline(fp, key))
	{
		cout << key << endl;
		outfile << key << endl;

		int LongColumnCount = (cipher.length()) % (key.length());
		int ShortColumnLength = (cipher.length()) / (key.length());

		vector<vector<char>> intermediate_cipher(ShortColumnLength + 1, vector<char>(key.length()));

		solveSecondTransposition(intermediate_cipher, cipher, key);

		key_score = score(cipher, intermediate_cipher, bigram);

		cout << key_score << endl;
		outfile << key_score << endl;
	}
		
	fp.close();

}

float score(const string cipher, vector<vector<char>> arr, const rapidjson::Document& bigrams)
{
	int num_rows = ceil(cipher.length() / arr[0].size());
	float score = 0.0;
	vector<vector<float>> B(arr[0].size(), vector<float>(arr[0].size(), numeric_limits<float>::lowest()));

	for (int i = 0; i < arr[0].size(); i++)
	{
		for (int j = 0; j < arr[0].size(); j++)
		{
			if (i == j)
				continue;

			B[i][j] = sum_log_freq_bigram(arr, bigrams, i, j, num_rows);
		}
	}

	//cout << endl << "B completed";

	vector<float> score_vector;

	likely_neighbours(B, cipher.length(), score_vector);

	//cout << endl << "Neighbours completed" << endl;

	score_vector.pop_back();

	for (int i = 0; i < score_vector.size(); i++)
	{
		score += score_vector[i];
	}

	return score;
}

float sum_log_freq_bigram(vector<vector<char>> arr, const rapidjson::Document &json, int left, int right, int rows)
{
	float sum = 0.0;

	for (int i = 0; i < rows; i++)
	{
		if (arr[i][left] == '-' || arr[i][right] == '-')
		{
			if (i < (rows - 1))
			{
				cout << endl << "Encountered a hyphen (-) before the last row!!!";
				exit(0);
			}
			break;
		}

		string str = string() + easyToLower(arr[i][left]) + easyToLower(arr[i][right]);
		
		sum += json[str.c_str()].GetDouble();
		

	}
	
	sum = sum / rows;
	
	return sum;
}

void likely_neighbours(vector<vector<float>>& B, int keylength, vector<float>& scores)
{
	float max = 1;
	int max_r, max_c;

	vector<int> neighbour(keylength, -1);

	while (max != numeric_limits<float>::lowest())
	{
		max = numeric_limits<float>::lowest();
		for (int i = 0; i < B.size(); i++)
		{
			for (int j = 0; j < B[0].size(); j++)
			{
				if (B[i][j] > max)
				{
					max = B[i][j];
					max_r = i;
					max_c = j;
				}
			}
		}

		scores.push_back(max);

		neighbour[max_r] = max_c;

		/*
		for (int i = 0; i < B[0].size(); i++)
		{
			if (i != max_c)
				B[max_r][i] = 0;
		}

		for (int i = 0; i < B.size(); i++)
		{
			if (i != max_r)
				B[i][max_c] = 0;
		}
		*/

		if (B.size() > max_r)
		{
			B.erase(B.begin() + max_r);
		}

		for (int i = 0; i < B.size(); i++)
		{
			if (B[i].size() > max_c)
			{
				B[i].erase(B[i].begin() + max_c);
			}
		}

	}
}

void solveSecondTransposition(vector<vector<char>>& FirstMatrix, const string CipherText, const string KeyArray)
{
	int CipherTextLength = CipherText.length();
	int KeyLength = KeyArray.length();

	vector<int> KeyArrayVal(KeyLength);
	vector<int> KeyIndexVal(KeyLength);

	SortKey(KeyArrayVal, KeyIndexVal, KeyArray);

	BuildIntmdMatrix(FirstMatrix, CipherText, KeyIndexVal, KeyLength);
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

void BuildIntmdMatrix(vector<vector<char>>& FirstMatrix, const string CipherText, vector<int> KeyIndexVal, int KeyLength)
{
	int CipherTextLength = CipherText.length();
	int LongColumnCount = CipherTextLength%KeyLength;
	int ShortColumnLength = CipherTextLength / KeyLength;

	vector<vector<char>> IntmdMatrix(ShortColumnLength + 1, vector<char>(KeyLength));

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
}

char easyToLower(char in)
{
	if (in <= 'Z' && in >= 'A')
	{
		return in - ('Z' - 'z');
	}

	return in;
}
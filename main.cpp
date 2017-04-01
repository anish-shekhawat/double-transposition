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
int count_substring(const string, const string);
float score(const string, vector<vector<char>>, const rapidjson::Document&);
float sum_log_freq_bigram(vector<vector<char>>, const rapidjson::Document&, int, int, int);
void likely_neighbours(vector<vector<float>>&, int, vector<float>&);
void solve_transposition(vector<vector<char>>&, const string, const string);
void sort_key(vector<int> &, vector<int> &, const string);
void build_matrix(vector<vector<char>>&, const string, vector<int>, int);
char custom_to_lower(char in);

void main()
{
	ifstream fp, op;
	string cipher;
	string key;
	float key_score;
    int ch;

	fp.open("cipher.txt");

	// Store ciphertext from file
	getline(fp, cipher);

	fp.close();

	FILE* fjson = fopen("bigrams.json", "r");

	char readBuffer[17408];
	rapidjson::FileReadStream is(fjson, readBuffer, sizeof(readBuffer));

	rapidjson::Document bigram;
	bigram.ParseStream(is);
	fclose(fjson);

    cout << "Please enter your choice: " << endl;
    cout << "1. Score Key 2" << endl;
    cout << "2. Find Key 1" << endl;
    cout << "Enter your choice (1/2): ";
    cin >> ch;

    if (ch == 1)
    {
        int chfile;
        cout << "Enter n-gram dictionary to score (n = 2,3,4,5) : ";
        cin >> chfile;

        if (chfile >= 2 || chfile <= 5)
        {
            fp.open("dictionary/w" + chfile + ".txt");

            ofstream outfile("out" + chfile + ".txt");

            while (getline(fp, key))
            {
                cout << key << endl;
                outfile << key << endl;

                int LongColumnCount = (cipher.length()) % (key.length());
                int ShortColumnLength = (cipher.length()) / (key.length());

                vector<vector<char>> intermediate_cipher(ShortColumnLength + 1, vector<char>(key.length()));

                solve_transposition(intermediate_cipher, cipher, key);

                key_score = score(cipher, intermediate_cipher, bigram);

                cout << key_score << endl;
                outfile << key_score << endl;
            }

            fp.close();
        }
        else
        {
            cout << "Wrong input!! Exiting!";
            exit(0);
        }
    }
    else
    {
        dictionary_attack(cipher, "keys.txt");
    }



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

		string str = string() + custom_to_lower(arr[i][left]) + custom_to_lower(arr[i][right]);

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

void solve_transposition(vector<vector<char>>& Result, const string CipherText, const string KeyArray)
{
	int CipherTextLength = CipherText.length();
	int KeyLength = KeyArray.length();

	vector<int> KeyArrayVal(KeyLength);
	vector<int> KeyIndexVal(KeyLength);

	sort_key(KeyArrayVal, KeyIndexVal, KeyArray);

	build_matrix(Result, CipherText, KeyIndexVal, KeyLength);
}

void sort_key(vector<int> &KeyArrayVal, vector<int> &KeyIndexVal, const string KeyArray)
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

void build_matrix(vector<vector<char>>& Result, const string CipherText, vector<int> KeyIndexVal, int KeyLength)
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
			Result[j][KeyIndexVal[i]] = IntmdMatrix[j][i];
		}
	}
}

char custom_to_lower(char in)
{
	if (in <= 'Z' && in >= 'A')
	{
		return in - ('Z' - 'z');
	}

	return in;
}

void dictionary_attack(const string cipher, const string keys_file)
{
	ifstream fp;
	string key2;
    string key1;
    string intmd_text;
    string plaintext;

	fp.open(keys_file);

	while (getline(fp, key2))
	{
        int LongColumnCount = (cipher.length()) % (key2.length());
        int ShortColumnLength = (cipher.length()) / (key2.length());

        vector<vector<char>> intermediate_cipher(ShortColumnLength + 1, vector<char>(key2.length()));
        solve_transposition(intermediate_cipher, cipher, key2);

        intmd_text = getText(intermediate_cipher, ShortColumnLength, key2.length());

        ifstream dictp;

        for (int i = 2; i < 6; i++)
        {
            dictp.open("dictionary/w" + i + ".txt");

            while (getline(dictp, key1))
            {
                vector<vector<char>> intermediate_cipher(ShortColumnLength + 1, vector<char>(key1.length()));
                solve_transposition(intermediate_cipher, intmd_text, key1);

                plaintext = getText(cipher, key);

                if (plaintext.find("DOUBLETRANSPOSITION") != string::npos)
                {
                    cout << "Key2 - " << key2 << endl;
                    cout << "Key1 - " << key1 << endl;
                    cout << "Plaintext - " << plaintext << endl;
                }
            }

            dictp.close();
        }
    }
}

string getText(vector<vector<char>> matrix, int short_column_length, int key_length)
{
    string text = "";
    for (int i = 0; i < short_column_length + 1; i++)
    {
        for (int j = 0; j < key_length; j++)
        {
            if (matrix[i][j] == 45)
                continue;
            text += matrix[i][j];
        }
    }

    return text;
}

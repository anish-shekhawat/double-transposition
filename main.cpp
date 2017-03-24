#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <math.h>

#define KEYLENGTH 19

using namespace std;

int get_file_char_length(FILE*);
void compute_bigram_distribution(string const, unordered_map<string, int> &);
int count_substring(const string, const string);
float score(const string, const char [][KEYLENGTH], unordered_map<string, int> hashtable);
float sum_log_freq_bigram(const char [][KEYLENGTH], unordered_map<string, int>, int, int, int);
void likely_neighbours(vector<vector<int>> &);
string solve_transposition(const string, const string);
void sort_key(vector<int> &, vector<int> &, const string);
char** build_matrix(const string, vector<int>, int);
vector<string> dictionary_attack(const string, const string);
string get_plaintext(char**, int, int);

void main()
{
	ifstream fp;
	string cipher;

	fp.open("cipher.txt");

	// Store ciphertext from file
	getline(fp, cipher);

	// Hashmap to store ciphertext bigram frequencies
	unordered_map<string, int> bigram_dist;

	// Get bigram distribution
	compute_bigram_distribution(cipher, bigram_dist);

	cout << endl << "Size: " << bigram_dist.size() << endl;

	dictionary_attack("RHOTNSIDTTREYEHNECIBXTPOCSIEYECLDTERTAOHEITUEISSPSNABRPT", "keys.txt");
	dictionary_attack("ATRYBSORHEXPHEOCTSEYTLPNIENDOATHSTCBUNIESREEDRIPICTTETSI", "keys.txt");
	cout << endl;
}

void compute_bigram_distribution(string const cipher, unordered_map<string, int> &hashtable)
{
	string temp;
	int count=0;

	// Store distinct bigrams in the hashtable
	for (int i = 0; i < cipher.length() - 1; i++)
	{
		temp = cipher[i];
		temp += cipher[i + 1];

		hashtable.insert(make_pair(temp, 0));
		count++;
	}

	//cout << "\nCount: " << count;
	//cout << "\nSize: " << hashtable.size();

	// Compute bigram frequencies

	unordered_map<string, int>::iterator it = hashtable.begin();
	int ctr = 0;
	while (it != hashtable.end())
	{
		temp = it->first;
		//cout << endl << temp;

		it->second = count_substring(cipher, temp);

		//cout << " " << it->second;

		it++;
		ctr++;

	}
	cout << "\nCount: " << ctr;
}

int count_substring(const string str, const string substr)
{
	if (substr.length() == 0)
	{
		return 0;
	}

	int count = 0;

	for (size_t offset = str.find(substr); offset != string::npos;
		offset = str.find(substr, offset + 1))
	{
		++count;
	}

	return count;
}

float score(const string cipher, const char arr[][KEYLENGTH], unordered_map<string, int> hashtable)
{
	int num_rows = ceil(cipher.length() / KEYLENGTH);
	float score = 0;
	vector<vector<float>> B(KEYLENGTH, vector<float>(KEYLENGTH, 0));

	for (int i = 0; i < KEYLENGTH; i++)
	{
		for (int j = 0; j < KEYLENGTH; j++)
		{
			if (i == j)
				continue;

			B[i][j] = sum_log_freq_bigram(arr, hashtable, i, j, num_rows);
		}
	}

	//likely_neighbours(B);

	for (int i = 0; i < B.size(); i++)
	{
		for (int j = 0; j < B[0].size(); j++)
		{
			if (B[i][j] != -1)
			{
				score += B[i][j];
			}
		}
	}

	return score;
}

float sum_log_freq_bigram(const char arr[][KEYLENGTH], unordered_map<string, int> hashtable, int left, int right, int rows)
{
	string temp;
	float sum = 0;
	unordered_map<std::string, int>::iterator it;


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

		temp = arr[i][left];
		temp += arr[i][right];

		it = hashtable.find(temp);

		if (it != hashtable.end())
		{
			if (it->second != 0)
			{
				sum += log10(it->second);
			}
		}
		else
		{
			cout << endl << "Bigram Not Found in Hashtable";
			exit(0);
		}

	}

	sum = sum / rows;

	return sum;
}

void likely_neighbours(vector<vector<float>> &B)
{
	float max = 1;
	int max_r, max_c;

	vector<int> neighbour(KEYLENGTH, -1);

	while (max != 0)
	{
		max = 0;
		for (int i = 0; i < B.size(); i++)
		{
			for (int j = 0; j < B[0].size(); j++)
			{
				if (B[i][j] > max && neighbour[i] == -1)
				{
					max = B[i][j];
					max_r = i;
					max_c = j;
				}
			}
		}

		neighbour[max_r] = max_c;

		for (int i = 0; i < B[0].size(); i++)
		{
			if (i != max_c)
				B[max_r][i] = -1;
		}

		for (int i = 0; i < B.size(); i++)
		{
			if (i != max_r)
				B[i][max_c] = -1;
		}
	}
}

vector<string> dictionary_attack(const string cipher, const string keys_file)
{
	vector<string> possible_keys;
	ifstream fp;
	string key;

	fp.open(keys_file);

	while (getline(fp, key)) 
	{
		string plaintext = solve_transposition(cipher, key);
		if (plaintext.size() == 0)
			continue;
		cout << "Key - " << key << endl;
		cout << "Plaintext - " << plaintext << endl;
		possible_keys.push_back(key);
	}

	return possible_keys;
}

string solve_transposition(const string cipher, const string key)
{
	string plaintext = "";
	int cipher_length = cipher.length();
	int key_length = key.length();
	int short_column_length = cipher_length / key_length;

	vector<int> key_array_val(key_length);
	vector<int> key_index_val(key_length);

	sort_key(key_array_val, key_index_val, key);

	char** plain_matrix = build_matrix(cipher, key_index_val, key_length);

	plaintext = get_plaintext(plain_matrix, short_column_length, key_length);
	
	return plaintext;
}

void sort_key(vector<int> &key_array_val, vector<int> &key_index_val, const string key)
{
	int key_length = key.length();
	for (int i = 0; i < key_length; i++)
	{
		int char_val = key[i];
		if (char_val > 64 && char_val < 91)
		{
			char_val = char_val + 32;
		}
		key_array_val[i] = char_val;
		key_index_val[i] = i;
	}

	vector<int> key_array_val_cpy(key_array_val);

	for (int i, j = 1; j <= key_length - 1; j++) {
		i = j;

		while (i > 0 && key_array_val_cpy[i] < key_array_val_cpy[i-1]) {
			int temp = key_array_val_cpy[i];
			key_array_val_cpy[i] = key_array_val_cpy[i - 1];
			key_array_val_cpy[i-1] = temp;
			temp = key_index_val[i];
			key_index_val[i] = key_index_val[i - 1];
			key_index_val[i - 1] = temp;
			i--;
		}
	}
}

char** build_matrix(const string cipher, vector<int> key_index_val, int key_length)
{
	int cipher_length = cipher.length();
	int long_column_count = cipher_length%key_length;
	int short_column_length = cipher_length / key_length;

	char** unsorted_matrix = new char*[short_column_length+1];
	for (int i = 0; i < short_column_length+1; ++i)
		unsorted_matrix[i] = new char[key_length];

	char** plain_matrix = new char*[short_column_length + 1];
	for (int i = 0; i < short_column_length + 1; ++i)
		plain_matrix[i] = new char[key_length];

	int index = 0;
	for (int i = 0; i < key_length; i++)
	{
		for (int j = 0; j < (short_column_length + 1); j++)
		{
			if (key_index_val[i] >= long_column_count && j == short_column_length)
			{
				unsorted_matrix[j][i] = '-';
				continue;
			}
			unsorted_matrix[j][i] = cipher[index];
			index++;
		}
	}

	for (int i = 0; i < key_length; i++)
	{
		for (int j = 0; j < (short_column_length + 1); j++)
		{
			plain_matrix[j][key_index_val[i]] = unsorted_matrix[j][i];
		}
	}

	return plain_matrix;
}

string get_plaintext(char** plain_matrix, int short_column_length, int key_length)
{
	string plaintext = "";
	for (int i = 0; i < short_column_length + 1; i++)
	{
		for (int j = 0; j < key_length; j++)
		{
			if (plain_matrix[i][j] == 45)
				continue;
			plaintext += plain_matrix[i][j];
		}
	}
	if (plaintext.find("DOUBLETRANSPOSITION") != string::npos)
	{
		return plaintext;
	}
	return "";
}
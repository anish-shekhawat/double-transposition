#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <math.h>

#define KEYLENGHT 19

using namespace std;

int get_file_char_length(FILE*);
void compute_bigram_distribution(string const, unordered_map<string, int> &);
int count_substring(const string, const string);
float score(const string, const int [][KEYLENGHT], unordered_map<string, int> hashtable);
float sum_log_freq_bigram(const int [][KEYLENGHT], unordered_map<string, int>, int, int, int);
void likely_neighbours(vector<vector<int>> &);

void main()
{
	ifstream fp;
	
	fp.open("cipher.txt");

	string cipher;

	// Store ciphertext from file
	getline(fp, cipher);

	// Hashmap to store ciphertext bigram frequencies
	unordered_map<string, int> bigram_dist;

	// Get bigram distribution
	compute_bigram_distribution(cipher, bigram_dist);

	cout << endl << "Size: " << bigram_dist.size();

	
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

float score(const string cipher, const char arr[][KEYLENGHT], unordered_map<string, int> hashtable)
{
	int num_rows = ceil(cipher.length() / KEYLENGHT);
	float score = 0;
	vector<vector<float>> B(KEYLENGHT, vector<float>(KEYLENGHT, 0));

	for (int i = 0; i < KEYLENGHT; i++)
	{
		for (int j = 0; j < KEYLENGHT; j++)
		{
			if (i == j)
				continue;
			
			B[i][j] = sum_log_freq_bigram(arr, hashtable, i, j, num_rows);
		}
	}

	likely_neighbours(B);

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

float sum_log_freq_bigram(const char arr[][KEYLENGHT], unordered_map<string, int> hashtable, int left, int right, int rows)
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

	vector<int> neighbour(KEYLENGHT, -1);

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
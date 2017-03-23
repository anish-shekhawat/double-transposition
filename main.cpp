#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>

using namespace std;

float score(int*, int, int);
int get_file_char_length(FILE*);
void compute_bigram_distribution(string const, unordered_map<string, int> &);
int count_substring(const string, const string);

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
/******************************************************************************
 * Course       : CS 2250 E01 (Fall 2015)
 * Assignment   : Project 5
 * Summary      : This program loads a dictionary from an input file, creates a
 * 				  tree and stores words/definitions inside it, and allows user
 * 				  to look up words and write the tree to a file.
 * Last Modified: December 6, 2015
 ******************************************************************************/
#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include <string>
using namespace std;

class myTrie
{
private:
	static const int ALPHABET_SIZE = 26;

	struct Node
	{
		string word;
		string definition;
		Node* next[ALPHABET_SIZE];
	};

	typedef Node TrieNode;
	typedef TrieNode* Trie;

	void deleteMyTrie(TrieNode* nodePtr);
	void displayTrieReal(TrieNode* nodePtr);
	void createWriteContent(TrieNode* nodePtr);

	Trie root;
	string writeContents;
public:
	myTrie();
	~myTrie();
	Trie create_trie();
	bool insert_trie(string word, string definition);
	bool loadDictionary(string fileName);
	string lookup(string word);
	void deleteTrie();
	void displayTrie();
	bool writeTrie(string fileName);
};




#endif /* DICTIONARY_H_ */

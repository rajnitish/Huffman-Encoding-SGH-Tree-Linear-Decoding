#ifndef __HUFFMAN_H__
#define __HUFFMAN_H__
#include <string>
#include <queue>
#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <bits/stdc++.h>
#include <chrono>
#include <locale>

//#define _DEBUG_

typedef unsigned char CHAR;
typedef unsigned int UINT;
using namespace std;

using namespace std::chrono;

typedef std::ratio<1l, 1000000000000l> pico;
typedef duration<long long, pico> picoseconds;
typedef std::ratio<1l, 1000000l> micro;
typedef duration<long long, micro> microseconds;

#define ExtndAscii_len 256
struct huffman_node
{
	int id;																				//character
	int freq;																				//frequency of the character
	UINT code;																			//huffman code for the character
	CHAR code_length;
	huffman_node* left;
	huffman_node* right;
	huffman_node()
	{
		left = right = NULL;
		freq = 0;
		code_length = 0;
	}
	bool operator<(const huffman_node& a) const
	{
		return freq >a.freq;
	}
};

struct sgh_node
{
	int id;
	UINT code;
	CHAR code_length;
	sgh_node* left;
	sgh_node* right;
	bool is_subtree_root;
	bool is_symbol_node;
	bool is_last_symbol; // Last symbol means char with least freq in sgh tree
	int freq;
	bool is_entered_in_Index;
	bool is_entered_in_symbol;

	sgh_node()
	{
		id = -1;
		left = right = NULL;
		is_subtree_root = false;
		is_symbol_node = false;
		is_last_symbol = false;
		code = '\0';
		freq = 0;
		is_entered_in_Index = false;
		is_entered_in_symbol = false;
		code_length = 0;
	}

	sgh_node(CHAR code)
	{
		id = -1;
		this->code = code;
		left = right = NULL;
		is_subtree_root = false;
		is_symbol_node = false;
		is_last_symbol = false;
		freq = 0;
		code_length = 0;
		is_entered_in_Index = false;
		is_entered_in_symbol = false;
	}

	bool operator<(const sgh_node& a) const
	{
		return freq > a.freq;
	}
};

struct struct_index
{
	int starting_address; //starting address in symbol table i.e decimal value of code mentioned below
	int depth_subtree;


	struct_index(int starting_address,int depth_subtree)
	{
		this->starting_address = starting_address;
		this->depth_subtree = depth_subtree;
	}

	void display()
	{
		cout<<" Starting address in Symbol Table is "<<starting_address<<" and depth of subtree is "<<depth_subtree<<endl;
	}

};

struct struct_symbol_entry
{
	int id;
	CHAR code_length;
	bool isMultientryElement;
	bool isfirstEntry;  // only valid if Duplicated

	struct_symbol_entry(int id, CHAR code_length,bool isFirstEntry, bool isMultientryElement = false)
	{
		this->id = id;
		this->code_length = code_length;
		this->isMultientryElement = isMultientryElement;
		this->isfirstEntry = isfirstEntry;

	}
};

typedef huffman_node* node_ptr;


#define LAST(k,n) ((k) & ((1<<(n))-1))
#define MID(k,m,n) LAST((k)>>(m),((n)-(m)))

class huffman
{
protected:


	enum CHILD_SIDE{LEFT,RIGHT};
	node_ptr node_array[ExtndAscii_len];
	sgh_node sgh_node_array[ExtndAscii_len];

	vector<struct_index> index_array;
	vector<struct_symbol_entry> symbol_array;

	fstream in_file, out_file;
	node_ptr child, parent, root;
	sgh_node *sghRoot = new sgh_node();
	CHAR id;
	string in_file_name, out_file_name;

	int MAX_LENGTH_SGH_CODE = 0;
	UINT sghCharOutput = 0;
	CHAR sghLastCharLen = 0;

	class compare
	{//a object funtion to set comparing rule of priority queue
	public:
		int operator()(const node_ptr& c1, const node_ptr& c2) const
		{
			if( c1->freq > c2->freq) return 1;
			else if( c1->freq < c2->freq) return 0;
			else return -1;
		}
	};

	class compareFreq_Code
	{
	public:
		bool operator()(const huffman_node& a, const huffman_node& b) const
		{
		if(a.freq != b.freq)
		{
			return (a.freq > b.freq);
		}
		else
		return (a.code_length < b.code_length);
		}
	};

	priority_queue<node_ptr, vector<node_ptr>, compare> pq;									//priority queue of frequency from high to low
	void create_node_array();																
	void traverse(node_ptr, UINT,CHAR);														//traverse the huffman tree and get huffman code for a character
	bool isIndexArrayElementExist(int );

	void createSghNode(sgh_node**, UINT,CHAR, CHILD_SIDE);
	int removeSubtreeStatusFromLastSubtree(sgh_node*);

	int depth(sgh_node* );
	void sghCharCount();
	UINT countNodes(sgh_node* root);
	int findHeightOfthisNode(sgh_node*,sgh_node *);
	int getLevelUtil(sgh_node *,UINT, int);
	bool isComplete (sgh_node* root, UINT, UINT);
	void insertIntoSymbolArray(sgh_node*, bool,sgh_node *);
	void traverseSGHForSymbolArray(sgh_node*);
	void traverseSGHForIndexArray(sgh_node*);

	int returnFirstIndexFromSymbolArray(int);
	sgh_node* findleftmostSymbol(sgh_node*);
	bool checkPrefixness();
	bool isSubtreeAlreadymade(UINT,CHAR, vector<sgh_node> );


	void print2DUtil_sgh(sgh_node *, int );
	void print2DUtil_huffman(huffman_node *, int );
	void print2D_huffman(huffman_node *);
	void print2D_sgh(sgh_node *);
	CHAR findCodeLength(UINT);
	void printBinary(UINT, CHAR);
	void decodeSghCode(UINT,CHAR &, UINT);



public:
	~huffman();
	UINT sghCharCountDeco = 0;
	UINT sghCharCountEnco = -1;
	UINT huffCharCountEnco = 0;
	UINT huffCharCountDeco = 1;

	void deleteHuffTree(huffman_node* node);
	void deleteSghTree(sgh_node* node);

	void makeHugeRandomFile(string);
	void setInputFile(string);

	void create_pq();
	void create_huffman_tree();
	void calculate_huffman_codes();
	void huffEncoding();
	void huffDecoding();
	void huffCompareText();


	void generateSGHCode();
	void build_sgh_tree();
	void create_symbol_array();
	void create_index_array();
	void sghEncoding();
	void sghDecoding();
	void sghCompareText();

	void printSymbolArray();
	void printIndexArray();

};

#endif

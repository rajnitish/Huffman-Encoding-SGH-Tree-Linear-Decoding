#include <iostream>
#include <algorithm>
#include <chrono>

#include "huffman.h"

using namespace std;
using namespace std::chrono;

void encode(string,huffman*);
void decode(string,huffman*);
void checkMatching(string,huffman*);

int main(int argc, char *argv[])
{
	if(argc != 2)
		cout<<" Run HuffmanCoding.exe filename.txt "<<endl;
	else
	{
		huffman *h = new huffman;
		//h->makeHugeRandomFile("T5.txt");
		encode(argv[1],h);
		decode(argv[1],h);
		checkMatching(argv[1],h);

	}
	return 0;
}

void encode(string str,huffman *h)
{
	h->setInputFile(str);
	h->create_pq();
	h->create_huffman_tree();
	h->calculate_huffman_codes();
	h->generateSGHCode();
	h->build_sgh_tree();
	h->create_symbol_array();
	//h->printSymbolArray();
	h->create_index_array();
	//h->printIndexArray();
	auto start = high_resolution_clock::now();
	h->huffEncoding();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
	cout << "Average Time taken by each character: huffEncoding is "<<duration.count()/h->huffCharCountEnco << " nanoseconds" << endl;


	auto start1 = high_resolution_clock::now();
	h->sghEncoding();
	auto stop1 = high_resolution_clock::now();
	auto duration1 = duration_cast<nanoseconds>(stop1 - start1);
	cout << "Average Time taken by each character:  sghEncoding is "<<duration1.count()/h->huffCharCountEnco << " nanoseconds" << endl;

}

void decode(string str,huffman *h)
{
	h->setInputFile(str);
	auto start = high_resolution_clock::now();
	h->huffDecoding();
	auto stop = high_resolution_clock::now();
	auto duration = duration_cast<nanoseconds>(stop - start);
	cout << "Average Time taken by each character: huffDecoding is "<<duration.count()/h->huffCharCountDeco << " nanoseconds" << endl;

	auto start1 = high_resolution_clock::now();
	h->sghDecoding();
	auto stop1 = high_resolution_clock::now();
	auto duration1 = duration_cast<nanoseconds>(stop1 - start1);
	cout << "Average Time taken by each character:  sghDecoding is   "<<duration1.count()/h->sghCharCountDeco << " nanoseconds" << endl;

}

void checkMatching(string str,huffman *h)
{

	h->setInputFile(str);
	h->huffCompareText();
	h->sghCompareText();

}


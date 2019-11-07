#ifndef __PRINT_H__
#define __PRINT_H__

#include<bits/stdc++.h>
#include "huffman.h"
#include<iostream>
using namespace std;
#define COUNT 10

void huffman::printBinary(UINT code, CHAR code_len)
{
	for(unsigned char i=code_len;i>0;i--)
	{
		unsigned char d = ((code >> (code_len-1)) & 0x01);
		cout<<(int)d;
		code = code<<1;

	}
}

void huffman::print2DUtil_sgh(sgh_node *root, int space)
{
	// Base case
	if (root == NULL)
		return;

	// Increase distance between levels
	space += COUNT;

	// Process right child first
	print2DUtil_sgh(root->right, space);

	// Print current node after space
	// count
	cout<<endl;
	for (int i = COUNT; i < space; i++)
		cout<<" ";
	cout<<root->code;
	if(root->is_symbol_node)cout<<"("<<(char)root->id<<")";
	if(root->is_subtree_root)cout<<"(@)";
	cout<<endl;

	// Process left child
	print2DUtil_sgh(root->left, space);
}

// Wrapper over print2DUtil()
void huffman::print2D_sgh(sgh_node *root)
{
	// Pass initial space count as 0
	print2DUtil_sgh(root, 0);
}


void huffman::print2DUtil_huffman(huffman_node *root, int space)
{
	// Base case
	if (root == NULL)
		return;

	// Increase distance between levels
	space += COUNT;

	// Process right child first
	print2DUtil_huffman(root->right, space);

	// Print current node after space
	// count
	cout<<endl;
	for (int i = COUNT; i < space; i++)
		cout<<" ";
	cout<<root->code<<"\n";

	// Process left child
	print2DUtil_huffman(root->left, space);
}

// Wrapper over print2DUtil()
void huffman::print2D_huffman(huffman_node *root)
{
	// Pass initial space count as 0
	print2DUtil_huffman(root, 0);
}

void huffman::printIndexArray()
{
	cout<<" PRINTING INDEX ARRAY "<<endl;
	int count = 0;
	for(auto x:index_array)
	{
		cout<<"T"<<count<<" [  "<<x.starting_address<<" , "<<x.depth_subtree<<" ] "<<endl;
		count++;
	}
}

void huffman::printSymbolArray()
{
	cout<<" PRINTING SYMBOL ARRAY "<<endl;
	int count = 0;
	for(auto x:symbol_array)
	{
		cout<<"S"<<count<<" [  "<<(char)x.id<<" , "<<(int)x.code_length<<" ] "<<endl;
		count++;
	}
}

#endif

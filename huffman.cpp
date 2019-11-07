#ifndef __HUFFMAN_CPP__
#define __HUFFMAN_CPP__

#include "huffman.h"

#include "Print.h"

void huffman::setInputFile(string in)
{
	in_file_name.clear();
	in_file_name = in;
}

huffman::~huffman()
{
	deleteHuffTree(root);
	deleteSghTree(sghRoot);

	for (CHAR i = 0; i < ExtndAscii_len-1; i++)
	{
		delete node_array[i];
	}

}

void huffman::deleteHuffTree(huffman_node* node)
{
	if (node == NULL) return;

	deleteHuffTree(node->left);
	deleteHuffTree(node->right);

	delete node;
}
void huffman::deleteSghTree(sgh_node* node)
{
	if (node == NULL) return;

	deleteSghTree(node->left);
	deleteSghTree(node->right);

	delete node;
}
void huffman::create_node_array()
{
	for (CHAR i = 0; i < ExtndAscii_len-1; i++)
	{
		node_array[i] = new huffman_node;
		node_array[i]->id = i;
		node_array[i]->freq = 0;
		node_array[i]->code_length = 0;
	}
}

void huffman::create_pq()
{
	create_node_array();

	in_file.open(in_file_name, ios::in | ios::binary);
	CHAR id = in_file.get();
	while (!in_file.eof())
	{
		node_array[id]->freq++;
		node_array[id]->id = id;
		id = in_file.get();
	}
	in_file.close();


	for (CHAR i = 0; i < ExtndAscii_len-1; i++)
	{
		if (node_array[i]->freq)
		{
			pq.push(node_array[i]);
		}

	}

}

void huffman::create_huffman_tree()
{
	priority_queue<node_ptr, vector<node_ptr>, compare> temp(pq);
	while (temp.size() > 1)
	{//create the huffman tree with highest frequency characher being leaf from bottom to top
		root = new huffman_node;
		root->freq = 0;
		root->right = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		root->left = temp.top();
		root->freq += temp.top()->freq;
		temp.pop();
		temp.push(root);
	}
}

void huffman::calculate_huffman_codes()
{
	traverse(root, '\0',0);
}

void huffman::createSghNode(sgh_node** head_ref, UINT code, CHAR code_length,CHILD_SIDE side)
{
	sgh_node* new_node = new sgh_node();
	new_node->code = code;
	new_node->code_length = code_length;
	if(side == CHILD_SIDE::LEFT)
		new_node->left = (*head_ref);
	else
		new_node->right = (*head_ref);

	(*head_ref) = new_node;
}


void huffman::generateSGHCode()
{
	vector<huffman_node> v;
	for (int i = 0; i < ExtndAscii_len-1; i++)
	{
		if(node_array[i]->freq != 0)
		{
			huffman_node tmp = *node_array[i];
			v.push_back(tmp);
		}
	}

	sort(v.begin(),v.end(),compareFreq_Code());
	MAX_LENGTH_SGH_CODE = v[v.size()-1].code_length;

	//std::cout<<"MAX_LENGTH_SGH_CODE ------------------>"<<MAX_LENGTH_SGH_CODE<<endl;

#ifdef _DEBUG_

	cout<<"AFTER SORTING HUFFMAN VECTOR ------------------------------------------>"<<endl;
	for(auto x:v)
	{
		cout<<" ID = "<<x.id<<" CHARACTER = "<<(unsigned char)x.id<<" FREQ = "<<x.freq<<" HUFFMAN CODE = ";
		printBinary(x.code,x.code_length);
		cout<<endl;
	}
#endif

	UINT prev_code = '\0'; // Ci
	int prev_hufmman_code_length = 0; //li

	for(auto it = v.begin(); it != v.end(); ++it)
	{
		if(it == v.begin())
		{
			UINT s = 0;
			sgh_node_array[it->id].code = s;
			sgh_node_array[it->id].code_length = it->code_length;

			prev_code = s;

		}
		else if(std::next(it) == v.end()) // last element
		{
			UINT s = 1;

			for(CHAR it= 0 ;it<MAX_LENGTH_SGH_CODE -1;it++)	{	s = s<<1;	s += 1;		}
			sgh_node_array[it->id].code = s;
			sgh_node_array[it->id].code_length = MAX_LENGTH_SGH_CODE;
			sgh_node_array[it->id].is_last_symbol = true;
		}
		else
		{
			int len_diff = it->code_length - prev_hufmman_code_length;
			UINT code_word = (prev_code + 1 )*(float)pow(2,len_diff);

			sgh_node_array[it->id].code = code_word;

			if(code_word<it->code)
				sgh_node_array[it->id].code_length = it->code_length;
			else
			{
				int newLen = findCodeLength(code_word);
				if(newLen <it->code_length) newLen = it->code_length;
				sgh_node_array[it->id].code_length = newLen;
			}

			prev_code = sgh_node_array[it->id].code;

#ifdef _DEBUG_
			cout<<"PREVIOUS CODE = "<<prev_code<<endl;
			cout<<" SYMBOL DEBUG "<<(unsigned char)it->id<<" len_diff "<<len_diff<<" pow :: "<<(float)pow(2,len_diff) <<" Deci of prev :: "<<prev_code;
			cout<<" codeword value :: "<<code_word<<endl;
#endif
		}

		sgh_node_array[it->id].freq = it->freq;
		sgh_node_array[it->id].id = it->id;


		prev_hufmman_code_length = it->code_length;


	}

	if(checkPrefixness()) std::exit(0);

#ifdef _DEBUG_

	cout<<"AFTER SGH CODE GENERATED --------------------------------------------->"<<endl;
	vector<sgh_node> vec;
	for (auto x:sgh_node_array)
		if(x.freq)
			vec.push_back(x);

	sort(vec.begin(),vec.end());
	for (auto x:vec)
		if(x.freq)
		{
			cout <<"Symbol :: "<< (unsigned char)x.id << " Freq :: "<<x.freq<<" SGH CodeWord :: ";
			printBinary(x.code,x.code_length);
			cout<<endl;
		}
#endif

}

CHAR huffman::findCodeLength(UINT c)
{
	CHAR p = 0;
	for(int i = 0;i <32;i++)
	{
		if(!c) break;
		c = c>>1;
		p++;
	}
	return p;
}

void huffman::sghCharCount()
{
	huffCharCountDeco>>=1;
	fstream inn_file;
	inn_file.open(in_file_name, ios::in | std::ios::binary );

	while(!inn_file.eof())
	{
		sghCharCountEnco++;
		inn_file.get();
	}
	inn_file.close();

}

void huffman::sghEncoding()
{
	cout<<"\n\nsghEncoding started.............................\n\n";
	sghCharCountEnco = huffCharCountEnco;
	std::string str= in_file_name;
	out_file_name = "sghCompressed_"+str;


	in_file.open(in_file_name, ios::in | std::ios::binary );
	out_file.open(out_file_name, ios::out | std::ios::binary);

	id = in_file.get();

	CHAR tmp = 0;
	UINT K = 0;

	UINT charInput = 0;
	while (!in_file.eof())
	{
		UINT code = sgh_node_array[id].code;

		for (CHAR i = sgh_node_array[id].code_length; i >0 ; i--)
		{
			CHAR current_bit = (code >> (sgh_node_array[id].code_length-1)) & 0x01;
			code = code<<1;
			tmp = tmp+current_bit;

			K++;


			if(K%8 == 0 || ((sghCharCountEnco == charInput + 1)&&((int)i == 1)) )
			{
				out_file<<tmp;
				tmp = 0;
				sghCharOutput++;
			}
			if((sghCharCountEnco == charInput + 1)&&((int)i == 1))
			{
				sghLastCharLen = K%8;

			}

			tmp = tmp<<1;

		}



		charInput++;
		id = in_file.get();

		if(in_file.eof())  // No need to save sentinel in case of sgh as its decoding is pretty simple.
		{
		}
	}

	float ACL = (float)( sghCharOutput )/(float)(charInput);
	ACL = (round(ACL*100))/100;
	cout<<"ACL FOR SGH HUFFMAN IS "<<ACL<<endl;


	in_file.close();
	out_file.close();

	cout<<"\n\nsghEncoding end.............................\n\n";
}

void huffman::huffEncoding()
{
	cout<<"\n\nhuffEncoding started.............................\n\n";

	std::string str = in_file_name;
	out_file_name = "huffCompressed_"+str;


	in_file.open(in_file_name, ios::in | std::ios::binary );
	out_file.open(out_file_name, ios::out | std::ios::binary);

	id = in_file.get();

	CHAR tmp = 0;
	UINT K = 0;
	while (!in_file.eof())
	{
		UINT code = node_array[id]->code;
		for (CHAR i = node_array[id]->code_length; i >0 ; i--)
		{

			CHAR current_bit = (code >> (node_array[id]->code_length-1)) & 0x01;
			code = code<<1;
			tmp = tmp+current_bit;

			K++;

			if(K%8 == 0)
			{
				out_file<<tmp;
				tmp = 0;
				continue;
			}

			tmp = tmp<<1;

		}

		huffCharCountEnco++;
		id = in_file.get();

		if(in_file.eof())
		{
			tmp >>=1;
			out_file<<tmp;

			char lastLen = K%8;
			out_file<<lastLen;

			in_file.close();
			out_file.close();
		}
	}

	float ACL = (float)(K)/(float)(huffCharCountEnco);
	ACL = (round(ACL*100))/100;
	cout<<"ACL FOR BASIC HUFFMAN IS "<<ACL<<endl;

	cout<<"\n\nhuffEncoding end.............................\n\n";
}

int random(int min, int max){
	return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

void huffman::makeHugeRandomFile(string fileName)
{
	fstream out;
	out.open(fileName, ios::out | std::ios::binary);
	string str ;
	char a[]={32,32,32};
	UINT size = 2147483648; // == 2 GB
	CHAR T;
	for (UINT i = 0; i < size; i++) {
		T = random(1,254) ;

		out<<T;
	}

	for(auto x:a)out<<x;
	out.close();
}
void huffman::huffDecoding()
{
	cout<<"\n\nhuffDecoding started.............................\n\n";

	std::string str= in_file_name;

	in_file.open("huffCompressed_"+str, ios::in | std::ios::binary );
	out_file.open("huffDecoded_"+str, ios::out | std::ios::binary);

	struct huffman_node* curr = root;

	fstream inn_file,in2_file;

	inn_file.open("huffCompressed_"+str, ios::in | std::ios::binary );
	in2_file.open("huffCompressed_"+str, ios::in | std::ios::binary );


	inn_file.get();
	while(!inn_file.eof())
	{
		inn_file.get();
		huffCharCountDeco++;
	}

	inn_file.close();

	in2_file.seekg(huffCharCountDeco-3, ios::beg);

	// Read the next 2 characters from the file into a buffer
	CHAR A[2];
	A[0] = in2_file.get();
	A[1] = in2_file.get();

	// End the buffer with a null terminating character
	A[2] = 0;

	in2_file.close();
	inn_file.close();


	UINT K = 1;
	CHAR comp_code = in_file.get();

	while (!in_file.eof())
	{
		if(K == huffCharCountDeco - 3)
		{
			CHAR nextCode = comp_code;
			CHAR code = nextCode;
			CHAR current_bit;
			CHAR code1 = A[0]<<(8-(int)A[1]);

			for (CHAR i = 8+(int)A[1]; i >0 ; i--)
			{
				if(i >(int)A[1])
					current_bit = (code >>7) & 0x01;
				else
					current_bit = (code1 >>7) & 0x01;

				if (current_bit == 0)
					curr = curr->left;
				else
					curr = curr->right;

				// reached leaf node
				if (curr->left==NULL and curr->right==NULL)
				{
					out_file<<(CHAR)curr->id;
					curr = root;
				}

				if(i >(int)A[1])
					code <<= 1;
				else
					code1 <<= 1;
			}

			break;
		}

		else
		{
			CHAR code = comp_code;
			for (CHAR i = 8; i >0 ; i--)
			{
				CHAR current_bit = (code >>7) & 0x01;

				if (current_bit == 0)
					curr = curr->left;
				else
					curr = curr->right;

				// reached leaf node
				if (curr->left==NULL and curr->right==NULL)
				{
					out_file<<(CHAR)curr->id;
					curr = root;
				}

				code <<= 1;
			}
		}

		comp_code =  in_file.get();
		K++;

	}

	in_file.close();
	out_file.close();

	sghCharCount();


	cout<<"\n\nhuffDecoding end.............................\n\n";
}

void huffman::decodeSghCode(UINT code, CHAR &remain_code_len ,UINT max_len)
{
	UINT K = 1;
	for (UINT i = max_len; i >0 ; i--)
	{
		CHAR current_bit = (code >>(max_len-1)) & 0x01;

		if(current_bit == 0 && K <max_len)
		{
			int IA_index = K-1;
			struct_index index_entry = index_array[IA_index];

			UINT tmp = 0;
			code<<=1;
			K++;

			for(CHAR j = 0; j<index_entry.depth_subtree ; j++)
			{
				CHAR cb = (code >>(max_len -1)) & 0x01;

				tmp += cb;
				if(j != index_entry.depth_subtree -1)
				{
					tmp <<= 1;

					code <<= 1;
				}
				i--;
				K++;
			}

			int SA_index = index_entry.starting_address + tmp;
			struct_symbol_entry symbol_entry = symbol_array[SA_index];
			int id = symbol_entry.id;
			out_file<<(unsigned char)id;
			int code_length = symbol_entry.code_length;

			remain_code_len = max_len - code_length;
			return;
		}
		else if(K == max_len)
		{
			struct_symbol_entry symbol_entry = symbol_array[symbol_array.size() - 1];
			if(current_bit == 0)
				symbol_entry = symbol_array[symbol_array.size() - 2];
			int id = symbol_entry.id;
			out_file<<(unsigned char)id;
			remain_code_len = 0;
			return;
		}
		K++;
		code <<= 1;
	}

}

void huffman::sghDecoding()
{
	cout<<"\n\nsghDecoding started.............................\n\n";
	std::string str= in_file_name;

	in_file.open("sghCompressed_"+str, ios::in | std::ios::binary );
	out_file.open("sghDecoded_"+str, ios::out | std::ios::binary);

	CHAR comp_code = in_file.get();
	UINT fetchedCode = 0;
	UINT tmpCode = 0;
	CHAR remLen = 0;
	int K = 1;
	while (!in_file.eof())
	{
		CHAR code = comp_code;
		sghCharCountDeco++;
		int X = 8;
		if(sghCharCountDeco == sghCharOutput && sghLastCharLen != 0 )
		{
			X = sghLastCharLen;
			code =  code<<(8-sghLastCharLen);
		}
		for (CHAR i = X; i >0 ; i--)
		{
			CHAR current_bit = (code >>7) & 0x01;
			code <<= 1;
			tmpCode += current_bit;

			bool flag = (sghCharCountDeco == sghCharOutput) && (i == 1) ;


			if(K%MAX_LENGTH_SGH_CODE == 0 || flag )
			{

				fetchedCode = fetchedCode + tmpCode;

				if(flag){
					decodeSghCode(fetchedCode,remLen,(K%MAX_LENGTH_SGH_CODE));
				}

				else
				{
					decodeSghCode(fetchedCode,remLen,MAX_LENGTH_SGH_CODE);
				}


				if(remLen>=0)
				{
					fetchedCode = 	LAST(fetchedCode,MAX_LENGTH_SGH_CODE);
					fetchedCode <<= MAX_LENGTH_SGH_CODE - remLen;
					K = remLen+1;
				}

				remLen = 0;
				tmpCode = 0;
			}
			else
			{
				tmpCode <<=1;
				K++;
			}
		}


		comp_code =  in_file.get();

	}

	in_file.close();
	out_file.close();

	cout<<"\n\nsghDecoding end.............................\n\n";
}
void huffman::build_sgh_tree()
{
	vector<sgh_node> subtreeList;
	sgh_node* last_symbol_parent = NULL;
	for (auto x : sgh_node_array)
	{
		if(x.freq)
		{
			UINT code = x.code;
			bool is_zero_found = false;
			UINT tmp = 0b0;
			sgh_node *sgh_root = sghRoot;
			CHAR tmp_len = 0;

			for (CHAR i = x.code_length; i >0 ; i--)
			{
				CHAR current_bit = (code >> (x.code_length-1)) & 0x01;
				code = code<<1;

				tmp = tmp+current_bit;
				tmp_len++;

				if (current_bit == 0)
				{
					if (sgh_root->left == NULL)
					{
						createSghNode(&(sgh_root->left),tmp,tmp_len,CHILD_SIDE::LEFT);

						if(is_zero_found == false) // This is for first time we found zero i.e. we have found our Ti's node
						{
							bool flg = isSubtreeAlreadymade(tmp,tmp_len,subtreeList);

							if(!flg)
							{
								(sgh_root->left)->is_subtree_root = true;
								subtreeList.push_back(*(sgh_root->left));

							}
							is_zero_found = true;
						}

					}


					sgh_root = sgh_root->left;

				}
				else if (current_bit == 1)
				{

					if (sgh_root->right == NULL)
					{
						createSghNode(&(sgh_root->right),tmp,tmp_len,CHILD_SIDE::RIGHT);
					}

					if(x.is_last_symbol && i == 1)
					{
						last_symbol_parent = sgh_root;
						sgh_root->is_subtree_root = true;
					}

					sgh_root = sgh_root->right;
				}

				tmp = tmp<<1;
			}
			sgh_root->id = x.id;																	//attach id to the leaf
			sgh_root->is_symbol_node = true;


		}
	}

	if(!last_symbol_parent)
	{
		cout<<" last_symbol_parent is NULL "<<endl;
		exit(0);
	}

	removeSubtreeStatusFromLastSubtree(last_symbol_parent);
	last_symbol_parent->is_subtree_root = true;
}

void huffman::insertIntoSymbolArray(sgh_node* root, bool completeness, sgh_node *subtreeRoot)
{

	if(root == NULL) return;

	insertIntoSymbolArray(root->left,completeness,subtreeRoot);

	if(root->is_symbol_node)
	{

		if(!completeness)
		{
			int height_node = findHeightOfthisNode(root,subtreeRoot);
			int dep = depth(subtreeRoot);
			int diff = dep - height_node;

			if(diff == 0)
			{
				struct_symbol_entry obj(root->id,root->code_length,false,false);
				symbol_array.push_back(obj);
			}
			else
			{
				for(int i = 0; i<2*diff;i++)
				{
					bool isFirstEntry = false;

					if(i == 0)
						isFirstEntry = true;
					struct_symbol_entry obj(root->id,root->code_length,isFirstEntry,true);
					symbol_array.push_back(obj);

				}

			}
		}
		else
		{
			struct_symbol_entry obj(root->id,root->code_length,false,false);
			symbol_array.push_back(obj);
		}

	}

	insertIntoSymbolArray(root->right,completeness,subtreeRoot);

}

void huffman::traverseSGHForSymbolArray(sgh_node* root) // DFS of tree performed i.e. Inorder
{
	if(root == NULL) return;

	traverseSGHForSymbolArray(root->left);

	if(root->is_subtree_root && !root->is_entered_in_symbol)
	{

		unsigned int node_count = countNodes(root);
		unsigned int index = 0;

		bool flagCompletness = isComplete(root,index,node_count);
		insertIntoSymbolArray(root,flagCompletness,root);


		root->is_entered_in_symbol = true;
	}

	traverseSGHForSymbolArray(root->right);
}

void huffman::traverseSGHForIndexArray(sgh_node* root)
{
	if(root == NULL) return;
	traverseSGHForIndexArray(root->left);

	if(root->is_subtree_root && !root->is_entered_in_Index)
	{
		sgh_node* tmp = findleftmostSymbol(root);
		int strtAddr = returnFirstIndexFromSymbolArray(tmp->id);
		int dep = depth(root) - 1;
		struct_index obj(strtAddr,dep);
		index_array.push_back(obj);
		root->is_entered_in_Index = true;
	}

	traverseSGHForIndexArray(root->right);
}

void huffman::create_index_array()
{
	traverseSGHForIndexArray(sghRoot);

}

void huffman::create_symbol_array()
{
	traverseSGHForSymbolArray(sghRoot);

}

bool huffman::isSubtreeAlreadymade(UINT code,CHAR code_len, vector<sgh_node> subtreeCodes)
{
	for(auto x:subtreeCodes)
	{
		bool myflag = true;
		UINT tmp1 = x.code;
		for (CHAR i = x.code_length; i >0 ; i--)
		{
			int sft = x.code_length-1;
			CHAR current_bit_i = ( tmp1 >> (sft) ) & 0x01;
			tmp1 = tmp1<<1;

			UINT tmp2 = code;
			for(CHAR j = code_len;j>0;j--)
			{
				int sft2 = code_len-1;
				CHAR current_bit_j = (tmp2 >>sft2 ) & 0x01;
				tmp2 = tmp2<<1;

				if( (x.code_length - i) == (code_len -j) )
				{
					if(current_bit_i != current_bit_j)
					{
						myflag = false;
						break;
					}
					else
					{
						if(current_bit_i == 0 && current_bit_j == 0)
							return true;
					}

				}
			}
			if(myflag == false)
			{
				break;
			}

		}

	}
	return false;
}

int huffman::returnFirstIndexFromSymbolArray(int id)
{
	int count = 0;
	for(auto x:symbol_array)
	{
		if(x.id == id)
			return count;  // immediate return first out of duplicates.

		count++;
	}
	return -1;
}

int huffman::removeSubtreeStatusFromLastSubtree(sgh_node* node)
{
	if (node == NULL)
		return 0;
	else
	{
		removeSubtreeStatusFromLastSubtree(node->left);
		if(node->is_subtree_root) node->is_subtree_root = false;
		removeSubtreeStatusFromLastSubtree(node->right);
	}
	return 0;
}

bool huffman::checkPrefixness()
{
	bool flag1 = false, flag2 = false;

	for(auto x:node_array)
	{
		if(!x)continue;
		if(x->freq==0 ) continue;

		for(auto y:node_array)
		{
			if(!y)continue;
			if(y->freq==0) continue;
			if(x->id == y->id ) continue;


			bool myflag = true;
			UINT tmp = x->code;
			for (CHAR i = x->code_length; i >0 ; i--)
			{
				CHAR current_bit_i = (tmp >> (x->code_length -1) ) & 0x01;
				tmp = tmp<<1;

				UINT tmp2 = y->code;
				for(CHAR j = y->code_length;j>0;j--)
				{
					CHAR current_bit_j = (tmp2 >> (y->code_length -1) ) & 0x01;
					tmp2 = tmp2<<1;

					if( (x->code_length - i) == (y->code_length -j) )
					{
						if(current_bit_i != current_bit_j)
						{
							myflag = false;
						}

					}
				}


			}

			if(myflag == true ) {
				cout<<" BASIC HUFFMAN CODE of "<<(CHAR)x->id<<" ";
				printBinary(x->code,x->code_length);
				cout<<" is prefix of "<<(CHAR)y->id<<" ";
				printBinary(y->code,y->code_length);
				cout<<endl;
				flag1 = true;
			}

		}
	}

	for(auto x:sgh_node_array)
	{
		for(auto y:sgh_node_array)
		{
			if(x.id == y.id || !(x.freq>0 && y.freq>0)) continue;

			bool myflag = true;
			UINT tmp1 = x.code;
			for (CHAR i = x.code_length; i >0 ; i--)
			{

				CHAR current_bit_i = (tmp1 >> (x.code_length -1) ) & 0x01;
				tmp1 = tmp1<<1;

				UINT tmp2 = y.code;
				for(CHAR j = y.code_length;j>0;j--)
				{

					CHAR current_bit_j = (tmp2 >> (y.code_length -1) ) & 0x01;
					tmp2 = tmp2<<1;

					if( (x.code_length - i) == (y.code_length -j) )
					{
						if(current_bit_i != current_bit_j)
						{
							myflag = false;
							break;
						}

					}
				}

			}
			if(myflag == true ) {
				cout<<" SGH HUFFMAN CODE of "<<(CHAR)x.id<<" ";
				printBinary(x.code,x.code_length);
				cout<<" is prefix of "<<(CHAR)y.id<<" ";
				printBinary(y.code,y.code_length);
				cout<<endl;
				flag1 = true;
			}

		}

	}
	return (flag1 || flag2);
}

unsigned int huffman::countNodes(sgh_node* root)
{
	if (root == NULL)
		return (0);
	return (1 + countNodes(root->left) + countNodes(root->right));
}

bool huffman::isComplete (sgh_node* root, unsigned int index, unsigned int number_nodes)
{
	if (root == NULL)   return (true);

	if (index >= number_nodes)
		return (false);

	return (isComplete(root->left, 2*index + 1, number_nodes) &&
			isComplete(root->right, 2*index + 2, number_nodes));
}

sgh_node* huffman::findleftmostSymbol(sgh_node* root)
{
	if(root->left == NULL) return root;
	findleftmostSymbol(root->left);
}

int huffman::getLevelUtil(struct sgh_node *node,UINT data, int level)
{
	if (node == NULL)
		return 0;

	if (node -> id == data)
		return level;

	int downlevel = getLevelUtil(node -> left,data, level + 1);
	if (downlevel != 0)
		return downlevel;

	downlevel = getLevelUtil(node->right,data, level + 1);
	return downlevel;
}

int huffman::findHeightOfthisNode(sgh_node* root,sgh_node *subtreeRoot)
{
	return  getLevelUtil(subtreeRoot, root->id, 1);
}

int huffman::depth(sgh_node* node)
{
	if (node == NULL)
		return 0;
	else
	{
		int lDepth = depth(node->left);
		int rDepth = depth(node->right);

		if (lDepth > rDepth)
			return(lDepth + 1);
		else return(rDepth + 1);
	}
}

void huffman::traverse(node_ptr node, UINT code,CHAR code_len)
{
	if (node->left == NULL && node->right == NULL)
	{
		node->code = code;
		node->code_length = code_len;
	}
	else
	{
		UINT newCode1 = code<<1;
		newCode1 += 0b0;

		traverse(node->left, newCode1,code_len+1);

		UINT newCode2 = code<<1;
		newCode2 += 0b1;

		traverse(node->right, newCode2,code_len+1 );
	}
}

void huffman::huffCompareText()
{
	std::string str= in_file_name;
	out_file_name = "huffDecoded_"+str;

	in_file.open(in_file_name, ios::in | std::ios::binary);
	out_file.open(out_file_name, ios::in | std::ios::binary);

	unsigned char c1,c2;
	bool flag = true;;
	while(1){
		c1=in_file.get();
		c2=out_file.get();
		if(c1!=c2 ){
			cout<<" HURRAY MISMATCH  IN HUFFMAN DECODED FILE "<<endl;
			cout<<c1<<endl;
			cout<<c2<<endl;
			flag=false;
			break;
		}
		if(in_file.eof() && out_file.eof())
			break;
		if((in_file.eof() && !out_file.eof()) || (!in_file.eof() && out_file.eof()) )
		{
			flag = false;
			break;
		}
	}
	out_file.close();
	in_file.close();
	if(flag)
		cout<<"HUFF Original & Decompress Files are same."<<endl;
	else
		cout<<"HUFF Original & Decompress Files are not same."<<endl;
}

void huffman::sghCompareText()
{
	std::string str= in_file_name;
	out_file_name = "sghDecoded_"+str;

	in_file.open(in_file_name, ios::in | std::ios::binary);
	out_file.open(out_file_name, ios::in | std::ios::binary);

	unsigned char c1,c2;
	bool flag = true;
	UINT count = 4;
	while(1){
		c1=in_file.get();
		c2=out_file.get();
		if(c1!=c2){
			cout<<" HURRAY MISMATCH IN SGH DECODED FILE "<<endl;
			cout<<c1<<endl;
			cout<<c2<<endl;
			flag=false;
		}
		if(in_file.eof() && out_file.eof())
			break;
		if((in_file.eof() && !out_file.eof()) || (!in_file.eof() && out_file.eof()) )
		{
			flag = false;
			break;
		}

		count++;
		if(count == huffCharCountEnco)
			break;
	}
	out_file.close();
	in_file.close();
	if(flag)
		cout<<" SGH Original & Decompress Files are same."<<endl;
	else
		cout<<" SGH Original & Decompress Files are not same."<<endl;
}

#endif

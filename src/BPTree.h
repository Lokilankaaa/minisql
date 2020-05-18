#pragma once
#include "BPT_node.h"
#include <vector>

using namespace std;

enum Compare_Operator { BL, BE, EQ, AE, AB, BETWEEN };
const int Invalid_index = -1;
typedef struct SelectResult
{
	int index;
	LeafNode* dest;
}RESULT;

class BPT {
public:
	BPT();
	~BPT();

	bool Insert(KEYTYPE key, const DATATYPE& data);
	bool Remove(KEYTYPE key);
	bool Update(KEYTYPE oldkey, KEYTYPE newkey);

	vector<DATATYPE> Select(KEYTYPE key, int opera);
	vector<DATATYPE> Select(KEYTYPE smallkey, KEYTYPE largekey);

	bool Search(KEYTYPE key);
	void Clear();
	void print();
	void printdata();

private:
	void RecursiveInsert(TreeNode* parent, KEYTYPE key, const DATATYPE& data);
	void RecursiveRemove(TreeNode* parent, KEYTYPE key);

	void PrintInConcavo(TreeNode* node, int cnt);
	bool RecursiveSearch(TreeNode* node, KEYTYPE key);
	void ChangeKey(TreeNode* node, KEYTYPE oldKey, KEYTYPE newKey);
	void Search(KEYTYPE key, RESULT& result);
	void RecursiveSearch(TreeNode* node, KEYTYPE key, SelectResult& result);
	void Remove(KEYTYPE key, DATATYPE& dataValue);
	void RecursiveRemove(TreeNode* parentNode, KEYTYPE key, DATATYPE& dataValue);

	TreeNode* root;
	LeafNode* head;
	KEYTYPE maxval;
};
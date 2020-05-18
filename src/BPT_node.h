#pragma once
//#include <iostream>
//using namespace std;

//此处考虑用template代替
typedef double KEYTYPE;//键的类型
typedef int DATATYPE; //数据类型
enum Node { INTERNAL, LEAF };
enum Siblings { LEFT, RIGHT };

const int DEGREE = 7;  //节点的度
const int min_keynum = DEGREE - 1;
const int max_keynum = 2 * DEGREE - 1;
const int min_chinum = min_keynum + 1;
const int max_chinum = max_keynum + 1;
const int min_leafnum = min_keynum;
const int max_leafnum = max_keynum;


//template <class T>
class TreeNode {
public:
	TreeNode() {
		SetType(LEAF);
		SetKeyNum(0);
	};
	virtual ~TreeNode() { SetKeyNum(0); };
	Node GetType() { return type; }
	void SetType(Node type) { this->type = type; };
	int GetKeyNum() { return degree; }
	void SetKeyNum(int num) { degree = num; }
	KEYTYPE GetKeyValue(int index) { return values[index]; }
	void SetKeyValue(int index, KEYTYPE key) { values[index] = key; }
	int GetKeyIndex(KEYTYPE key);

	virtual void RemoveKey(int keyindex, int childindex) = 0;
	virtual void Split(TreeNode* parent, int childindex) = 0;
	virtual void Merge(TreeNode* parent, TreeNode* child, int index) = 0;
	virtual void Clear() = 0;
	virtual void BorrowNode(TreeNode* dest, TreeNode* parent, int index, Siblings brother) = 0;
	virtual int GetChildIndex(KEYTYPE key, int index) = 0;
protected:
	Node type;
	int degree;
	KEYTYPE values[max_keynum];
};

class InternalNode :public TreeNode {
public:
	InternalNode() { SetType(INTERNAL); };
	~InternalNode() {};

	TreeNode* GetChild(int index) { return children[index]; }
	void SetChild(int index, TreeNode* child) { children[index] = child; }
	void Insert(int keyindex, int childindex, KEYTYPE key, TreeNode* child);


	virtual void RemoveKey(int keyindex, int childindex);
	virtual void Split(TreeNode* parent, int childindex);
	virtual void Merge(TreeNode* parent, TreeNode* child, int index);
	virtual void Clear();
	virtual void BorrowNode(TreeNode* dest, TreeNode* parent, int index, Siblings brother);
	virtual int GetChildIndex(KEYTYPE key, int index);

private:
	TreeNode* children[max_chinum];
};

class LeafNode :public TreeNode {
public:
	LeafNode()
	{
		SetType(LEAF);
		SetLeft(NULL);
		SetRight(NULL);
	};
	~LeafNode() {};

	LeafNode* GetLeft() { return left; }
	LeafNode* GetRight() { return right; }
	void SetLeft(LeafNode* leaf) { left = leaf; }
	void SetRight(LeafNode* leaf) { right = leaf; }
	DATATYPE GetData(int index) { return data[index]; }
	void SetData(int index, const DATATYPE& data);
	void Insert(KEYTYPE key, const DATATYPE& data);

	virtual void RemoveKey(int keyindex, int childindex);
	virtual void Split(TreeNode* parent, int childindex);
	virtual void Merge(TreeNode* parent, TreeNode* child, int index);
	virtual void Clear();
	virtual void BorrowNode(TreeNode* dest, TreeNode* parent, int index, Siblings brother);
	virtual int GetChildIndex(KEYTYPE key, int index);

private:
	LeafNode* left;
	LeafNode* right;
	DATATYPE data[max_leafnum];
};
#pragma once
#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "bplustree.h"
#include "buffer_manager.h"
#include "buffer_manager_page.h"

extern BufferManager buffer_manager;

using namespace std;
template <typename T>
class TreeNode {
public:
	//�ý����key����
	unsigned int key_num;
	//ָ�򸸽ڵ�ָ��
	TreeNode* parent;
	//���key����
	vector <T> keys;
	//ָ���ӽ���ָ������
	vector <TreeNode*> children;
	vector <int> vals;
	//ָ����һ��Ҷ����ָ��
	TreeNode* next;
	//�˽���Ƿ���Ҷ���ı�־
	bool isleaf;
	//�����Ķ�
	int degree;

public:
	//���캯��
	//���룺���Ķȣ��Ƿ�ΪҶ���
	//���ܣ���ʼ��һ���µĽ�㣬��Leaf=true�򴴽�һ��Ҷ��㣬���򴴽�һ��֦�ɽ��
	TreeNode(int in_degree, bool Leaf = false);
	//��������
	~TreeNode();
	//���ܣ��ж��Ƿ�Ϊ�����
	bool IsRoot();
	//���룺keyֵ��(index)
	//�����bool
	//���ܣ��жϽ�����Ƿ����ĳkeyֵ����ͨ��index���÷��ظ�ֵindex
	//��ͨ����������boolֵ˵���Ƿ��ҵ�
	bool FindKey(T key, unsigned int& index);
	//�����TreeNodeָ��
	//���ܣ���һ�������ѳ��������(��B+���ķ����й�)
	//�½��Ϊ��������һ�����
	//ͬʱ����key���÷���ȥ���ϲ��keyֵ
	TreeNode* SplitNode(T& key);
	//���룺keyֵ
	//�������ӵ�position
	//���ܣ���֦�ɽ�������keyֵ����������ӵ�λ��
	unsigned int AddKey(T& key);
	//���룺keyֵ
	//�������ӵ�position
	//���ܣ���Ҷ��������keyֵ����������ӵ�λ��
	unsigned int AddKey(T& key, int val);
	//���룺keyֵ��Ӧ��index
	//�����bool
	//���ܣ��ڽ����ɾ��index��Ӧ��keyֵ
	bool DeleteKeyByIndex(unsigned int index);
	//������һ������Ҷ���ָ��
	TreeNode* nextLeaf();
	//����:��ʼindex,��ֹkey�����ؽ��vector����
	//���:������ֹkey����true�����򷵻�flase
	//���ܣ�����һ����Χ��value����
	bool FindHigher(unsigned int index, T& key, vector<int>& values);
	bool FindLower(unsigned int index, vector<int>& values);

	void PrintList();
};

//ģ����BPlusTree�����ڶ�����B+���Ĳ���
//�ɴ�ģ�����TreeNodeģ�飬�����ɴ�ģ����buffer_managerģ����н���
//����ļ��Ķ�д����
template <typename T>
class BPlusTree {

private:
	//������TreeNodeָ�룬�����߽��в�������
	typedef TreeNode<T>* Tree;
	//����ṹ��������ʱ�洢���ҵ�keyֵ��������λ�ã�������в���
	struct SearchNodeParse {
		Tree pNode; //������Ӧkey�Ľ��ָ��
		unsigned int index; //key�ڽ���е�index
		bool ifFound; //�Ƿ��ҵ���key
	};

private:
	//�ļ����������ļ���д����
	string file_name;
	//�����ָ��
	Tree root;
	//Ҷ������ͷָ��
	Tree leaf_head;
	//����key������
	unsigned int key_num;
	//������level����
	unsigned int level;
	//������node����
	unsigned int node_num;
	//fileNode* file; // the filenode of this tree
	//ÿ��keyֵ��size������һ������˵������key��sizeӦ����ͬ��
	int key_size;
	//���Ķ�
	int degree;

public:
	//���캯��
	//���ڹ���һ���µ�����ȷ��m_name,key��size�����Ķ�
	//ͬʱ������������Ϊ���������ڴ�
	BPlusTree(string m_name, int key_size, int degree);
	//��������
	//�ͷ���Ӧ���ڴ�
	~BPlusTree();

	//����keyֵ���ض�Ӧ��Value
	int SearchVal(T& key);
	//���룺keyֵ����value
	//�����bool
	//���ܣ������в���һ��keyֵ
	//�����Ƿ����ɹ�
	bool InsertKey(T& key, int val);
	//���룺keyֵ
	//�����bool
	//���ܣ�������ɾ��һ��keyֵ
	//�����Ƿ�ɾ���ɹ�
	bool DeleteKey(T& key);
	//���룺���ĸ����
	//���ܣ�ɾ�����������ͷ��ڴ�ռ䣬��Ҫ��������������
	void DropTree(Tree node);
	//���룺key1��key2������vals������
	//���ܣ����ط�Χ�����������value����vals������
	void SearchRange(T& key1, T& key2, vector<int>& vals, int flag);

	//�Ӵ��̶�ȡ��������
	void ReadFromDiskAll();
	//��������д�����
	void WrittenBackToDiskAll();
	//�ڴ����ж�ȡĳһ�������
	void ReadFromDisk(char* p, char* end);

	void PrintLeaf();

	string findTableName(data index);
private:
	//��ʼ��B+�����������ڴ�ռ�
	void InitTree();
	//���ڲ���ĳkey����������������������е���
	bool AdjustAfterInsert(Tree node);
	//����ɾ��ĳkey����ܳ��ֽ�������������������е���
	bool AdjustAfterDelete(Tree node);
	//���ڲ���ĳkeyֵ������Ҷ���λ��
	void FindToLeaf(Tree node, T key, SearchNodeParse& snp);
	//��ȡ�ļ���С
	void GetFile(string file_path);
	int GetBlockNum(string table_name);
};



template <class T>
TreeNode<T>::TreeNode(int in_degree, bool leaf) :
	key_num(0),
	parent(NULL),
	next(NULL),
	isleaf(leaf),
	degree(in_degree)
{
	//degree+1��keyָ��degree+2���ӽڵ�
	for (unsigned i = 0; i < degree + 1; i++) {
		children.push_back(NULL);
		keys.push_back(T());
		vals.push_back(0);
	}
	children.push_back(NULL);
}

template <class T>
TreeNode<T>::~TreeNode() {};

template <class T>
bool TreeNode<T>::IsRoot()
{
	if (!parent)
		return true;
	else
		return false;
}

template<class T>
bool TreeNode<T>::FindKey(T key, unsigned int& index)
{
	unsigned int left, right, locate;

	if (!key_num) {
		index = 0;
		return false;
	}
	else {
		if (keys[0] > key) {
			index = 0;
			return false;
		}
		else if (keys[key_num - 1] < key) {
			index = key_num;
			return false;
		}
		else if (key_num < 30) {
			for (unsigned int i = 0; i < key_num; i++) {
				if (keys[i] == key)
					return true;
				else if (keys[i] < key)
					continue;
				else {
					index = i;
					return false;
				}
			}
		}
		else {
			left = locate = 0;
			right = key_num - 1;

			while (right >= left) {
				locate = (left + right) / 2;
				if (keys[locate] == key) {
					index = locate;
					return true;
				}
				else if (keys[locate] > key)
					right = locate;
				else
					left = locate;
			}

			if (keys[left] > key)
				index = left;
			else if (keys[right] < key)
				index = right;

			return false;
		}
	}
}

template <class T>
TreeNode<T>* TreeNode<T>::SplitNode(T& key)
{
	unsigned int min_node_num = (degree - 1) / 2;
	TreeNode* new_node = new TreeNode(degree, this->isleaf);

	if (!isleaf) {
		//��÷��ѽڵ㸸�ڵ�keyֵ
		key = keys[min_node_num];

		for (unsigned int i = 0; i < (degree + 1) - (min_node_num + 1) + 1; i++) {
			//����keyֵ,ԭ����key��0����
			if (i < degree - min_node_num) {
				new_node->keys[i] = this->keys[i + min_node_num + 1];
				this->keys[i + min_node_num + 1] = T();
			}
			//�����ӽ��ָ��,ԭ����ָ����NULL����
			new_node->children[i] = this->children[i + min_node_num + 1];
			new_node->children[i]->parent = new_node;
			this->children[i + min_node_num + 1] = NULL;
		}
		//����key��������ڵ��໥λ��
		new_node->parent = this->parent;
		this->keys[min_node_num] = T();
		this->key_num = new_node->key_num = min_node_num;
	}
	else {
		key = keys[min_node_num + 1];

		for (unsigned int i = 0; i < degree - min_node_num; i++) {
			new_node->keys[i] = keys[i + min_node_num + 1];
			new_node->vals[i] = this->vals[i + min_node_num + 1];

			keys[i + min_node_num + 1] = T();
			vals[i + min_node_num + 1] = 0;
		}

		new_node->nextLeaf = this->nextLeaf;
		new_node->parent = this->parent;
		this->nextLeaf = new_node;

		new_node->key_num = min_node_num;
		this->key_num = new_node->key_num + 1;
	}

	return new_node;
}

template <class T>
unsigned int TreeNode<T>::AddKey(T& key)
{
	unsigned int index = 0;
	bool exist = FindKey(key, index);
	if (!key_num) {
		keys[0] = key;
		++key_num;
		return 0;
	}
	else {
		if (!exist) {
			//���������key���ӽڵ�ָ��λ��
			for (unsigned int i = key_num; i > index; i--) {
				keys[i] = keys[i - 1];
				children[i] = children[i - 1];
			}

			key_num++;
			keys[index] = key;
			children[index + 1] = NULL;

			return index;
		}
		else {
			cout << "error in Add (T& key), key has already existed!!" << endl;
			exit(0);
		}
	}
}

template <class T>
unsigned int TreeNode<T>::AddKey(T& key, int value)
{
	unsigned int index = 0;
	bool exist = FindKey(key, index);

	if (!isleaf) {
		cout << "error in Add(T & key), this isn't a leaf node!! " << endl;
		return -1;
	}

	if (!key_num) {
		key_num++;
		keys[0] = value;
		vals[0] = value;
		return 0;
	}
	else {
		if (!exist) {
			for (unsigned int i = key_num; i > index; i--) {
				keys[i] = keys[i - 1];
				vals[i] = keys[i - 1];
			}
			key_num++;
			keys[index] = key;
			vals[index] = value;
			return index;
		}
		else {
			cout << "error in Add (T& key), key has already existed!!" << endl;
			exit(0);
		}
	}
	return 0;
}

template<class T>
bool TreeNode<T>::DeleteKeyByIndex(unsigned int index)
{
	if (index > key_num) {
		cout << "error in DeleteKeyByIndex(unsigned int index), index is more than key_num!" << endl;
		return false;
	}
	else {
		if (!isleaf) {
			for (unsigned int i = index; i < key_num - 1; i++) {
				keys[i] = keys[i + 1];
				children[i + 1] = children[i + 2];
			}
			keys[key_num - 1] = T();
			children[key_num] = NULL;
		}
		else {
			for (unsigned int i = index; i < key_num - 1; i++) {
				keys[i] = keys[i + 1];
				vals[i] = vals[i + 1];
			}

			keys[key_num - 1] = T();
			vals[key_num - 1] = 0;
		}

		key_num--;
		return true;
	}
	return false;
}

template <class T>
TreeNode<T>* TreeNode<T>::nextLeaf()
{
	return next;
}

template <class T>
bool TreeNode<T>::FindHigher(unsigned int index, T& key, vector<int>& values)
{
	unsigned int i;

	for (i = index; i < key_num && keys[i] <= key; i++)
		values.push_back(vals[i]);

	if (keys[i] >= key)
		return true;
	else
		return false;
}

template <class T>
bool TreeNode<T>::FindLower(unsigned int index, vector<int>& values)
{
	unsigned int i;

	for (i = index; i < key_num; i++)
		values.push_back(vals[i]);

	return false;
}


/*Class BPlusTree*/

template<class T>
BPlusTree<T>::BPlusTree(string m_name, int key_size, int degree) :
	file_name(m_name),
	key_num(0),
	level(0),
	node_num(0),
	root(NULL),
	leaf_head(NULL),
	key_size(key_size),
	degree(degree)
{
	InitTree();
	ReadFromDiskAll();
}

template<class T>
BPlusTree<T>::~BPlusTree()
{
	DropTree(root);
	key_num = 0;
	root = NULL;
	level = 0;
}

template<class T>
void BPlusTree<T>::InitTree()
{
	root = new TreeNode<T>(degree, true);
	leaf_head = root;
	key_num = 0;
	level = node_num = 1;
}

template<class T>
void BPlusTree<T>::FindToLeaf(Tree node, T key, SearchNodeParse& snp)
{
	unsigned int index = 0;

	if (node->FindKey(key, index)) {
		if (!(node->isleaf)) {
			node = node->children[index + 1];

			while (!node->isleaf)
				node = node->children[0];

			snp.pNode = node;
			snp.index = 0;
			snp->ifFound = true;
		}
		else {
			snp.pNode = node;
			snp.index = index;
			snp.ifFound = true;
		}
	}
	else {
		if (!(node->isleaf))
			FindToLeaf(node->children[index], key, snp);
		else {
			snp.pNode = node;
			snp.index = index;
			snp.ifFound = false;
		}
	}
}

template <class T>
bool BPlusTree<T>::InsertKey(T& key, int val)
{
	SearchNodeParse snp;
	if (!root)
		InitTree();
	FindToLeaf(root, key, snp);
	if (snp.ifFound) {
		cout << "error in InsertKey, a duplicated key!!" << endl;
		return false;
	}
	else {
		key_num++;
		snp.pNode->Addkey(key, val);
		if (snp.pNode->key_num == degree)
			AdjustAfterInsert(snp.pNode);
		return true;
	}
	return false;
}

template<class T>
bool BPlusTree<T>::AdjustAfterInsert(Tree node)
{
	T key;
	unsigned int index;
	Tree parent;
	Tree newnode = node->SplitNode(key);
	node_num++;

	if (!(node->IsRoot())) {
		parent = node->parent;
		index = parent->AddKey(key);
		parent->children[index + 1] = newnode;
		newnode->parent = parent;

		if (parent->key_num == degree)
			return AdjustAfterInsert(parent);

		return true;
	}
	else {
		if (!root) {
			cout << "error in AdjustAfterInsert, cannot allocate memory for the new root!!" << endl;
			exit(0);
		}
		else {
			node_num++;
			level++;
			this->root = root;
			newnode->parent = node->parent = root;
			root->AddKey(key);
			root->children[0] = node;
			root->children[1] = newnode;
			return true;
		}
		return false;
	}

	return false;
}
template <class T>
int BPlusTree<T>::SearchVal(T& key)
{
	if (!root) {
		cout << "error in SearchVal, the tree is empty!!" << endl;
		return -1;
	}
	SearchNodeParse snp;
	FindToLeaf(root, key, snp);

	if (!snp.ifFound)
		return -1;
	else
		return snp.pNode->vals[snp.index];
}

template <class T>
bool BPlusTree<T>::DeleteKey(T& key)
{
	SearchNodeParse snp;
	unsigned int index = 0;
	Tree current_parent;
	bool if_found;

	if (!root) {
		cout << "error in DeleteKey, the tree is empty!!" << endl;
		return false;
	}
	else {
		FindToLeaf(root, key, snp);
		if (!snp.ifFound) {
			cout << "error in DeleteKey, no such key!!" << endl;
			return false;
		}
		else {
			if (snp.pNode->IsRoot()) {
				key_num--;
				snp.pNode->DeleteKeyByIndex(snp.index);
				return AdjustAfterDelete();
			}
			else {
				if (!snp.index && leaf_head != snp.pNode) {
					current_parent = snp.pNode->parent;
					if_found = current_parent->FindKey(key, index);

					while (!if_found) {
						if (current_parent->parent)
							current_parent = current_parent->parent;
						else
							break;

						if_found = current_parent->FindKey(key, index);
					}

					key_num--;
					current_parent->keys[index] = snp.pNode->keys[1];
					snp.pNode->DeleteKeyByIndex(snp.index);
					return AdjustAfterDelete();
				}
				else {
					key_num--;
					snp.pNode->DeleteKeyByIndex(snp.index);
					return AdjustAfterDelete();
				}
			}
		}
	}
	return false;
}


template <class T>
bool BPlusTree<T>::AdjustAfterDelete(Tree node)
{
	unsigned int min_key_num = (degree - 1) / 2;
	unsigned int index = 0;
	//���ֲ���Ҫ���������
	if (((node->isLeaf) && (node->num >= min_key_num)) ||
		((degree != 3) && (!node->isLeaf) && (node->num >= min_key_num - 1)) ||
		((degree == 3) && (!node->isLeaf) && (node->num < 0))) {
		return  true;

	}

	if (node->IsRoot()) { //��ǰ���Ϊ�����
		if (node->num > 0) //����Ҫ����
			return true;
		else { //������Ҫ����
			if (root->isleaf()) { //����Ϊ�������
				delete node;
				leaf_head = root = NULL;
				level--;
				node_num--;
			}
			else { //���ڵ㽫��Ϊ��ͷ��
				root = node->children[0];
				root->parent = NULL;
				delete node;
				level--;
				node_num--;
			}
		}
	}
	else { //�Ǹ��ڵ����
		Tree parent = node->parent, brother = NULL;
		if (node->isleaf()) { //��ǰΪҶ�ڵ�
			parent->findKey(node->keys[0], index);
			//ѡ�����ֵ�
			if ((parent->children[0] != node) && (index + 1 == parent->key_num)) {
				brother = parent->children[index];
				if (brother->key_num > min_key_num) {
					for (unsigned int i = node->key_num; i > 0; i--) {
						node->keys[i] = node->keys[i - 1];
						node->vals[i] = node->vals[i - 1];
					}

					node->keys[0] = brother->keys[brother->key_num - 1];
					node->vals[0] = brother->vals[brother->key_num - 1];
					brother->DeleteKeyByIndex(brother->key_num - 1);

					node->key_num++;
					parent->keys[index] = node->keys[0];

					return true;
				}
				else {
					parent->DeleteKeyByIndex(index);

					for (unsigned int i = 0; i < node->key_num; i++) {
						brother->keys[i + brother->key_num] = node->keys[i];
						brother->vals[i + brother->key_num] = node->vals[i];
					}
					brother->key_num += node->key_num;
					brother->next = node->next;

					delete node;
					node_num--;

					return AdjustAfterDelete(parent);
				}

			}
			else {
				if (parent->children[0] == node)
					brother = parent->children[1];
				else
					brother = parent->children[index + 2];
				if (brother->key_num > min_key_num) {
					node->keys[node->key_num] = brother->keys[0];
					node->vals[node->key_num] = brother->vals[0];
					node->key_num++;
					brother->DeleteKeyByIndex(0);
					if (parent->children[0] == node)
						parent->keys[0] = brother->keys[0];
					else
						parent->keys[index + 1] = brother->keys[0];
					return true;

				}
				else {
					for (int i = 0; i < brother->key_num; i++) {
						node->keys[node->key_num + i] = brother->keys[i];
						node->vals[node->key_num + i] = brother->vals[i];
					}
					if (node == parent->children[0])
						parent->DeleteKeyByIndex(0);
					else
						parent->DeleteKeyByIndex(index + 1);
					node->key_num += brother->key_num;
					node->next = brother->next;

					delete brother;
					node_num--;

					return AdjustAfterDelete(parent);
				}
			}

		}
		else { //֦�ɽڵ����
			unsigned int index = 0;
			parent->FindKey(node->children[0]->keys[0], index);
			if ((parent->children[0] != node) && (index + 1 == parent->key_num)) {
				brother = parent->children[index];
				if (brother->key_num > min_key_num - 1) {
					node->children[node->key_num + 1] = node->children[node->num];
					for (unsigned int i = node->key_num; i > 0; i--) {
						node->children[i] = node->children[i - 1];
						node->keys[i] = node->keys[i - 1];
					}
					node->children[0] = brother->children[brother->key_num];
					node->keys[0] = parent->keys[index];
					node->num++;

					parent->keys[index] = brother->keys[brother->key_num - 1];

					if (brother->children[brother->key_num])
						brother->children[brother->key_num]->parent = node;
					brother->DeleteKeyByIndex(brother->key_num - 1);

					return true;

				}
				else {
					brother->key_num++;
					brother->keys[brother->key_num] = parent->keys[index];
					parent->DeleteKeyByIndex(index);

					for (unsigned int i = 0; i < node->key_num; i++) {
						brother->children[brother->key_num + i] = node->children[i];
						brother->keys[brother->key_num + i] = node->keys[i];
						brother->children[brother->key_num + i]->parent = brother;
					}
					brother->children[brother->key_num + node->key_num] = node->children[node->key_num];
					brother->children[brother->key_num + node->key_num]->parent = brother;

					brother->key_num += node->key_num;

					delete node;
					node_num--;

					return AdjustAfterDelete(parent);
				}

			}
			else {
				if (parent->children[0] == node)
					brother = parent->children[1];
				else
					brother = parent->children[index + 2];
				if (brother->key_num > min_key_num - 1) {

					node->children[node->key_num + 1] = brother->children[0];
					node->keys[node->key_num] = brother->keys[0];
					node->children[node->key_num + 1]->parent = node;
					node->key_num++;

					if (node == parent->children[0])
						parent->keys[0] = brother->keys[0];
					else
						parent->keys[index + 1] = brother->keys[0];

					brother->children[0] = brother->children[1];
					brother->DeleteKeyByIndex(0);

					return true;
				}
				else {

					node->keys[node->num] = parent->keys[index];

					if (node == parent->children[0])
						parent->DeleteKeyByIndex(0);
					else
						parent->DeleteKeyByIndex(index + 1);

					node->key_num++;

					for (unsigned int i = 0; i < brother->key_num; i++) {
						node->children[node->key_num + i] = brother->children[i];
						node->keys[node->key_num + i] = brother->keys[i];
						node->children[node->key_num + i]->parent = node;
					}
					node->children[node->key_num + brother->key_num] = brother->children[brother->key_num];
					node->children[node->key_num + brother->key_num]->parent = node;

					node->key_num += brother->key_num;

					delete brother;
					node_num--;

					return AdjustAfterDelete(parent);
				}
			}
		}
	}
	return false;
}

template <class T>
void BPlusTree<T>::DropTree(Tree node)
{
	if (!node)
		return;
	if (!node->isleaf)
		for (unsigned int i = 0; i <= node->key_num; i++) {
			DropTree(node->children[i]);
			node->children[i] = NULL;
		}

	delete  node;
	node_num--;
}

template <class T>
void BPlusTree<T>::SearchRange(T& key1, T& key2, vector<int>& vals, int flag)
{
	SearchNodeParse snp1, snp2;
	bool finished;
	unsigned int index;
	Tree node;

	if (!root)
		return;
	if (flag == 2) {
		FindToLeaf(root, key1, snp1);
		finished = false;
		node = snp1.pNode;
		index = snp1.index;

		do {
			finished = node->FindLower(index, vals);
			index = 0;
			if (!(node->next))
				break;
			else
				node = node->nextLeaf();
		} while (!finished);
	}
	else if (flag == 1) {
		FindToLeaf(root, key2, snp2);

		finished = false;
		node = snp2.pNode;
		index = snp2.index;

		do {
			finished = node->FindRange2(index, vals);
			index = 0;
			if (!(node->nextLeafNode))
				break;
			else
				node = node->nextLeaf();
		} while (!finished);
	}
	else {
		FindToLeaf(root, key1, snp1);
		FindToLeaf(root, key2, snp2);
		finished = false;

		if (key1 > key2) {
			node = snp2.pNode;
			index = snp2.index;

			do {
				finished = node->FindRange(index, key1, vals);
				index = 0;
				if (!(node->nextLeafNode))
					break;
				else
					node = node->nextLeaf();
			} while (!finished);
		}
		else {
			node = snp1.pNode;
			index = snp1.index;

			do {
				finished = node->FindRange(index, key2, vals);
				index = 0;
				if (!(node->nextLeafNode))
					break;
				else
					node = node->nextLeaf();
			} while (!finished);
		}
	}
	//����ȥ�������ظ�Ԫ��
	sort(vals.begin(), vals.end());
	vals.erase(unique(vals.begin(), vals.end()), vals.end());
}

template <class T>
void BPlusTree<T>::GetFile(string fname)
{
	FILE* fp = fopen(fname.c_str(), "w+");
	fclose(fp);
}

template <class T>
int BPlusTree<T>::GetBlockNum(string table_name)
{
	char* p;
	int block_num = -1;
	do {
		p = buffer_manager.getPage(table_name, block_num + 1);
		block_num++;
	} while (p[0] != '\0');
	return block_num;
}

template <class T>
void BPlusTree<T>::ReadFromDiskAll()
{
	string fname = "./database/index/" + file_name;
	//string fname = file_name;
	GetFile(fname);
	int block_num = GetBlockNum(fname);

	if (block_num <= 0)
		block_num = 1;

	for (int i = 0; i < block_num; i++) {
		//��ȡ��ǰ��ľ��
		char* p = buffer_manager.getPage(fname, i);
		//char* t = p;
		//�����������м�¼

		ReadFromDisk(p, p + PAGESIZE);
	}
}

template <class T>
void BPlusTree<T>::ReadFromDisk(char* p, char* end)
{
	T key;
	int j, value;
	char temp[100];

	for (unsigned int i = 0; i < PAGESIZE; i++)
		if (p[i] != '#')
			return;
		else {
			i += 2;

			for (j = 0; i < PAGESIZE && p[i] != ' '; i++)
				temp[j++] = p[i];
			temp[j] = '\0';
			string s(temp);
			stringstream stream(s);
			stream >> key;

			memset(temp, 0, sizeof(temp));

			i++;
			for (j = 0; i < PAGESIZE && p[i] != ' '; i++)
				temp[j++] = p[i];
			temp[j] = '\0';
			string s1(temp);
			stringstream stream1(s1);
			stream1 >> value;

			InsertKey(key, value);
		}
}


template <class T>
void BPlusTree<T>::WrittenBackToDiskAll()
{
	int i, j, block_num, offset, page_id;
	string fname = "./database/index/" + file_name;
	//string fname = file_name;
	GetFile(fname);
	block_num = GetBlockNum(fname);

	Tree ntmp = leaf_head;

	for (j = 0, i = 0; ntmp != NULL; j++) {
		char* p = buffer_manager.getPage(fname, j);
		offset = 0;
		memset(p, 0, PAGESIZE);

		for (i = 0; i < ntmp->key_num; i++) {
			p[offset++] = '#';
			p[offset++] = ' ';

			copyString(p, offset, ntmp->keys[i]);
			p[offset++] = ' ';
			copyString(p, offset, ntmp->vals[i]);
			p[offset++] = ' ';
		}

		p[offset] = '\0';

		page_id = buffer_manager.getPageId(fname, j);
		buffer_manager.modifyPage(page_id);

		ntmp = ntmp->nextLeafNode;
	}

	while (j < block_num) {
		char* p = buffer_manager.getPage(fname, j);
		memset(p, 0, PAGESIZE);

		int page_id = buffer_manager.getPageId(fname, j);
		buffer_manager.modifyPage(page_id);

		j++;
	}
}


template <class T>
void BPlusTree<T>::PrintLeaf()
{
	Tree p = leaf_head;
	while (p != NULL) {
		p->PrintList();
		p = p->nextLeaf();
	}

	return;
}

template <class T>
void TreeNode<T>::PrintList()
{
	for (int i = 0; i < key_num; i++)
		cout << "->" << keys[i];
	cout << endl;
}

template <class T>
string BPlusTree<T>::findTableName(data index)
{
	if (index.type == TYPE_INT) {
		if (this->SearchVal(index.int_data) != -1)
			return file_name;
	 }
	else if (index.type == TYPE_FLOAT) {
		if (this->SearchVal(index.float_data) != -1)
			return file_name;
	}
	else {
		if (this->SearchVal(index.char_data) != -1)
			return file_name;
	}
	string default_ = "error!";
	return default_;
}
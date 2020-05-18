#pragma once
#include "BPTree.h"
#include <iostream>
#include <algorithm>

using namespace std;

bool BPT::Insert(KEYTYPE key, const DATATYPE& data)
{
	if (Search(key))
		return false;
	if (!root) {
		root = new LeafNode();
		head = (LeafNode*)root;
		maxval = key;
	}
	if (root->GetKeyNum() >= max_keynum) {
		InternalNode* newnode = new InternalNode();
		newnode->SetChild(0, root);
		root->Split(newnode, 0);
		root = newnode;
	}
	if (key > maxval)
		maxval = key;
	RecursiveInsert(root, key, data);
	return true;
}

void BPT::RecursiveInsert(TreeNode* parent, KEYTYPE key, const DATATYPE& data)
{
	if (parent->GetType() == LEAF)  // 叶子结点，直接插入
	{
		((LeafNode*)parent)->Insert(key, data);
	}
	else
	{
		// 找到子结点
		int keyindex = parent->GetKeyIndex(key);
		int childindex = parent->GetChildIndex(key, keyindex); // 孩子结点指针索引
		TreeNode* childNode = ((InternalNode*)parent)->GetChild(childindex);
		if (childNode->GetKeyNum() >= max_leafnum)  // 子结点已满，需进行分裂
		{
			childNode->Split(parent, childindex);
			if (parent->GetKeyValue(childindex) <= key)   // 确定目标子结点
			{
				childNode = ((InternalNode*)parent)->GetChild(childindex + 1);
			}
		}
		RecursiveInsert(childNode, key, data);
	}
}

bool BPT::Update(KEYTYPE oldkey, KEYTYPE newkey)
{
	if (Search(newkey))
		return false;
	else {
		int dataval;
		Remove(oldkey, dataval);
		if (dataval == Invalid_index)
			return false;
		else
			return Insert(newkey, dataval);
	}
}


void BPT::Clear()
{
	if (root) {
		root->Clear();
		delete root;
		root = NULL;
		head = NULL;
	}
}

void BPT::print()
{
	PrintInConcavo(root, 10);
}
void BPT::printdata()
{
	LeafNode* iterator = head;
	while (!iterator) {
		for (int i = 0; i < iterator->GetKeyNum(); i++)
			cout << iterator->GetData(i) << " ";
		cout << endl;
		iterator = iterator->GetRight();
	}
}

void BPT::PrintInConcavo(TreeNode* TreeNode, int cnt)
{
	if (!TreeNode) {
		int i, j;
		for (i = 0; i < TreeNode->GetKeyNum(); i++) {
			if (TreeNode->GetType() != LEAF)
				PrintInConcavo(((InternalNode*)TreeNode)->GetChild(i), cnt - 2);
			for (j = cnt; j >= 0; j--)
				cout << "-";
			cout << TreeNode->GetKeyValue(i) << endl;
		}
		if(TreeNode->GetType()!=LEAF)
			PrintInConcavo(((InternalNode*)TreeNode)->GetChild(i), cnt - 2);
	}
}

bool BPT::Search(KEYTYPE key)
{
	return RecursiveSearch(root, key);
}

bool BPT::RecursiveSearch(TreeNode* TreeNode, KEYTYPE key)
{
	if (!TreeNode)
		return false;
	else {
		int keyindex = TreeNode->GetKeyIndex(key);
		int childindex = TreeNode->GetChildIndex(key, keyindex);
		if (keyindex < TreeNode->GetKeyNum() && key == TreeNode->GetKeyValue(keyindex))
			return true;
		else 
			return RecursiveSearch(((InternalNode*)TreeNode)->GetChild(childindex), key);
	}
}


bool BPT::Remove(KEYTYPE key)
{
	if (!Search(key))
		return false;
	if (root->GetKeyNum() == 1) {
		if (root->GetType() == LEAF) {
			Clear();
			return true;
		}
		else {
			TreeNode* child1 = ((InternalNode*)root)->GetChild(0);
			TreeNode* child2 = ((InternalNode*)root)->GetChild(1);
			if (child1->GetKeyNum() == min_keynum && child2->GetKeyNum() == min_keynum) {
				child1->Merge(root, child2, 0);
				root = child1;
			}
		}
	}
	RecursiveRemove(root, key);
	return true;
}

void BPT::Remove(KEYTYPE key, DATATYPE& dataval)
{
	if (!Search(key)) {
		dataval = Invalid_index;
		return;
	}
	if (root->GetKeyNum() == 1) {
		if (root->GetType() == LEAF) {
			dataval = ((LeafNode*)root)->GetData(0);
			Clear();
			return;
		}
		else {
			TreeNode* child1 = ((InternalNode*)root)->GetChild(0);
			TreeNode* child2 = ((InternalNode*)root)->GetChild(1);
			if (child1->GetKeyNum() == min_keynum && child2->GetKeyNum() == min_keynum)
			{
				child1->Merge(root, child2, 0);
				delete root;
				root = child1;
			}
		}
	}
	RecursiveRemove(root, key, dataval);
}

void BPT::RecursiveRemove(TreeNode* parent, KEYTYPE key)
{
	int keyindex = parent->GetKeyIndex(key);
	int childindex = parent->GetChildIndex(key, keyindex); // 孩子结点指针索引
	if (parent->GetType() == LEAF)// 找到目标叶子节点
	{
		if (key == maxval && keyindex > 0)
		{
			maxval = parent->GetKeyValue(keyindex - 1);
		}
		parent->RemoveKey(keyindex, childindex);  // 直接删除
		// 如果键值在内部结点中存在，也要相应的替换内部结点
		if (childindex == 0 && root->GetType() != LEAF && parent != head)
		{
			ChangeKey(root, key, parent->GetKeyValue(0));
		}
	}
	else // 内结点
	{
		TreeNode* child = ((InternalNode*)parent)->GetChild(childindex); //包含key的子树根节点
		if (child->GetKeyNum() == min_keynum)                       // 包含关键字达到下限值，进行相关操作
		{
			TreeNode* left = childindex > 0 ? ((InternalNode*)parent)->GetChild(childindex - 1) : NULL;                       //左兄弟节点
			TreeNode* right = childindex < parent->GetKeyNum() ? ((InternalNode*)parent)->GetChild(childindex + 1) : NULL;//右兄弟节点
			// 先考虑从兄弟结点中借
			if (left && left->GetKeyNum() > min_keynum)// 左兄弟结点可借
			{
				child->BorrowNode(left, parent, childindex - 1, LEFT);
			}
			else if (right && right->GetKeyNum() > min_keynum)//右兄弟结点可借
			{
				child->BorrowNode(right, parent, childindex, RIGHT);
			}
			//左右兄弟节点都不可借，考虑合并
			else if (left)                    //与左兄弟合并
			{
				left->Merge(parent, child, childindex - 1);
				child = left;
			}
			else if (right)                   //与右兄弟合并
			{
				child->Merge(parent, right, childindex);
			}
		}
		RecursiveRemove(child, key);
	}
}

void BPT::RecursiveRemove(TreeNode * parent, KEYTYPE key, DATATYPE & dataval)
{
	int keyindex = parent->GetKeyIndex(key);
	int childindex = parent->GetChildIndex(key, keyindex); // 孩子结点指针索引
	if (parent->GetType() == LEAF)// 找到目标叶子节点
	{
		if (key == maxval && keyindex > 0)
		{
			maxval = parent->GetKeyValue(keyindex - 1);
		}
		dataval = ((LeafNode*)parent)->GetData(keyindex);
		parent->RemoveKey(keyindex, childindex);  // 直接删除
		// 如果键值在内部结点中存在，也要相应的替换内部结点
		if (childindex == 0 && root->GetType() != LEAF && parent != head)
		{
			ChangeKey(root, key, parent->GetKeyValue(0));
		}
	}
	else // 内结点
	{
		TreeNode* childnode = ((InternalNode*)parent)->GetChild(childindex); //包含key的子树根节点
		if (childnode->GetKeyNum() == min_keynum)                       // 包含关键字达到下限值，进行相关操作
		{
			TreeNode* left = childindex > 0 ? ((InternalNode*)parent)->GetChild(childindex - 1) : NULL;                       //左兄弟节点
			TreeNode* right = childindex < parent->GetKeyNum() ? ((InternalNode*)parent)->GetChild(childindex + 1) : NULL;//右兄弟节点
			// 先考虑从兄弟结点中借
			if (left && left->GetKeyNum() > min_keynum)// 左兄弟结点可借
			{
				childnode->BorrowNode(left, parent, childindex - 1, LEFT);
			}
			else if (right && right->GetKeyNum() > min_keynum)//右兄弟结点可借
			{
				childnode->BorrowNode(right, parent, childindex, RIGHT);
			}
			//左右兄弟节点都不可借，考虑合并
			else if (left)                    //与左兄弟合并
			{
				left->Merge(parent, childnode, childindex - 1);
				childnode = left;
			}
			else if (right)                   //与右兄弟合并
			{
				childnode->Merge(parent, right, childindex);
			}
		}
		RecursiveRemove(childnode, key, dataval);
	}
}

void BPT::ChangeKey(TreeNode* TreeNode, KEYTYPE oldKey, KEYTYPE newKey)
{
	if (TreeNode != NULL && TreeNode->GetType() != LEAF)
	{
		int index = TreeNode->GetKeyIndex(oldKey);
		if (index < TreeNode->GetKeyNum() && oldKey == TreeNode->GetKeyValue(index))  // 找到
		{
			TreeNode->SetKeyValue(index, newKey);
		}
		else   // 继续找
		{
			ChangeKey(((InternalNode*)TreeNode)->GetChild(index), oldKey, newKey);
		}
	}
}

vector<DATATYPE> BPT::Select(KEYTYPE key, int opera)
{
	vector<DATATYPE> results;
	if (root != NULL)
	{
		if (key > maxval)   // 比较键值大于B+树中最大的键值
		{
			if (opera == BE || opera == BL)
				for (LeafNode* iterator = head; iterator != NULL; iterator = iterator->GetRight())
					for (int i = 0; i < iterator->GetKeyNum(); ++i)
						results.push_back(iterator->GetData(i));
		}
		else if (key < head->GetKeyValue(0))  // 比较键值小于B+树中最小的键值
		{
			if (opera == AE || opera == AB)
				for (LeafNode* iterator = head; iterator != NULL; iterator = iterator->GetRight())
					for (int i = 0; i < iterator->GetKeyNum(); ++i)
						results.push_back(iterator->GetData(i));
		}
		else  // 比较键值在B+树中
		{
			RESULT result;
			Search(key, result);
			switch (opera)
			{
				case BL:
				case BE:
				{
					LeafNode* iterator = head;
					int i;
					while (iterator != result.dest)
					{
						for (i = 0; i < iterator->GetKeyNum(); ++i)
						{
							results.push_back(iterator->GetData(i));
						}
						iterator = iterator->GetRight();
					}
					for (i = 0; i < result.index; ++i)
					{
						results.push_back(iterator->GetData(i));
					}
					if (iterator->GetKeyValue(i) < key ||
						(opera == BE && key == iterator->GetKeyValue(i)))
					{
						results.push_back(iterator->GetData(i));
					}
				}
				break;
				case EQ:
				{
					if (result.dest->GetKeyValue(result.index) == key)
					{
						results.push_back(result.dest->GetData(result.index));
					}
				}
				break;
				case AE:
				case AB:
				{
					LeafNode* iterator = result.dest;
					if (key < iterator->GetKeyValue(result.index) ||
						(opera == BE && key == iterator->GetKeyValue(result.index))
						)
					{
						results.push_back(iterator->GetData(result.index));
					}
					int i;
					for (i = result.index + 1; i < iterator->GetKeyNum(); ++i)
					{
						results.push_back(iterator->GetData(i));
					}
					iterator = iterator->GetRight();
					while (iterator != NULL)
					{
						for (i = 0; i < iterator->GetKeyNum(); ++i)
						{
							results.push_back(iterator->GetData(i));
						}
						iterator = iterator->GetRight();
					}
				}
				break;
				default:  // 范围查询
					break;
			}
		}
	}
	sort<vector<DATATYPE>::iterator>(results.begin(), results.end());
	return results;
}

vector<DATATYPE> BPT::Select(KEYTYPE smallKey, KEYTYPE largeKey)
{
	vector<DATATYPE> results;
	if (smallKey <= largeKey)
	{
		RESULT start, end;
		Search(smallKey, start);
		Search(largeKey, end);
		LeafNode* iterator = start.dest;
		int i = start.index;
		if (iterator->GetKeyValue(i) < smallKey)
		{
			++i;
		}
		if (end.dest->GetKeyValue(end.index) > largeKey)
		{
			--end.index;
		}
		while (iterator != end.dest)
		{
			for (; i < iterator->GetKeyNum(); ++i)
			{
				results.push_back(iterator->GetData(i));
			}
			iterator = iterator->GetRight();
			i = 0;
		}
		for (; i <= end.index; ++i)
		{
			results.push_back(iterator->GetData(i));
		}
	}
	sort<vector<DATATYPE>::iterator>(results.begin(), results.end());
	return results;
}

void BPT::Search(KEYTYPE key, RESULT& result)
{
	RecursiveSearch(root, key, result);
}

void BPT::RecursiveSearch(TreeNode* TreeNode, KEYTYPE key, RESULT& result)
{
	int index = TreeNode->GetKeyIndex(key);
	int childindex = TreeNode->GetChildIndex(key, index); // 孩子结点指针索引
	if (TreeNode->GetType() == LEAF)
	{
		result.index = index;
		result.dest = (LeafNode*)TreeNode;
		return;
	}
	else
	{
		return RecursiveSearch(((InternalNode*)TreeNode)->GetChild(childindex), key, result);
	}

}
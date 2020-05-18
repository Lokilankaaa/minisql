#include <iostream>
#include "BPlusTree.h"

using namespace std;
//二分法查找key的值在当前节点的下标
int TreeNode::GetKeyIndex(KEYTYPE key)
{
	int left = 0;
	int right = GetKeyNum() - 1;
	int current;
	while (left != right) {
		current = (left + right) / 2;
		KEYTYPE currentkey = GetKeyValue(current);
		if (key > currentkey)
			left = current + 1;
		else
			right = current;
	}
	return left;
}


//InternalNode
//递归删除所有子树
void InternalNode::Clear()
{
	for (int i = 0; i <= degree; i++)
	{
		children[i]->Clear();
		delete children[i];
		children[i] = NULL;
	}
}

void InternalNode::Split(TreeNode * parent, int childindex)
{
	InternalNode* newnode = new InternalNode;
	newnode->SetKeyNum(min_keynum);
	for (int i = 0; i < min_keynum; ++i)
		newnode->SetKeyValue(i, values[i + min_chinum]);
	for (int j = 0; j < min_chinum; j++)
		newnode->SetChild(j, children[j + min_chinum]);
	SetKeyNum(min_keynum);
	((InternalNode*)parent)->Insert(childindex, childindex + 1, values[min_keynum], newnode);
}

void InternalNode::Insert(int keyindex, int childindex, KEYTYPE key, TreeNode* child)
{
	int i;
	for (i = GetKeyNum(); i < keyindex; i--)
	{
		SetChild(i + 1, children[i]);
		SetKeyValue(i, values[i - 1]);
	}
	if (i == childindex)
		SetChild(i + 1, children[i]);
	SetChild(childindex, child);
	SetKeyValue(keyindex, key);
	SetKeyNum(degree + 1);
}

void InternalNode::Merge(TreeNode* parent, TreeNode* child, int index)
{
	Insert(min_keynum, min_keynum + 1, parent->GetKeyValue(index), ((InternalNode*)child)->GetChild(0));
	for (int i = 1; i <= child->GetKeyNum(); i++)
		Insert(min_keynum + i, min_keynum + i + 1, child->GetKeyValue(i - 1), ((InternalNode*)child)->GetChild(i));
	parent->RemoveKey(index, index + 1);
	delete ((InternalNode*)parent)->GetChild(index + 1);
}

void InternalNode::RemoveKey(int keyindex, int childindex) 
{
	for (int i = 0; i < GetKeyNum() - keyindex - 1; i++)
	{
		SetKeyValue(keyindex + i, GetKeyValue(keyindex + i + 1));
		SetChild(childindex + i, GetChild(childindex + i + 1));
	}
	SetKeyNum(GetKeyNum() - 1);
}

void InternalNode::BorrowNode(TreeNode* dest, TreeNode* parent, int index, Siblings brother)
{
	switch (brother) {
	case LEFT:
		Insert(0, 0, parent->GetKeyValue(index), ((InternalNode*)dest)->GetChild(dest->GetKeyNum()));
		parent->SetKeyValue(index, dest->GetKeyValue(dest->GetKeyNum() - 1));
		dest->RemoveKey(dest->GetKeyNum() - 1, dest->GetKeyNum());
		break;
	case RIGHT:
		Insert(GetKeyNum(), GetKeyNum() + 1, parent->GetKeyValue(index), ((InternalNode*)dest)->GetChild(0));
		parent->SetKeyValue(index, dest->GetKeyValue(0));
		dest->RemoveKey(0, 0);
		break;
	default:
		break;
	}
}

int InternalNode::GetChildIndex(KEYTYPE key, int index)
{
	if (key == GetKeyValue(index))
		return index + 1;
	else
		return index;
}

// LeafNode


void LeafNode::Clear()
{
	for (int i = 0; i < degree; ++i)
	{
		// if type of data is pointer
		//delete data[i];
		//data[i] = NULL;
	}
}

void LeafNode::Insert(KEYTYPE key, const DATATYPE& Data)
{
	int i;
	for (i = degree; i >= 1 && values[i - 1] > key; --i)
	{
		SetKeyValue(i, values[i - 1]);
		SetData(i, data[i - 1]);
	}
	SetKeyValue(i, key);
	SetData(i, Data);
	SetKeyNum(degree + 1);
}

void LeafNode::Split(TreeNode* parent, int index)
{
	LeafNode* newNode = new LeafNode();//分裂后的右节点
	SetKeyNum(min_leafnum);
	newNode->SetKeyNum(min_leafnum + 1);
	newNode->SetRight(GetRight());
	SetRight(newNode);
	newNode->SetLeft(this);
	for (int i = 0; i < min_leafnum + 1; ++i)// 拷贝关键字的值
	{
		newNode->SetKeyValue(i, values[i + min_leafnum]);
	}
	for (int i = 0; i < min_leafnum + 1; ++i)// 拷贝数据
	{
		newNode->SetData(i, data[i + min_leafnum]);
	}
	((InternalNode*)parent)->Insert(index, index + 1, values[min_leafnum], newNode);
}

void LeafNode::Merge(TreeNode* parent, TreeNode* child, int index)
{
	// 合并数据
	for (int i = 0; i < child->GetKeyNum(); ++i)
		Insert(child->GetKeyValue(i), ((LeafNode*)child)->GetData(i));
	SetRight(((LeafNode*)child)->GetRight());
	//父节点删除index的key，
	parent->RemoveKey(index, index + 1);
}

void LeafNode::RemoveKey(int index, int childIndex)
{
	for (int i = index; i < GetKeyNum() - 1; ++i)
	{
		SetKeyValue(i, GetKeyValue(i + 1));
		SetData(i, GetData(i + 1));
	}
	SetKeyNum(GetKeyNum() - 1);
}

void LeafNode::BorrowNode(TreeNode* dest, TreeNode* parent, int index, Siblings brother)
{
	switch (brother)
	{
		case LEFT:  // 从左兄弟结点借
		{
			Insert(dest->GetKeyValue(dest->GetKeyNum() - 1), ((LeafNode*)dest)->GetData(dest->GetKeyNum() - 1));
			dest->RemoveKey(dest->GetKeyNum() - 1, dest->GetKeyNum() - 1);
			parent->SetKeyValue(index, GetKeyValue(0));
		}
		break;
		case RIGHT:  // 从右兄弟结点借
		{
			Insert(dest->GetKeyValue(0), ((LeafNode*)dest)->GetData(0));
			dest->RemoveKey(0, 0);
			parent->SetKeyValue(index, dest->GetKeyValue(0));
		}
		break;
		default:
			break;
	}
}
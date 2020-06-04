#ifndef MINISQL_BPLUSTREE_H
#define MINISQL_BPLUSTREE_H

#include <vector>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "buffer_manager.h"
#include <cstring>

#define TYPE_INT -1
#define TYPE_FLOAT 0

template<typename T>
inline void copyString(char *p, int &offset, T data) {
    std::stringstream stream;
    stream << data;
    std::string s1 = stream.str();
    for (int i = 0; i < s1.length(); i++, offset++)
        p[offset] = s1[i];
}

extern BufferManager buf_manager;

using namespace std;
template <typename T>
class TreeNode {
public:
    //该结点内key数量
    unsigned int key_num;
    //指向父节点指针
    TreeNode* parent;
    //存放key容器
    vector <T> keys;
    //指向子结点的指针容器
    vector <TreeNode*> children;
    vector <int> vals;
    //指向下一个叶结点的指针
    TreeNode* next;
    //此结点是否是叶结点的标志
    bool isleaf;
    //此树的度
    int degree;

public:
    //构造函数
    //输入：树的度，是否为叶结点
    //功能：初始化一个新的结点，若Leaf=true则创建一个叶结点，否则创建一个枝干结点
    TreeNode(int in_degree, bool Leaf = false);
    //析构函数
    ~TreeNode();
    //功能：判断是否为根结点
    bool IsRoot();
    //输入：key值，(index)
    //输出：bool
    //功能：判断结点内是否存在某key值，并通过index引用返回该值index
    //并通过函数返回bool值说明是否找到
    bool FindKey(T key, unsigned int& index);
    //输出：TreeNode指针
    //功能：将一个结点分裂成两个结点(与B+树的分裂有关)
    //新结点为本结点的下一个结点
    //同时功过key引用返回去到上层的key值
    TreeNode* SplitNode(T& key);
    //输入：key值
    //输出：添加的position
    //功能：在枝干结点中添加key值，并返回添加的位置
    unsigned int AddKey(T& key);
    //输入：key值
    //输出：添加的position
    //功能：在叶结点中添加key值，并返回添加的位置
    unsigned int AddKey(T& key, int val);
    //输入：key值对应的index
    //输出：bool
    //功能：在结点中删除index对应的key值
    bool DeleteKeyByIndex(unsigned int index);
    //返回下一个相邻叶结点指针
    TreeNode* nextLeaf();
    //输入:起始index,终止key，返回结果vector容器
    //输出:到达终止key返回true，否则返回flase
    //功能：返回一定范围的value容器
    bool FindHigher(unsigned int index, T& key, vector<int>& values);
    bool FindLower(unsigned int index, vector<int>& values);

    void PrintList();
};

//模板类BPlusTree，用于对整个B+树的操作
//由此模块操作TreeNode模块，并且由此模块与buffer_manager模块进行交互
//完成文件的读写操作
template <typename T>
class BPlusTree {

private:
    //重命名TreeNode指针，方便后边进行操作区分
    typedef TreeNode<T>* Tree;
    //特殊结构，用于临时存储查找的key值及所处的位置，方便进行操作
    struct SearchNodeParse {
        Tree pNode; //包含对应key的结点指针
        unsigned int index; //key在结点中的index
        bool ifFound; //是否找到该key
    };

private:
    //文件名，用于文件读写操作
    string file_name;
    //根结点指针
    Tree root;
    //叶结点的左部头指针
    Tree leaf_head;
    //此树key的数量
    unsigned int key_num;
    //此树的level数量
    unsigned int level;
    //此树的node数量
    unsigned int node_num;
    //fileNode* file; // the filenode of this tree
    //每个key值的size，对于一颗树来说，所有key的size应是相同的
    int key_size;
    //树的度
    int degree;

public:
    //构造函数
    //用于构造一颗新的树，确定m_name,key的size，树的度
    //同时调用其他函数为本树分配内存
    BPlusTree(string m_name, int key_size, int degree);
    //析构函数
    //释放相应的内存
    ~BPlusTree();

    //根据key值返回对应的Value
    int SearchVal(T& key);
    //输入：key值及其value
    //输出：bool
    //功能：在树中插入一个key值
    //返回是否插入成功
    bool InsertKey(T& key, int val);
    //输入：key值
    //输出：bool
    //功能：在树中删除一个key值
    //返回是否删除成功
    bool DeleteKey(T& key);
    //输入：树的根结点
    //功能：删除整棵树并释放内存空间，主要用在析构函数中
    void DropTree(Tree node);
    //输入：key1，key2，返回vals的容器
    //功能：返回范围搜索结果，将value放入vals容器中
    void SearchRange(T& key1, T& key2, vector<int>& vals, int flag);

    //从磁盘读取所有数据
    void ReadFromDiskAll();
    //将新数据写入磁盘
    void WrittenBackToDiskAll();
    //在磁盘中读取某一块的数据
    void ReadFromDisk(char* p, char* end);

    void PrintLeaf();

    string findTableName(data index);
private:
    //初始化B+树，并分配内存空间
    void InitTree();
    //用于插入某key后结点满情况，对整棵树进行调整
    bool AdjustAfterInsert(Tree node);
    //用于删除某key后可能出现结点空情况，对整棵树进行调整
    bool AdjustAfterDelete(Tree node);
    //用于查找某key值所处的叶结点位置
    void FindToLeaf(Tree node, T key, SearchNodeParse& snp);
    //获取文件大小
    void GetFile(string fname);
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
    //degree+1个key指向degree+2个子节点
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
                else if (keys[locate] > key){
                    if(right==locate) break;
                    right = locate;
                }
                else
                {
                    if(left==locate) break;
                    left = locate;
                }
            }

            if (keys[left] > key)
                index = left;
            else if (keys[right] < key)
                index = right;

            return false;
        }
    }
    return false;
}

template <class T>
TreeNode<T>* TreeNode<T>::SplitNode(T& key)
{
    unsigned int min_node_num = (degree - 1) / 2;
    TreeNode* new_node = new TreeNode(degree, this->isleaf);

    if (!isleaf) {
        //获得分裂节点父节点key值
        key = keys[min_node_num];

        for (unsigned int i = 0; i < (degree + 1) - (min_node_num + 1) + 1; i++) {
            //拷贝key值,原来的key以0代替
            if (i < degree - min_node_num) {
                new_node->keys[i] = this->keys[i + min_node_num + 1];
                this->keys[i + min_node_num + 1] = T();
            }
            //拷贝子结点指针,原来的指针以NULL代替
            new_node->children[i] = this->children[i + min_node_num + 1];
            new_node->children[i]->parent = new_node;
            this->children[i + min_node_num + 1] = NULL;
        }
        //调整key的数量与节点相互位置
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

        new_node->next = this->next;
        new_node->parent = this->parent;
        this->next = new_node;

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
            //调整插入后key与子节点指针位置
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
            throw e_index_exist();
        }
    }
}

template <class T>
unsigned int TreeNode<T>::AddKey(T& key, int value)
{
    unsigned int index = 0;
    bool exist = FindKey(key, index);

    if (!isleaf) {
        throw e_index_not_exist();
        return -1;
    }

    if (!key_num) {
        key_num++;
        keys[0] = key;            //修改
        vals[0] = value;
        return 0;
    }
    else {
        if (!exist) {
            for (unsigned int i = key_num; i > index; i--) {
                keys[i] = keys[i - 1];
                vals[i] = vals[i - 1];
            }
            key_num++;
            keys[index] = key;
            vals[index] = value;
            return index;
        }
        else {
            throw e_index_exist();
        }
    }
    return 0;
}

template<class T>
bool TreeNode<T>::DeleteKeyByIndex(unsigned int index)
{
    if (index > key_num) {
        throw e_index_full();
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
            snp.ifFound = true;
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
        throw e_unique_conflict();
        return false;
    }
    else {
        key_num++;
        snp.pNode->AddKey(key, val);
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
            throw e_exit_command();
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
        throw e_index_not_exist();
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
        throw e_index_not_exist();
        return false;
    }
    else {
        FindToLeaf(root, key, snp);
        if (!snp.ifFound) {
            throw e_index_not_exist();
            return false;
        }
        else {
            if (snp.pNode->IsRoot()) {
                key_num--;
                snp.pNode->DeleteKeyByIndex(snp.index);
                return AdjustAfterDelete(snp.pNode);
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
                    return AdjustAfterDelete(snp.pNode);
                }
                else {
                    key_num--;
                    snp.pNode->DeleteKeyByIndex(snp.index);
                    return AdjustAfterDelete(snp.pNode);
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
    //三种不需要调整的情况
    if (((node->isleaf) && (node->key_num >= min_key_num)) ||
        ((degree != 3) && (!node->isleaf) && (node->key_num >= min_key_num - 1)) ||
        ((degree == 3) && (!node->isleaf) && (node->key_num < 0))) {
        return  true;

    }

    if (node->IsRoot()) { //当前结点为根结点
        if (node->key_num > 0) //不需要调整
            return true;
        else { //正常需要调整
            if (root->isleaf) { //将成为空树情况
                delete node;
                leaf_head = root = NULL;
                level--;
                node_num--;
            }
            else { //根节点将成为左头部
                root = node->children[0];
                root->parent = NULL;
                delete node;
                level--;
                node_num--;
            }
        }
    }
    else { //非根节点情况
        Tree parent = node->parent, brother = NULL;
        if (node->isleaf) { //当前为叶节点
            parent->FindKey(node->keys[0], index);
            //选择左兄弟
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
        else { //枝干节点情况
            unsigned int index = 0;
            parent->FindKey(node->children[0]->keys[0], index);
            if ((parent->children[0] != node) && (index + 1 == parent->key_num)) {
                brother = parent->children[index];
                if (brother->key_num > min_key_num - 1) {
                    node->children[node->key_num + 1] = node->children[node->key_num];
                    for (unsigned int i = node->key_num; i > 0; i--) {
                        node->children[i] = node->children[i - 1];
                        node->keys[i] = node->keys[i - 1];
                    }
                    node->children[0] = brother->children[brother->key_num];
                    node->keys[0] = parent->keys[index];
                    node->key_num++;
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

                    node->keys[node->key_num] = parent->keys[index];

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
            finished = node->FindLower(index, vals);
            index = 0;
            if (!(node->next))
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
                finished = node->FindHigher(index, key1, vals);
                index = 0;
                if (!(node->next))
                    break;
                else
                    node = node->nextLeaf();
            } while (!finished);
        }
        else {
            node = snp1.pNode;
            index = snp1.index;

            do {
                finished = node->FindHigher(index, key2, vals);
                index = 0;
                if (!(node->next))
                    break;
                else
                    node = node->nextLeaf();
            } while (!finished);
        }
    }
    //排序并去除所有重复元素
    sort(vals.begin(), vals.end());
    vals.erase(unique(vals.begin(), vals.end()), vals.end());
}

template <class T>
void BPlusTree<T>::GetFile(string fname)
{
    FILE* fp = fopen(fname.c_str(), "w");
    fclose(fp);
}

template <class T>
int BPlusTree<T>::GetBlockNum(string table_name)
{
    char* p;
    int block_num = -1;
    do {
        p = buf_manager.getPage(table_name, block_num + 1);
        block_num++;
    } while (p[0] != '\0');
    return block_num;
}

template <class T>
void BPlusTree<T>::ReadFromDiskAll()
{
    string fname = file_name;
    //string fname = file_name;
//    GetFile(fname);
    int block_num = GetBlockNum(fname);

    if (block_num <= 0)
        block_num = 1;

    for (int i = 0; i < block_num; i++) {
        //获取当前块的句柄
        char* p = buf_manager.getPage(fname, i);
        //char* t = p;
        //遍历块中所有记录

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
    std::string fname = file_name;
    FILE *f = fopen(fname.c_str(), "r");
    if(f==NULL) fname="./database/index/"+file_name;
    fclose(f);
    //string fname = file_name;
    //GetFile(fname);
    block_num = GetBlockNum(fname);

    Tree ntmp = leaf_head;
    
    j = 0;
    char* p = buf_manager.getPage(fname, j);
    offset = 0;
    memset(p, 0, PAGESIZE);
    
    for ( i = 0; ntmp != NULL;) {
        
        for (i = 0; i < ntmp->key_num; i++) {
            
            std::stringstream stream1;
            stream1 << ntmp->keys[i];
            std::string s1 = stream1.str();
            
            std::stringstream stream2;
            stream2 << ntmp->vals[i];
            std::string s2 = stream2.str();
            if(offset+5+s1.length()+s2.length()>=PAGESIZE){
                while(offset<PAGESIZE) p[offset++]='\0';
                page_id = buf_manager.getPageId(fname, j);
                buf_manager.modifyPage(page_id);
                j++;
                char* p = buf_manager.getPage(fname, j);
                offset = 0;
                memset(p, 0, PAGESIZE);
            }
            
            p[offset++] = '#';
            p[offset++] = ' ';

            copyString(p, offset, ntmp->keys[i]);
            p[offset++] = ' ';
            copyString(p, offset, ntmp->vals[i]);
            p[offset++] = ' ';
        }


        ntmp = ntmp->next;
    }
    
    p[offset] = '\0';
    page_id = buf_manager.getPageId(fname, j);
    buf_manager.modifyPage(page_id);
    j++;

    while (j < block_num) {
        char* p = buf_manager.getPage(fname, j);
        memset(p, 0, PAGESIZE);

        int page_id = buf_manager.getPageId(fname, j);
        buf_manager.modifyPage(page_id);

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

#endif //MINISQL_BPLUSTREE_H

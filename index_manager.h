#pragma once 
#include <sstream>
#include <string>
#include <map>
#include "meta.h"
#include "buffer_manager.h"
#include "buffer_manager_page.h"
#include "bplustree.h"

using namespace std;
class IndexManager {
public:
	IndexManager(string table_name);
	~IndexManager();
	//���������ļ���B + ��
	void CreateIndex(string file_path, int keytype);
	//ɾ��������B+�����ļ�
	void DropIndex(string file_path, int keytype);
	//Ѱ������λ��
	int FindIndex(string file_path, data Data);
	//��ָ�������в���һ��key
	void InsertIndex(string file_path, data Data, int block_id);
	//��ָ��������ɾ����Ӧ��Key
	void DeleteIndexByKey(string file_path, data Data);
	//����һ����Χ�ڵ�value
	void SearchRange(string file_path, data Data1, data Data2, vector<int>& vals);
	//����index���ض�Ӧ�������
	//���ص����ļ������������ļ�����
	string FindTableName(data index);

private:
	typedef map<string, BPlusTree<int>*> int_Map;
	typedef map<string, BPlusTree<float>*> float_Map;
	typedef map<string, BPlusTree<string>*> string_Map;

	int static const TYPE_INT = -1;
	int static const TYPE_FLOAT = 0;

	int_Map IntMap_Index;
	float_Map FloatMap_Index;
	string_Map StringMap_Index;

	//����B+���ʺϵ�degree
	int GetDegree(int type);
	//���㲻ͬ����Key��size
	int GetKeySize(int type);

};
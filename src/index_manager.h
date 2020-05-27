#pragma once
#include <sstream>
#include <string>
#include <map>
#include "meta.h"
#include "buffer_manager.h"
#include "bplustree.h"

using namespace std;
class IndexManager {
public:
	IndexManager(string table_name);
	~IndexManager();

	void CreateIndex(string file_path, int keytype);
	void DropIndex(string file_path, int keytype);
	int FindIndex(string file_path, data Data);
	void InsertIndex(string file_path, data Data, int block_id);
	void DeleteIndexByKey(string file_path, data Data);
	void SearchRange(string file_path, data Data1, data Data2, vector<int>& vals);

private:
	typedef map<string, BPlusTree<int>*> int_Map;
	typedef map<string, BPlusTree<float>*> float_Map;
	typedef map<string, BPlusTree<string>*> string_Map;

	int static const TYPE_INT = -1;
	int static const TYPE_FLOAT = 0;

	int_Map IntMap_Index;
	float_Map FloatMap_Index;
	string_Map StringMap_Index;

	int GetDegree(int type);
	int GetKeySize(int type);

};
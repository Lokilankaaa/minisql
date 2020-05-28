#pragma once
#include <sstream>
#include <string>
#include <map>
#include "meta.h"
#include "buffer_manager.h"
//#include "bplustree.h"

using namespace std;
class IndexManager {
public:
	IndexManager(string table_name);
	~IndexManager();
	//创建索引文件及B + 树
	void CreateIndex(string file_path, int keytype);
	//删除索引、B+树及文件
	void DropIndex(string file_path, int keytype);
	//寻找索引位置
	int FindIndex(string file_path, data Data);
	//在指定索引中插入一个key
	void InsertIndex(string file_path, data Data, int block_id);
	//在指定索引中删除相应的Key
	void DeleteIndexByKey(string file_path, data Data);
	//返回一定范围内的value
	void SearchRange(string file_path, data Data1, data Data2, vector<int>& vals);

private:
//	typedef map<string, BPlusTree<int>*> int_Map;
//	typedef map<string, BPlusTree<float>*> float_Map;
//	typedef map<string, BPlusTree<string>*> string_Map;

	int static const TYPE_INT = -1;
	int static const TYPE_FLOAT = 0;

//	int_Map IntMap_Index;
//	float_Map FloatMap_Index;
//	string_Map StringMap_Index;

	//计算B+树适合的degree
	int GetDegree(int type);
	//计算不同类型Key的size
	int GetKeySize(int type);

};
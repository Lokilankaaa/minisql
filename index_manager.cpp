#pragma once
#include "index_manager.h"
#include "const.h"
#include "meta.h"
#include "buffer_manager.h"
#include "bplustree.h"
#include "catalog_manager.h"
#include <string>
#include <vector>
#include <map>
#include <io.h>
#include <string.h>

using namespace std;
IndexManager::IndexManager(string table_name)
{
	catalog_manager catalog;
	attributes_set attr = catalog.getAllattrs(table_name);

	for (int i = 0; i < attr.num; i++)
		if (attr.unique[i])
			CreateIndex("INDEX_FILE_" + attr.name[i] + "_" + table_name, attr.type[i]);
}
IndexManager::~IndexManager() 
{
	for (int_Map::iterator itInt = IntMap_Index.begin(); itInt != IntMap_Index.end(); itInt++) {
		if (itInt->second) {
			itInt->second->WrittenBackToDiskAll();
			delete itInt->second;
		}
	}
	for (string_Map::iterator itString = StringMap_Index.begin(); itString != StringMap_Index.end(); itString++) {
		if (itString->second) {
			itString->second->WrittenBackToDiskAll();
			delete itString->second;
		}
	}
	for (float_Map::iterator itFloat = FloatMap_Index.begin(); itFloat != FloatMap_Index.end(); itFloat++) {
		if (itFloat->second) {
			itFloat->second->WrittenBackToDiskAll();
			delete itFloat->second;
		}
	}
}

int IndexManager::GetDegree(int type)
{
	int degree = (PAGESIZE - sizeof(int)) / (GetKeySize(type) + sizeof(int));
	if (!(degree % 2))
		degree -= 1;
	return degree;
}

int IndexManager::GetKeySize(int type)
{
	if (type == TYPE_FLOAT)
		return sizeof(float);
	else if (type == TYPE_INT)
		return sizeof(int);
	else if (type > 0)
		return type;
	else {
		cout << "ERROR: in GetKeySize: invalid type" << endl;
		return -100;
	}
}


//���������ļ���B + ��
void IndexManager::CreateIndex(string file_path, int keytype)
{
	int key_size = GetKeySize(keytype); //��ȡkey��size
	int degree = GetDegree(keytype); //��ȡ��Ҫ��degree

	//�����������Ͳ�ͬ���ö�Ӧ�ķ�������ӳ���ϵ
	//�����ȳ�ʼ��һ��B+��
	if (keytype == TYPE_INT) {
		BPlusTree<int>* tree = new BPlusTree<int>(file_path, key_size, degree);
		IntMap_Index.insert(int_Map::value_type(file_path, tree));
	}
	else if (keytype == TYPE_FLOAT) {
		BPlusTree<float>* tree = new BPlusTree<float>(file_path, key_size, degree);
		FloatMap_Index.insert(float_Map::value_type(file_path, tree));
	}
	else {
		BPlusTree<std::string>* tree = new BPlusTree<std::string>(file_path, key_size, degree);
		StringMap_Index.insert(string_Map::value_type(file_path, tree));
	}
}

//ɾ��������B+�����ļ�
void IndexManager::DropIndex(string file_path, int keytype)
{
	//���ݲ�ͬ�������Ͳ��ö�Ӧ�Ĵ���ʽ
	if (keytype == TYPE_INT) {
		//����·����Ӧ�ļ�ֵ��
		int_Map::iterator itInt = IntMap_Index.find(file_path);
		if (itInt == IntMap_Index.end()) { //δ�ҵ�
			cout << "Error:in DropIndex, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			//ɾ����Ӧ��B+��
			delete itInt->second;
			//��ոü�ֵ��
			IntMap_Index.erase(itInt);
		}
	}
	else if (keytype == TYPE_FLOAT) { //ͬ��
		float_Map::iterator itFloat = FloatMap_Index.find(file_path);
		if (itFloat == FloatMap_Index.end()) {
			cout << "Error:in DropIndex, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			delete itFloat->second;
			FloatMap_Index.erase(itFloat);
		}
	}
	else {
		string_Map::iterator itString = StringMap_Index.find(file_path);
		if (itString == StringMap_Index.end()) { //ͬ��
			cout << "Error:in DropIndex, no index " << file_path <<" exits" << endl;
			return;
		}
		else {
			delete itString->second;
			StringMap_Index.erase(itString);
		}
	}

	return;
}
//Ѱ������λ��
int IndexManager::FindIndex(string file_path, data Data)
{
	//setKey(type, key);

	if (Data.type == TYPE_INT) {
		int_Map::iterator itInt = IntMap_Index.find(file_path);
		if (itInt == IntMap_Index.end()) { //δ�ҵ�
			cout << "error:in FindIndex, no index " << file_path << " exits" << endl;
			return -1;
		}
		else
			//�ҵ��򷵻ض�Ӧ�ļ�ֵ
			return itInt->second->SearchVal(Data.int_data);
	}
	else if (Data.type == TYPE_FLOAT) {
		float_Map::iterator itFloat = FloatMap_Index.find(file_path);
		if (itFloat == FloatMap_Index.end()) { //ͬ��
			cout << "error:in FindIndex, no index " << file_path << " exits" << endl;
			return -1;
		}
		else
			return itFloat->second->SearchVal(Data.float_data);
	}
	else {
		string_Map::iterator itString = StringMap_Index.find(file_path);
		if (itString == StringMap_Index.end()) { //ͬ��
			cout << "error:in FindIndex, no index " << file_path << " exits" << endl;
			return -1;
		}
		else
			return itString->second->SearchVal(Data.char_data);
	}
}
//��ָ�������в���һ��key
void IndexManager::InsertIndex(string file_path, data Data, int block_id)
{
	//setKey(type, key);

	if (Data.type == TYPE_INT) {
		int_Map::iterator itInt = IntMap_Index.find(file_path);
		if (itInt == IntMap_Index.end()) {
			cout << "error:in InsrtIndex,in the end!!"<< endl;
			return;
		}
		else
			itInt->second->InsertKey(Data.int_data, block_id);
	}
	else if (Data.type == TYPE_FLOAT) {
		float_Map::iterator itFloat = FloatMap_Index.find(file_path);
		if (itFloat == FloatMap_Index.end()) {
			cout << "error:in InsrtIndex,in the end!!" << endl;
			return;
		}
		else
			itFloat->second->InsertKey(Data.float_data, block_id);
	}
	else {
		string_Map::iterator itString = StringMap_Index.find(file_path);
		if (itString == StringMap_Index.end()) {
			cout << "error:in InsrtIndex,in the end!!" << endl;
			return;
		}
		else
			itString->second->InsertKey(Data.char_data, block_id);
	}

	return;
}
//��ָ��������ɾ����Ӧ��Key
void IndexManager::DeleteIndexByKey(string file_path, data Data)
{
	//setKey(type, key);

	if (Data.type == TYPE_INT) {
		int_Map::iterator itInt = IntMap_Index.find(file_path);
		if (itInt == IntMap_Index.end()) {
			cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else
			itInt->second->DeleteKey(Data.int_data);
	}
	else if (Data.type == TYPE_FLOAT) {
		float_Map::iterator itFloat = FloatMap_Index.find(file_path);
		if (itFloat == FloatMap_Index.end()) {
			cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else
			itFloat->second->DeleteKey(Data.float_data);
	}
	else {
		string_Map::iterator itString = StringMap_Index.find(file_path);
		if (itString == StringMap_Index.end()) {
			cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else
			itString->second->DeleteKey(Data.char_data);
	}
}
//����һ����Χ�ڵ�value
void IndexManager::SearchRange(string file_path, data Data1, data Data2, vector<int>& vals)
{
	int flag = 0;
	//������������Ƿ�ƥ��
	if (Data1.type == -2) 
		flag = 1;
	else if (Data2.type == -2) 
		flag = 2;
	else if (Data1.type != Data2.type) {
		cout << "ERROR: in SearchRange: Wrong data type!!" << endl;
		return;
	}

	if (Data1.type == TYPE_INT) {
		int_Map::iterator itInt = IntMap_Index.find(file_path);
		if (itInt == IntMap_Index.end()) {
			cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else
			itInt->second->SearchRange(Data1.int_data, Data2.int_data, vals, flag);
	}
	else if (Data1.type == TYPE_FLOAT) {
		float_Map::iterator itFloat = FloatMap_Index.find(file_path);
		if (itFloat == FloatMap_Index.end()) {
			cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else
			itFloat->second->SearchRange(Data1.float_data, Data2.float_data, vals, flag);
	}
	else {
		string_Map::iterator itString = StringMap_Index.find(file_path);
		if (itString == StringMap_Index.end()) {
			cout << "Error:in search index, no index " << file_path <<" exits" << endl;
			return;
		}
		else
			itString->second->SearchRange(Data1.char_data, Data2.char_data, vals, flag);
	}
}

string IndexManager::FindTableName(data index)
{
	string filepath = "xxxxx\\*.txt";
	string filename;
	struct _finddata_t fileinfo;
	long handle;
	handle = _findfirst(filepath.c_str(), &fileinfo);
	do {
		filename = fileinfo.name;

		if (Data.type == TYPE_INT) {
			int_Map::iterator itInt = IntMap_Index.find(filename);
			if (itInt == IntMap_Index.end())
				continue;
			else
				//�ҵ��򷵻ض�Ӧ�ļ�ֵ
				return itInt->second->findTableName(index);
		}
		else if (Data.type == TYPE_FLOAT) {
			float_Map::iterator itFloat = FloatMap_Index.find(filename);
			if (itFloat == FloatMap_Index.end()) 
				continue;
			else
				return itFloat->second->findTableName(index);
		}
		else {
			string_Map::iterator itString = StringMap_Index.find(filename);
			if (itString == StringMap_Index.end()) 
				continue;
			else
				return itString->second->findTableName(index);
		}

	}while (!_findnext(handle, &fileinfo));
	_findclose(handle);
	string default_ = "error: in FindTableName,no such index!!";
	return default_;
}
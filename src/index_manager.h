//
// Created by 薛伟 on 2020/5/30.
//

#ifndef MINISQL_INDEX_MANAGER_H
#define MINISQL_INDEX_MANAGER_H


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
    explicit IndexManager(string table_name);
    ~IndexManager();
    //创建索引文件及B + 树
    void CreateIndex(const string& file_path, int keytype);
    //删除索引、B+树及文件
    void DropIndex(const string& file_path, int keytype);
    //寻找索引位置
    int FindIndex(const string& file_path, data Data);
    //在指定索引中插入一个key
    void InsertIndex(string file_path, data Data, int block_id);
    //在指定索引中删除相应的Key
    void DeleteIndexByKey(const string& file_path, data Data);
    //返回一定范围内的value
    void SearchRange(const string& file_path, data Data1, data Data2, vector<int>& vals);
    //输入index返回对应表的名称
    //返回的是文件名，表名在文件名中
    string FindTableName(const data& index);

private:
    typedef map<string, BPlusTree<int>*> int_Map;
    typedef map<string, BPlusTree<float>*> float_Map;
    typedef map<string, BPlusTree<string>*> string_Map;

    int_Map IntMap_Index;
    float_Map FloatMap_Index;
    string_Map StringMap_Index;

    int static const TYPE_INT = -1;
    int static const TYPE_FLOAT = 0;

    //计算B+树适合的degree
    int GetDegree(int type);
    //计算不同类型Key的size
    int GetKeySize(int type);

};


#endif //MINISQL_INDEX_MANAGER_H

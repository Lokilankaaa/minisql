#include "index_manager.h"
#include "meta.h"
#include "buffer_manager.h"
#include "bplustree.h"
#include "Catalog_manager.h"
#include "error.h"
#include <string>
#include <vector>
#include <map>
#include <cstring>
#include <dirent.h>

using namespace std;

IndexManager::IndexManager(string table_name) {
    attributes_set attr = catalog_manager::getAllattrs(table_name);

    for (int i = 0; i < attr.num; i++)
        if (attr.unique[i])
            CreateIndex("INDEX_FILE_" + attr.name[i] + "_" + table_name, attr.type[i]);
}

IndexManager::~IndexManager() {
    for (auto& itInt : IntMap_Index) {
        if (itInt.second) {
            itInt.second->WrittenBackToDiskAll();
            delete itInt.second;
        }
    }
    for (auto& itString : StringMap_Index) {
        if (itString.second) {
            itString.second->WrittenBackToDiskAll();
            delete itString.second;
        }
    }
    for (auto& itFloat : FloatMap_Index) {
        if (itFloat.second) {
            itFloat.second->WrittenBackToDiskAll();
            delete itFloat.second;
        }
    }
}

int IndexManager::GetDegree(int type) {
    int degree = (PAGESIZE - sizeof(int)) / (GetKeySize(type) + sizeof(int));
    if (!(degree % 2))
        degree -= 1;
    return degree;
}

int IndexManager::GetKeySize(int type) {
    if (type == TYPE_FLOAT)
        return sizeof(float);
    else if (type == TYPE_INT)
        return sizeof(int);
    else if (type > 0)
        return type;
    else {
        throw e_data_type_conflict();
    }
}


//创建索引文件及B + 树
void IndexManager::CreateIndex(const string& file_path, int keytype) {
    int key_size = GetKeySize(keytype); //获取key的size
    int degree = GetDegree(keytype); //获取需要的degree

    //根据数据类型不同，用对应的方法建立映射关系
    //并且先初始化一颗B+树
    if (keytype == TYPE_INT) {
        auto tree = new BPlusTree<int>(file_path, key_size, degree);
        IntMap_Index.insert(int_Map::value_type(file_path, tree));
    }
    else if (keytype == TYPE_FLOAT) {
        auto tree = new BPlusTree<float>(file_path, key_size, degree);
        FloatMap_Index.insert(float_Map::value_type(file_path, tree));
    }
    else {
        auto tree = new BPlusTree<std::string>(file_path, key_size, degree);
        StringMap_Index.insert(string_Map::value_type(file_path, tree));
    }
}

//删除索引、B+树及文件
void IndexManager::DropIndex(const string& file_path, int keytype) {
    //根据不同数据类型采用对应的处理方式
    if (keytype == TYPE_INT) {
        //查找路径对应的键值对
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) { //未找到
            throw e_index_not_exist();
        }
        else {
            //删除对应的B+树
            delete itInt->second;
            //清空该键值对
            IntMap_Index.erase(itInt);
        }
    }
    else if (keytype == TYPE_FLOAT) { //同上
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) {
            throw e_index_not_exist();
        }
        else {
            delete itFloat->second;
            FloatMap_Index.erase(itFloat);
        }
    }
    else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) { //同上
            throw e_index_not_exist();
        }
        else {
            delete itString->second;
            StringMap_Index.erase(itString);
        }
    }
}

//寻找索引位置
int IndexManager::FindIndex(const string& file_path, data Data) {
    //setKey(type, key);

    if (Data.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) { //未找到
            throw e_index_not_exist();
        }
        else
            //找到则返回对应的键值
            return itInt->second->SearchVal(Data.int_data);
    }
    else if (Data.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) { //同上
            throw e_index_not_exist();
        }
        else
            return itFloat->second->SearchVal(Data.float_data);
    }
    else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) { //同上
            throw e_index_not_exist();
        }
        else
            return itString->second->SearchVal(Data.char_data);
    }
}

//在指定索引中插入一个key
void IndexManager::InsertIndex(string file_path, data Data, int block_id) {
    //setKey(type, key);

    if (Data.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) {
            throw e_index_full();
        }
        else
            itInt->second->InsertKey(Data.int_data, block_id);
    }
    else if (Data.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) {
            throw e_index_full();
        }
        else
            itFloat->second->InsertKey(Data.float_data, block_id);
    }
    else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) {
            throw e_index_full();
        }
        else
            itString->second->InsertKey(Data.char_data, block_id);
    }
}

//在指定索引中删除相应的Key
void IndexManager::DeleteIndexByKey(const string& file_path, data Data) {
    //setKey(type, key);

    if (Data.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) {
            throw e_index_not_exist();
        }
        else
            itInt->second->DeleteKey(Data.int_data);
    }
    else if (Data.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) {
            throw e_index_not_exist();
        }
        else
            itFloat->second->DeleteKey(Data.float_data);
    }
    else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) {
            throw e_index_not_exist();
        }
        else
            itString->second->DeleteKey(Data.char_data);
    }
}

//返回一定范围内的value
void IndexManager::SearchRange(const string& file_path, data Data1, data Data2, vector<int>& vals) {
    int flag = 0;
    //检测数据类型是否匹配
    if (Data1.type == -2)
        flag = 1;
    else if (Data2.type == -2)
        flag = 2;
    else if (Data1.type != Data2.type) {
        throw e_data_type_conflict();
    }

    if (Data1.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) {
            throw e_index_not_exist();
        }
        else
            itInt->second->SearchRange(Data1.int_data, Data2.int_data, vals, flag);
    }
    else if (Data1.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) {
            throw e_index_not_exist();
        }
        else
            itFloat->second->SearchRange(Data1.float_data, Data2.float_data, vals, flag);
    }
    else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) {
            throw e_index_not_exist();
        }
        else
            itString->second->SearchRange(Data1.char_data, Data2.char_data, vals, flag);
    }
}


void listDir(char* path, std::vector<std::string>* files)
{
    DIR* directory_pointer;
    struct dirent* entry;
    char childpath[512];  //定义一个字符数组，用来存放读取的路径
    char filepath[512];  //定义一个字符数组，用来存放读取的路径
    directory_pointer = opendir(path);
    memset(childpath, 0, sizeof(childpath)); //将字符数组childpath的数组元素全部置零
    while ((entry = readdir(directory_pointer)) != NULL)  //读取pDir打开的目录，并赋值给ent, 同时判断是否目录为空，不为空则执行循环体
    {
        if (entry->d_type != DT_REG) {
            throw e_unknown_file_path();
        }
        sprintf(filepath, "%s/%s", path, entry->d_name);
        files->push_back(filepath);
    }
}

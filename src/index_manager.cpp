#include "index_manager.h"
#include "meta.h"
#include "buffer_manager.h"
#include "bplustree.h"
#include "Catalog_manager.h"
#include "error.h"
#include <vector>
#include <map>
#include <cstring>
#include <dirent.h>
#include "Grammar.h"
#include <sstream>

using namespace std;

void listDir(char *path, std::vector<std::string> *files) {
    DIR *directory_pointer;
    struct dirent *entry;
    char childpath[512];  //定义一个字符数组，用来存放读取的路径
    char filepath[512];  //定义一个字符数组，用来存放读取的路径
    directory_pointer = opendir(path);
    memset(childpath, 0, sizeof(childpath)); //将字符数组childpath的数组元素全部置零
    while ((entry = readdir(directory_pointer)) != NULL)  //读取pDir打开的目录，并赋值给ent, 同时判断是否目录为空，不为空则执行循环体
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        if (entry->d_type != DT_REG) {
            throw e_unknown_file_path();
        }
        sprintf(filepath, "%s/%s", path, entry->d_name);
        files->push_back(filepath);
    }
    closedir(directory_pointer);
}

IndexManager::IndexManager(string &table_name) {

//    std::string inPath = "./database/index/*.txt";//遍历文件夹下的所有.jpg文件
//    //用于查找的句柄
//    char filename[512];
//    long handle;
//    vector<string> files;
//    struct _finddata_t fileinfo;
//    //第一次查找
//    handle = _findfirst(inPath.c_str(), &fileinfo);
//    if (handle == -1) {
//        return;
//    }
    string file_path = "./database/index";
    string filename;
    char *path = const_cast<char *>(file_path.c_str());
    vector<string> files;

    listDir(path, &files);
//    if (files.empty())
//        return;
//    auto pos = files.begin();
//    while (pos != files.end()) {
//        if (split(*pos, '_')[split(*pos, '_').size() -3] == table_name){
//
//        }
//
//        else
//            pos++;
//    }

//    do {
//        //找到的文件的文件名
//        sprintf(filename, "%s/%s", inPath.c_str(), fileinfo.name);
//        files.push_back(split(filename, '/').back());
//    } while (!_findnext(handle, &fileinfo));
//    _findclose(handle);
    for (auto &i:files) {
        if(split(i, '_')[0]!="./database/index/INDEX") continue;
        auto tmp = split(i, '_')[split(i, '_').size() - 3];
        if (tmp == table_name) {
            auto x = split(i, '_').back();
            auto y = x.substr(0, x.size() - 4);
            int type = str2num<int>(y);
            auto tt = split(i, '/').back();
            if (type == -1) {
                auto tree = new BPlusTree<int>(i, GetKeySize(-1), GetDegree(-1));
                IntMap_Index.insert(int_Map::value_type(tt, tree));
//                tree->ReadFromDiskAll();
            } else if (!type) {
                auto tree1 = new BPlusTree<float>(i, GetKeySize(0), GetDegree(0));
                FloatMap_Index.insert(float_Map::value_type(tt, tree1));
//                tree1->ReadFromDiskAll();
            } else {
                auto tree2 = new BPlusTree<string>(i, GetKeySize(type), GetDegree(type));
                StringMap_Index.insert(string_Map::value_type(tt, tree2));
//                tree2->ReadFromDiskAll();
            }
        }
    }

    //for (int i = 0; i < attr.num; i++)
    //    if (attr.unique[i])
    //        CreateIndex("INDEX_FILE_" + attr.name[i] + "_" + table_name, attr.type[i]);

}

IndexManager::~IndexManager() {
    for (auto &itInt : IntMap_Index) {
        if (itInt.second) {
            itInt.second->writtenbackToDiskAll();
            delete itInt.second;
        }
    }
    for (auto &itString : StringMap_Index) {
        if (itString.second) {
            itString.second->writtenbackToDiskAll();
            delete itString.second;
        }
    }
    for (auto &itFloat : FloatMap_Index) {
        if (itFloat.second) {
            itFloat.second->writtenbackToDiskAll();
            delete itFloat.second;
        }
    }
}

int IndexManager::GetDegree(int type) {
    int degree = (PAGESIZE - sizeof(int)) / (GetKeySize(type) + sizeof(int)*2+5);
    if (!(degree % 2))
        degree -= 1;
    return degree;
}

int IndexManager::GetKeySize(int type) {
    if (type == TYPE_FLOAT)
        return sizeof(float)*2;
    else if (type == TYPE_INT)
        return sizeof(int)*2;
    else if (type > 0)
        return type;
    else {
        throw e_data_type_conflict();
        return -100;
    }
}


//创建索引文件及B + 树
void IndexManager::CreateIndex(const string &file_path, int keytype) {
    int key_size = GetKeySize(keytype); //获取key的size
    int degree = GetDegree(keytype); //获取需要的degree

    //根据数据类型不同，用对应的方法建立映射关系
    //并且先初始化一颗B+树
    if (keytype == TYPE_INT) {
        auto tree = new BPlusTree<int>(file_path, key_size, degree);
        IntMap_Index.insert(int_Map::value_type(file_path, tree));
    } else if (keytype == TYPE_FLOAT) {
        auto tree = new BPlusTree<float>(file_path, key_size, degree);
        FloatMap_Index.insert(float_Map::value_type(file_path, tree));
    } else {
        auto tree = new BPlusTree<std::string>(file_path, key_size, degree);
        StringMap_Index.insert(string_Map::value_type(file_path, tree));
    }
}

//删除索引、B+树及文件
void IndexManager::DropIndex(const string &file_path, int keytype) {
    //根据不同数据类型采用对应的处理方式
//    auto path = "./database/index/" + file_path;
//    if (keytype == TYPE_INT) {
//        //查找路径对应的键值对
//        auto itInt = IntMap_Index.find(file_path);
//        if (itInt == IntMap_Index.end()) { //未找到
//            throw e_index_not_exist();
//            return;
//        } else {
//            //删除对应的B+树
//            delete itInt->second;
//            //清空该键值对
//            IntMap_Index.erase(itInt);
//            remove(path.c_str());
//        }
//    } else if (keytype == TYPE_FLOAT) { //同上
//        auto itFloat = FloatMap_Index.find(file_path);
//        if (itFloat == FloatMap_Index.end()) {
//            throw e_index_not_exist();
//            return;
//        } else {
//            delete itFloat->second;
//            FloatMap_Index.erase(itFloat);
//            remove(path.c_str());
//        }
//    } else {
//        auto itString = StringMap_Index.find(file_path);
//        if (itString == StringMap_Index.end()) { //同上
//            throw e_index_not_exist();
//            return;
//        } else {
//            delete itString->second;
//            StringMap_Index.erase(itString);
//            remove(path.c_str());
//        }
//    }

    auto path = "./database/index/" + file_path;
    if (keytype == TYPE_INT) {
        remove(path.c_str());

    } else if (keytype == TYPE_FLOAT) { //同上
        remove(path.c_str());

    } else {
        remove(path.c_str());

    }

}

//寻找索引位置
int IndexManager::FindIndex(const string &file_path, data Data) {
    //setKey(type, key);

    if (Data.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) { //未找到
            throw e_index_not_exist();
            return -1;
        } else
            //找到则返回对应的键值
            return itInt->second->searchVal(Data.int_data);
    } else if (Data.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) { //同上
            throw e_index_not_exist();
            return -1;
        } else
            return itFloat->second->searchVal(Data.float_data);
    } else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) { //同上
            throw e_index_not_exist();
            return -1;
        } else
            return itString->second->searchVal(Data.char_data);
    }
}

//在指定索引中插入一个key
void IndexManager::InsertIndex(string file_path, data Data, int block_id) {
    //setKey(type, key);

    if (Data.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        itInt->second->insertKey(Data.int_data, block_id);
    } else if (Data.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        itFloat->second->insertKey(Data.float_data, block_id);
    } else {
        auto itString = StringMap_Index.find(file_path);
        itString->second->insertKey(Data.char_data, block_id);
    }
}

//在指定索引中删除相应的Key
void IndexManager::DeleteIndexByKey(const string &file_path, data Data) {
    //setKey(type, key);

    if (Data.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) {
            throw e_index_not_exist();
            return;
        } else
            itInt->second->deleteKey(Data.int_data);
    } else if (Data.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) {
            throw e_index_not_exist();
            return;
        } else
            itFloat->second->deleteKey(Data.float_data);
    } else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) {
            throw e_index_not_exist();
            return;
        } else
            itString->second->deleteKey(Data.char_data);
    }
}

//返回一定范围内的value
void IndexManager::SearchRange(const string &file_path, data Data1, data Data2, vector<int> &vals) {
    int flag = 0;
    //检测数据类型是否匹配
    if (Data1.type == -2)
        flag = 1;
    else if (Data2.type == -2)
        flag = 2;
    else if (Data1.type != Data2.type) {
        throw e_data_type_conflict();
        return;
    }

    if (Data1.type == TYPE_INT) {
        auto itInt = IntMap_Index.find(file_path);
        if (itInt == IntMap_Index.end()) {
            throw e_index_not_exist();
            return;
        } else
            itInt->second->searchRange(Data1.int_data, Data2.int_data, vals, flag);
    } else if (Data1.type == TYPE_FLOAT) {
        auto itFloat = FloatMap_Index.find(file_path);
        if (itFloat == FloatMap_Index.end()) {
            throw e_index_not_exist();
            return;
        } else
            itFloat->second->searchRange(Data1.float_data, Data2.float_data, vals, flag);
    } else {
        auto itString = StringMap_Index.find(file_path);
        if (itString == StringMap_Index.end()) {
            throw e_index_not_exist();
            return;
        } else
            itString->second->searchRange(Data1.char_data, Data2.char_data, vals, flag);
    }
}

//string IndexManager::FindTableName(const string& index_name) {
//    system("dir ./index >> a.txt");
//    fstream  f("a.txt");
//    std::vector<std::string> tmp;
//    std::string tt;
//    while (getline(f, tt)) {
//        tmp.push_back(split(tt, ' ').back());
//    }
//
//}

//
//string IndexManager::FindTableName(const string &index_name) {
//    //目标文件夹路径
//    std::string inPath = "./database/index/*.txt";//遍历文件夹下的所有.jpg文件
//    //用于查找的句柄
//    char filename[512];
//    long handle;
//    vector<string> files;
//    struct _finddata_t fileinfo;
//    //第一次查找
//    handle = _findfirst(inPath.c_str(), &fileinfo);
//    if (handle == -1) {
//        throw e_unknown_file_path();
//        exit(0);
//    }
//
//    do {
//        //找到的文件的文件名
//        sprintf(filename, "%s/%s", inPath.substr(0,inPath.size()-6).c_str(), fileinfo.name);
//        files.push_back(filename);
//    } while (!_findnext(handle, &fileinfo));
//    _findclose(handle);
//
//    auto pos = files.begin();
//
//    while (pos != files.end()) {
//        if (split(*pos, '_')[split(*pos, '_').size() -2] == index_name)
//            return *pos;
//        else
//            pos++;
//    }
//    throw e_index_not_exist();
//}

string IndexManager::FindTableName(const string &index_name) {
    string file_path = "./database/index";
    string filename;
    char *path = const_cast<char *>(file_path.c_str());
    vector<string> files;

    listDir(path, &files);
    auto pos = files.begin();
    while (pos != files.end()) {
        if (split(*pos, '_')[split(*pos, '_').size() - 2] == index_name)
            return *pos;
        else
            pos++;
    }

    throw e_index_not_exist();
}

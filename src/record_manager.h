//
// Created by lfy on 2020/5/18.
//

#ifndef MINISQL_BUFFER_RECORD_RECORD_MANAGER_H
#define MINISQL_BUFFER_RECORD_RECORD_MANAGER_H


//RecordManager类: 为API提供接口，直接对记录、文件进行操作
/* 接口说明：
 * createTableFile(表名) 可以创建一张表
 * dropTableFile(表名) 可以删除一张表
 * selectRecord(表名,返回变量table的table_name) 可以获取整张表的数据，并返回一个table类型变量，其table_name为第二个属性(默认值是"tmp_table")
 * selectRecord(表名,目标属性,条件,返回变量table的table_name) 可以获取满足条件的记录组成的table变量
 * insertRecord(表名,TUPLE记录) 向该表插入一条记录
 * deleteRecord(表名) 删除整张表的记录，但是这张表依旧保留
 * deleteRecord(表名,属性集合,约束集合) 可以删除含有满足条件属性的所有记录
 * createIndex(index管理类对象, 表名, 对应属性) 可以在对应表的对应属性上创建索引
 *
 * 详细信息见下面个函数说明
 */


#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <climits>
#include <cstdio>
#include <set>
#include "meta.h"
#include "index_manager.h"          //索引管理，根据整合后实际情况引用
#include "Catalog_manager.h"
#include "buffer_manager.h"
#include "error.h"
#include "record_function.h"

#define POSINF INT_MAX              //用于where语句的上下限
#define NEGINF INT_MIN
#define DATA_FILE_PATH "./database/data/"

extern BufferManager buf_manager;

class RecordManager {
public:
    //功能：为表创建相应的文件
    //异常：无异常处理，交由catalog managar处理
    //输入：所要建立的表的名字
    //输出：void
    void createTableFile(std::string table_name);
    //功能：删除表相对应的文件
    //异常：无异常处理，交由catalog managar处理
    //输入：所要删除的表的名字
    //输出：void
    void dropTableFile(std::string table_name);
    //功能：根据表名返回整张表
    //异常：如果表不存在则抛出e_table_not_exist异常
    //输入：table_name:表名, result_table_name:返回的table的table_name,默认为tmp_table
    //输出：代表整张表的table类型对象
    table selectRecord(std::string table_name, std::string result_table_name = "tmp_table");
    //功能：带条件的查询，返回满足Constraint条件的记录的table
    //异常：如果表不存在，则抛出e_table_not_exist异常;如果属性不存在，则抛出e_attribute_not_exist异常;如果where中；两个数据不匹配，则抛出e_data_type_conflict异常
    //输入：(表名, where子句的属性名, where子句的约束条件, 生成的表名)
    //输出：满足where子句条件的table
    table selectRecord(std::string table_name, std::string target_attr, Constraint target_cons, std::string result_table_name = "tmp_table");
    //功能：向对应表中插入一条记录
    //异常：如果代表记录的元祖类型不匹配,抛出e_tuple_type_conflict;如果主键冲突,抛出e_unique_conflict;
    //     如果与已存在的主键冲突,抛出e_primary_key_conflict;如果表不存在，抛出e_table_not_exist
    //输入：(表名,要插入的记录TUPLE)
    //输出：void
    void insertRecord(std::string table_name, TUPLE& tuple);
    //功能：删除对应表中的所有记录，保留表文件
    //异常：如果表不存在，则抛出e_table_not_exist异常
    //输入：想要清空数据的表名
    //输出：删除的记录数量
    int deleteRecord(std::string table_name);
    //功能带有多个条件的删除，删除满足vector<Constraint>所有条件的记录
    //异常：如果对应属性不存在，则抛出e_attribute_not_exist异常，如果where子句数据类型不匹配，则抛出e_data_type_conflict异常
    //输入：(表名,属性集合,约束集合)
    //输出：删除数据的数量
    int deleteRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Constraint> target_cons);
    //功能：对表中的记录创建索引
    //异常：如果表不存在，则抛出e_table_not_exist异常;如果对应属性不存在，则抛出e_attribute_not_exist异常
    //输入：(索引管理类对象的引用,表名,属性名)
    //输出：void
    void createIndex(IndexManager &index_manager, std::string table_name, std::string target_attr, std::string index_name);
private:
    //获取该文件一共有多少个block
    int countBlockNum(const std::string& table_name);
    //从传入指针的内存位置读取一个TUPLE(即一条记录的数据信息)
    TUPLE readOneTuple(const char* p, attributes_set attr);
    //获取一个TUPLE的长度(即一条记录的长度)
    int getTupleLength(char* p);
    //带索引查找满足条件的块,结果保存在传入的allBlockId
    void searchBlockWithIndex(std::string table_name, std::string target_attr, Constraint target_cons, std::vector<int> &allBlockId, std::string index_name);
    //在相应的块中查询满足条件的记录,结果保存在要输出的表的vector<Tuple>&中
    void searchRecordAccordCons(std::string table_name, int block_id, attributes_set target_attr, int attr_index, Constraint target_cons, std::vector<TUPLE> &result);
    //判断要插入的记录是否和其他记录有冲突
    bool judgeConflict(const std::vector<TUPLE>& table_tuples, std::vector<data> &target_value, int attr_index);
    //insertRecord调用的函数，表示在该位置插入一条记录
    void insertOneRecord(char *p, int offset, int len, const std::vector<data> &record_values);
    //根据条件的集合删除元素
    int deleteRecordAccordCons(std::string table_name, int block_id, attributes_set target_attr, std::vector<int> attr_index, std::vector<Constraint> target_cons);
    //deleteRecord—>deleteRecordAccordCons—>调用的函数，真正地删除一个数据
    char* deleteOneRecord(char *p);
};


#endif

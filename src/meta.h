//
// Created by 薛伟 on 2020/5/17.
//

#ifndef MINISQL_META_H
#define MINISQL_META_H

#include <string>
#include <vector>
#include "error.h"


// type == -1:int, 0:float, 1-255:char
struct data {
    int type;
    int int_data;
    float float_data;
    std::string char_data;
};

struct attributes_set {
    int num;//number of attributes
    std::string name[32]; //name of each attribute
    int type[32]; // type of each attribute
    bool unique[32]; // whether it's unique
    int primary_key; // -1 means no primary key, other means the position of key
    bool has_index[32]; // whether the attribute has index
};

class TUPLE {
private:
    std::vector<data> Data;
public:
    TUPLE();

    ~TUPLE();

    TUPLE(const TUPLE &t);

    void addData(const data &d);

    std::vector<data> getData() const;

    int getTupleSize() const;
};

class table {
private:
    std::string table_name;
    std::vector<TUPLE> tuples;
    attributes_set attributesSet;
    //index placeholder
public:
    table(const std::string &name, const attributes_set &attrs);

    ~table();

    table(const table &t);

    Error addTuple(const TUPLE &t);

    Error deleteTuple(int pos);

    //index func placeholder

    const std::string &getTableName() const;

    attributes_set getAttr() const;

    std::vector<TUPLE> getTuple() const;

    int getKeypos() const;
};

class database {
private:
    std::string database_name;
    std::vector<table> tables;
public:
    explicit database(std::string name);

    ~database();

    bool checkTable(const std::string &table_name);

    table getTable(const std::string &table_name);

    Error addTable(table &t);

    Error deleteTable(const std::string &name);
};

#endif //MINISQL_META_H

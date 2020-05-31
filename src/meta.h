//
// Created by 薛伟 on 2020/5/17.
//

#ifndef MINISQL_META_H
#define MINISQL_META_H

#include <string>
#include <vector>
#include "error.h"
#include "Grammar.h"


// type == -1:int, 0:float, 1-255:char
struct data {
    int type;
    int int_data{0};
    float float_data{0};
    std::string char_data;
public:
    bool operator==(const data &d) const {
        return type == d.type and int_data == d.int_data and float_data == d.float_data and char_data == d.char_data;
    }
};

struct Index {
    int num{0};
    int location[10];
    std::string name[10];
};

struct attributes_set {
    int num{0};//number of attributes
    std::string name[32]; //name of each attribute
    int type[32]; // type of each attribute
    bool unique[32]{false}; // whether it's unique
    int primary_key; // -1 means no primary key, other means the position of key
    bool hasIndex[32]{false};   //lfy add: judge whether has index
};

class TUPLE {
private:
    std::vector<data> Data;
    bool state;                 //lfy add: the state for the TUPLE(using in record_manager)
public:
    TUPLE();

    ~TUPLE();

    TUPLE(const TUPLE &t);

    void addData(const data &d);

    std::vector<data> getData() const;

    int getTupleSize() const;



    //lfy add: using for record_manager
    void setState() { state = true; };

    bool getState() const { return state; };

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

    int getTupleSize() const;

    //index func placeholder

    const std::string &getTableName() const;

    attributes_set getAttr() const;

    std::vector<TUPLE> getTuple() const;

    int getKeypos() const;
};


//lfy: add structure 'Constraint' for 'where (conditions)'

// 举例：如果想要获得老师工作大于10000的记录
/* std::string temp_attr;
 * data temp_data;
 * Constraint temp_con;
 *
 * temp_attr = std::string("salary");
 * temp_data.type = -1; temp_data.int_data = 10000;
 * temp_con.data = temp_data; temp_con.conSymbol = MORE;
 *
 * table resTable = selectRecord(table_name, temp_attr, temp_con);
 */
typedef enum {
    LESS,
    LESS_OR_EQUAL,
    EQUAL,
    MORE_OR_EQUAL,
    MORE,
    NOT_EQUAL
} CONSTRAINT;

struct Constraint {
    data conData;               //Data for relationship constraints
    CONSTRAINT conSymbol;       //Symbols of relation constraints
};


#endif //MINISQL_META_H

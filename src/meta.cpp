//
// Created by 薛伟 on 2020/5/17.
//

#include "meta.h"

#include <utility>

TUPLE::TUPLE(){
    state = false;
}

TUPLE::~TUPLE() = default;

TUPLE::TUPLE(const TUPLE &t) {
    state = false;
    Data = t.Data;
}

void TUPLE::addData(const data &d) {
    Data.push_back(d);
}

std::vector<data> TUPLE::getData() const {
    return Data;
}

int TUPLE::getTupleSize() const {
    return Data.size();
}

table::table(const std::string &name, const attributes_set &attrs) {
    table_name = name;
    attributesSet = attrs;
}

table::table(const table &t) {
    table_name = t.table_name;
    tuples = t.tuples;
    attributesSet = t.attributesSet;
}

Error table::addTuple(const TUPLE &t) {
    if (t.getTupleSize() != attributesSet.num) {
        return illegal_tuple_insert;
    } else {
        for (int i = 0; i < attributesSet.num; ++i) {
            if ((t.getData()[i].type > attributesSet.type[i]) ||
                    (t.getData()[i].type <= attributesSet.type[i] && t.getData()[i].type != attributesSet.type[i]))
                return illegal_tuple_insert;
        }
    }
    tuples.push_back(t);
    return successful;
}

const std::string &table::getTableName() const {
    return table_name;
}

attributes_set table::getAttr() const {
    return attributesSet;
}

std::vector<TUPLE> table::getTuple() const {
    return tuples;
}

int table::getKeypos() const {
    return attributesSet.primary_key;
}

Error table::deleteTuple(int pos) {
    if (pos <= tuples.size()) {
        tuples.erase(tuples.begin() + pos);
        return successful;
    } else {
        return tuple_not_exist;
    }
}

table::~table() = default;


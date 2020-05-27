//
// Created by 薛伟 on 2020/5/17.
//

#include "meta.h"

#include <utility>

TUPLE::TUPLE() = default;

TUPLE::~TUPLE() = default;

TUPLE::TUPLE(const TUPLE &t) {
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

database::database(std::string name) {
    database_name = std::move(name);
}

bool database::checkTable(const std::string &table_name) {
    for (auto &i:tables) {
        if (i.getTableName() == table_name)
            return true;
    }
    return false;
}

Error database::addTable(table &t) {
    if (!checkTable(t.getTableName())) {
        tables.push_back(t);
        return successful;
    } else {
        return insert_table_exist;
    }
}

Error database::deleteTable(const std::string &name) {
    for (int i = 0; i < tables.size(); ++i) {
        if (tables[i].getTableName() == name) {
            tables.erase(tables.begin() + i);
            return successful;
        }
    }
    return table_not_exist;
}

table database::getTable(const std::string &table_name) {
    if (checkTable(table_name)) {
        for (const auto &i:tables) {
            if (i.getTableName() == table_name)
                return i;
        }
    } else {
        return table("error", attributes_set());
    }
}

database::~database() = default;

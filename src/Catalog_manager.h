//
// Created by 薛伟 on 2020/4/29.
//

#ifndef MINISQL_CATALOG_MANAGER_H
#define MINISQL_CATALOG_MANAGER_H
#include <string>
#include <vector>
#include <map>

using namespace std;

struct Table{
    string table_name;
    map<string, string> attrbutes;
    string primary_key;
};

struct Database {
    string database_name;
    vector<Table> tables;
};

class catalog_manager {
protected:

};

#endif //MINISQL_CATALOG_MANAGER_H

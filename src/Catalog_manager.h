//
// Created by 薛伟 on 2020/4/29.
//

#ifndef MINISQL_CATALOG_MANAGER_H
#define MINISQL_CATALOG_MANAGER_H

#include <fstream>
#include <string>
#include <vector>
#include <map>
#include "meta.h"
#include "error.h"
#include <algorithm>

using namespace std;

const string CATALOG_FILE_PATH = "./database/catalog/cata_file";

class catalog_manager {
private:
    database db;
public:
    catalog_manager();

    ~catalog_manager() = default;

    Error createtable(std::string &table_name, attributes_set &attrs, int pk);

    Error droptable(std::string &table_name);

    bool hastable(std::string &table_name);


};

#endif //MINISQL_CATALOG_MANAGER_H

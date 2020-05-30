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
#include "buffer_manager.h"
#include <algorithm>

#define CATALOG_FILE_PATH "./database/catalog/cata_file"

extern BufferManager buf_manager;

class catalog_manager {
private:
    static int getBlockNum(const std::string &file_name);

    static bool hasattribute(std::string &table_name, std::string &attr_name);

    static int hastable(std::string &table_name);

public:
    catalog_manager() = default;

    ~catalog_manager() = default;

    static Error createtable(std::string &table_name, attributes_set &attrs, Index &index, int pk);

    static Error droptable(std::string &table_name);

    static Error createindex(std::string &table_name, std::string &attr_name, std::string &index_name);

    static Error dropindex(std::string &table_name, std::string &index_name);

    static attributes_set getAllattrs(std::string &table_name);

    static Index getAllindex(std::string &table_name);
};

#endif //MINISQL_CATALOG_MANAGER_H
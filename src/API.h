//
// Created by 薛伟 on 2020/4/29.
//

#ifndef MINISQL_API_H
#define MINISQL_API_H

#include <string>
#include "error.h"
#include "Catalog_manager.h"
#include "record_manager.h"
#include "index_manager.h"


class API {
private:
    static int check_type (std::string& val);

protected:
    catalog_manager catalogManager;

    static RecordManager rec_manager;

    table joinTable(std::vector<table> &tables);

public:
    static Error create_table(std::string &table_name, attributes_set &attrs, Index &index, int pk);

    static Error create_index(std::string &index_name, std::string &table_name, std::string &column_name);

    static Error drop_table(std::string &table_name);

    Error drop_index(std::string &index_name);

    Error insert_table(std::string &table_name, std::vector<std::string> values);

    Error select(std::string &table_name, std::vector<std::string> conditions);

    Error delete_table(std::string &table_name, std::vector<std::string> conditions);

};

#endif //MINISQL_API_H

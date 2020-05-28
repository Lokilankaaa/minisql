//
// Created by 薛伟 on 2020/4/29.
//

#ifndef MINISQL_API_H
#define MINISQL_API_H

#include <string>
#include "error.h"
#include "Catalog_manager.h"


class API {
protected:
    catalog_manager catalogManager;


public:
    Error create_table(string &table_name, attributes_set &attrs, int pk);

    Error create_index(std::string &index_name, std::string &table_name, std::string &column_name);

    Error drop_table(std::string &table_name);

    Error drop_index(std::string &index_name);

    Error insert_table(std::string &table_name, std::vector<std::string> values);

    Error select(std::string &table_name, std::vector<std::string> conditions);

    Error delete_table(std::string &table_name, std::vector<std::string> conditions);

};

#endif //MINISQL_API_H

//
// Created by 薛伟 on 2020/4/29.
//

#include "API.h"

Error API::create_table(string &table_name, attributes_set &attrs, int pk) {
    return catalogManager.createtable(table_name, attrs, pk);
}

Error API::create_index(std::string &index_name, std::string &table_name, std::string &column_name) {
    return invalid_value;
}

Error API::drop_table(std::string &table_name) {
    return catalogManager.droptable(table_name);
}

Error API::drop_index(std::string &index_name) {
    return invalid_value;
}

Error API::insert_table(std::string &table_name, std::vector<std::string> values) {
    return invalid_value;
}

Error API::select(std::string &table_name, std::vector<std::string> conditions) {
    return invalid_value;
}

Error API::delete_table(std::string &table_name, std::vector<std::string> conditions) {
    return invalid_value;
}
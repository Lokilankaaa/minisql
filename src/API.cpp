//
// Created by 薛伟 on 2020/4/29.
//

#include "API.h"

using namespace API;

Error create_table(string &table_name, vector<string> &args);

Error create_database(string &database_name);

Error create_index(string &index_name, string &table_name, string &column_name);

Error drop_table(string &table_name);

Error drop_database(string &database_name);

Error drop_index(string &index_name);

Error insert_table(string &table_name, vector<string> values);

Error select(string &table_name, vector<string> conditions);

Error delete_table(string &table_name, vector<string> conditions);

Error update_table();

Error use_database(string &database_name);
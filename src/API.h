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
    static int check_type(std::string &val);

    static CONSTRAINT check_cons(std::string &str);

//    static void find_del(data &d, std::vector<data> &vd);

    static bool find_in (data &d, std::vector<data> &vd);

protected:
    catalog_manager catalogManager;

    RecordManager rec_manager;

    static table joinTable(std::vector<table> &tables);

public:
    Error create_table(std::string &table_name, attributes_set &attrs, Index &index, int pk);

    Error create_index(std::string index_name, std::string &table_name, std::string &column_name);

    Error drop_table(std::string &table_name);

    static Error drop_index(std::string &index_name);

    Error insert_table(std::string &table_name, std::vector<std::string> &values);

    table select(std::vector<std::string> &attrs, std::vector<std::string> &tables, std::vector<std::string> &conds);

    int delete_table(std::string &table_name, std::vector<std::string> &conditions);

};

#endif //MINISQL_API_H

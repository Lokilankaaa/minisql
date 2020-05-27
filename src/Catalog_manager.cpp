//
// Created by 薛伟 on 2020/4/29.
//

#include "Catalog_manager.h"
#include "error.h"
#include "Grammar.h"

catalog_manager::catalog_manager() : db("test") {

}

Error catalog_manager::createtable(string &table_name, attributes_set &attrs, int pk) {
    if (hastable(table_name)) {
        return table_not_exist;
    } else {
        attrs.unique[pk] = true;
        std::string meta_data = "0000 ";
        meta_data += table_name + " ";
        meta_data += num2str(attrs.num) + " ";
        for (int i = 0; i < attrs.num; ++i) {
            meta_data += attrs.name[i] + " " + num2str(attrs.type[i]) + " " + num2str(attrs.unique[i]) + " ";
        }
        meta_data += num2str(pk) + " ";
//        for (int j = 0; j < ; ++j) {
//
//        }
    }
    return successful;
}

bool catalog_manager::hastable(string &table_name) {
    return false;
}

Error catalog_manager::droptable(string &table_name) {
    if (!hastable(table_name)) {
        return table_not_exist;
    } else {

    }
    return successful;
}

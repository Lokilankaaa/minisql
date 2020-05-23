//
// Created by 薛伟 on 2020/4/29.
//

#include "Catalog_manager.h"
#include "error.h"

catalog_manager::catalog_manager() : db("test") {

}

Error catalog_manager::createtable(string &table_name, attributes_set &attrs, int pk) {
    if (hastable(table_name)) {
        return table_not_exist;
    } else {

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

//
// Created by 薛伟 on 2020/4/29.
//

#include "API.h"

Error API::create_table(std::string &table_name, attributes_set &attrs, Index &index, int pk) {
    if (catalog_manager::createtable(table_name, attrs, index, pk) == successful)
        rec_manager.createTableFile(table_name);
    else
        return table_exist;
    return successful;
}

Error API::drop_table(std::string &table_name) {
    if (catalog_manager::droptable(table_name) == successful)
        rec_manager.dropTableFile(table_name);
    else
        return table_not_exist;
    return table_exist;
}

Error API::create_index(std::string &index_name, std::string &table_name, std::string &column_name) {
    auto res1 = catalog_manager::createindex(table_name, column_name, index_name);
    auto res2 = successful;

    if (res1 == successful and res2 == successful)
        return successful;
}

Error API::drop_index(std::string &index_name) {
    return invalid_value;
}

Error API::insert_table(std::string &table_name, std::vector<std::string> values) {
    TUPLE tuple;
    for (auto &i:values) {
        auto d = new data;
        d->type = check_type(i);
        if (d->type == -1)
            d->int_data = str2num<int>(i);
        else if (d->type == 0)
            d->float_data = str2num<float>(i);
        else
            d->char_data = i.substr(1, i.size() - 2);
        tuple.addData(*d);
    }
    rec_manager.insertRecord(table_name, tuple);
    return successful;
}

Error API::delete_table(std::string &table_name, std::vector<std::string> conditions) {
    return invalid_value;
}

Error API::select(std::string &table_name, std::vector<std::string> conditions) {
    return invalid_value;
}

int API::check_type(std::string &val) {
    if (val[0] == '\'' and val[val.size() - 1] == '\'')
        return val.size() - 2;
    else {
        if (num2str(str2num<int>(val)) == val)
            return -1;
        else
            return 0;
    }
}
//
// Created by 薛伟 on 2020/4/29.
//

#include "API.h"

int API::check_type(std::string &val) {
    if (val[0] == '\'' and val[val.size() - 1] == '\'')
        return static_cast<int>(val.size()) - 2;
    else {
        if (num2str(str2num<int>(val)) == val)
            return -1;
        else
            return 0;
    }
}

CONSTRAINT API::check_cons(std::string &str) {
    if (str == "<")
        return LESS;
    else if (str == "<=")
        return LESS_OR_EQUAL;
    else if (str == "=")
        return EQUAL;
    else if (str == ">=")
        return MORE_OR_EQUAL;
    else if (str == ">")
        return MORE;
    else if (str == "<>")
        return NOT_EQUAL;
    else
        throw e_syntax_error();
}

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
    return successful;
}

Error API::insert_table(std::string &table_name, std::vector<std::string> &values) {
    TUPLE tuple;
    if (catalog_manager::hastable(table_name) == -1)
        throw e_table_not_exist();
    auto attrs = catalog_manager::getAllattrs(table_name);
    if (values.size() != attrs.num)
        throw e_syntax_error();
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

Error API::create_index(std::string &index_name, std::string &table_name, std::string &column_name) {
    if (catalog_manager::createindex(table_name, column_name, index_name) == successful) {
        if (!catalog_manager::check_unique(table_name, column_name))
            throw e_index_define_error();
        IndexManager idx_manager(table_name);
        std::string idx_file_path = "INDEX_FILE_" + column_name + "_" + table_name + "_" + index_name;
        int type;
        auto attrs = catalog_manager::getAllattrs(table_name);
        for (int i = 0; i < attrs.num; ++i) {
            if (attrs.name[i] == column_name) {
                type = attrs.type[i];
                break;
            }
        }
        idx_manager.CreateIndex(idx_file_path, type);
        rec_manager.createIndex(idx_manager, table_name, column_name);
        return successful;
    } else {
        throw e_attribute_not_exist();
    }
}

Error API::drop_index(std::string &index_name) {
//    auto file_path = IndexManager::FindTableName(index_name);
//    auto table_name = split(file_path, '_').back();
//    catalog_manager::dropindex(table_name, index_name);
//    IndexManager idx_manager(table_name);
//    auto type = catalog_manager::getIndexType(index_name, table_name);
//    idx_manager.DropIndex(file_path, type);
    return successful;
}

int API::delete_table(std::string &table_name, std::vector<std::string> &conditions) {
    if (catalog_manager::hastable(table_name) == -1)
        throw e_table_not_exist();
    if (conditions.empty()) {
        return rec_manager.deleteRecord(table_name);
    } else {
        std::vector<std::string> target_attr;
        std::vector<Constraint> target_cons;
        for (auto &i:conditions) {
            auto res = split(i, ' ');
            data tmp_data;
            Constraint tmp_con;
            tmp_data.type = check_type(res[2]);
            if (tmp_data.type == -1) {
                tmp_data.int_data = str2num<int>(res[2]);
            } else if (!tmp_data.type) {
                tmp_data.float_data = str2num<float>(res[2]);
            } else {
                tmp_data.char_data = res[2].substr(1, res[2].size() - 2);
            }
            tmp_con.conData = tmp_data, tmp_con.conSymbol = check_cons(res[1]);
            target_attr.push_back(res[0]);
            target_cons.push_back(tmp_con);
        }
        return rec_manager.deleteRecord(table_name, target_attr, target_cons);
    }
}

//只实现单表查询
table API::select(std::vector<std::string> &attrs, std::vector<std::string> &tables, std::vector<std::string> &conds) {
    if (tables.size() == 1) {
        if (catalog_manager::hastable(tables[0]) == -1)
            throw e_table_not_exist();
        if (attrs[0] != "*") {
            auto t_attrs = catalog_manager::getAllattrs(tables[0]);
            bool a_flag = true;
            for (auto &i:attrs) {
                bool l_flag = false;
                for (int j = 0; j < t_attrs.num; j++) {
                    if (i == t_attrs.name[j])
                        l_flag = true;
                }
                a_flag &= l_flag;
            }
            if (!a_flag)
                throw e_attribute_not_exist();
        }
        if (conds.empty()) {
            return rec_manager.selectRecord(tables[0]);
        } else {
            std::vector<table> ts;
            for (auto &i:conds) {
                auto res = split(i, ' ');
                data tmp_data;
                Constraint tmp_con;
                tmp_data.type = check_type(res[2]);
                if (tmp_data.type == -1) {
                    tmp_data.int_data = str2num<int>(res[2]);
                } else if (!tmp_data.type) {
                    tmp_data.float_data = str2num<float>(res[2]);
                } else {
                    tmp_data.char_data = res[2];
                }
                tmp_con.conData = tmp_data, tmp_con.conSymbol = check_cons(res[1]);
                auto t = rec_manager.selectRecord(tables[0], res[0], tmp_con);
                ts.push_back(t);
            }
            return joinTable(ts);
        }
    } else {
        throw e_syntax_error();
    }
}

//void API::find_del(data &d, std::vector<data> &vd) {
//    std::vector<data> n_vd;
//    for (auto &i : vd) {
//        if (d == i)
//            n_vd.push_back(i);
//    }
//    vd = n_vd;
//}

bool API::find_in(data &d, std::vector<data> &vd) {
    for (auto &i:vd) {
        if (d == i)
            return true;
    }
    return false;
}

table API::joinTable(vector<table> &tables) {
    auto attrs = tables[0].getAttr();
    table n_table("tmp", attrs);
    int pk = attrs.primary_key;
    vector<data> remain_list;
    auto tmp = tables[0].getTuple();
    for (auto &i : tmp) {
        remain_list.push_back(i.getData()[pk]);
    }
    for (auto &t : tables) {
        auto t_tuples = t.getTuple();
        vector<data> tmp_;
        for (auto &i:t_tuples) {
            if (find_in(i.getData()[pk], remain_list)) {
                tmp_.push_back(i.getData()[pk]);
            }
        }
        remain_list = tmp_;
    }
    for (auto &i:tmp) {
        if (find_in(i.getData()[pk], remain_list))
            n_table.addTuple(i);
    }
    return n_table;
}
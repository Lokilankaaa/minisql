//
// Created by 薛伟 on 2020/4/29.
//

#include "interpreter.h"

#include <sstream>
#include <string>
#include <iostream>
#include "error.h"
#include <vector>

extern map<string, IndexManager*> im_map;

void Interpreter::get_script() {
    std::string tmp;
    script.clear();
    do {
        std::cout << "minisql > ";
        getline(std::cin, tmp);
        script += tmp + " ";
    } while (tmp[tmp.size() - 1] != ';');
    script = script.substr(0, script.size() - 2);
    normalize(script);
}

void Interpreter::normalize(std::string &one_sql) {
    for (int i = 0; i < one_sql.size(); ++i) {
        if (one_sql[i] == '*' or one_sql[i] == '=' or one_sql[i] == ',' or one_sql[i] == '(' or one_sql[i] == ')' or
            one_sql[i] == '>' or one_sql[i] == '<') {
            if (one_sql[i - 1] != ' ')
                one_sql.insert(i++, " ");
            if (one_sql[i + 1] != ' ')
                one_sql.insert(++i, " ");
        }
        if (one_sql[i] == '\t' or one_sql[i] == '\n' or one_sql[i] == '\r')
            one_sql[i] = ' ';
    }

    for (auto i = one_sql.begin(); i < one_sql.end() - 1;) {
        if (*i == ' ' and *(i + 1) == ' ') {
            auto tmp_i = i - 1;
            one_sql.erase(i);
            i = tmp_i + 1;
            continue;
        }
        i++;
    }
    auto it = one_sql.begin();
    while ((*it) == ' ') {
        one_sql.erase(it);
        it = one_sql.begin();
    }

//    one_sql = toLower(one_sql);
}

Interpreter::Interpreter() = default;

Interpreter::~Interpreter() = default;

void Interpreter::execute() {
    try {
        if (script.substr(0, 6) == "select") {
            exec_select(script);
        } else if (script.substr(0, 4) == "drop") {
            if (script.substr(5, 5) == "table")
                exec_drop_table(script);
            else if (script.substr(5, 5) == "index")
                exec_drop_index(script);
            else
                throw e_syntax_error();
        } else if (script.substr(0, 6) == "insert") {
            exec_insert_table(script);
        } else if (script.substr(0, 6) == "create") {
            if (script.substr(7, 5) == "table")
                exec_create_table(script);
            else if (script.substr(7, 5) == "index")
                exec_create_index(script);
            else
                throw e_syntax_error();
        } else if (script.substr(0, 6) == "delete")
            exec_delete_table(script);
        else if (script.substr(0, 4) == "exit" and script.size() == 4)
            exec_quit();
        else if (script.substr(0, 8) == "execfile")
            execfile(script);
        else
            throw e_syntax_error();
    }

    catch (const e_syntax_error &e) {
        std::cout << "minisql > ERROR! Unrecognized syntax!" << std::endl;
    }

    catch (const e_exit_command &e) {
        std::cout << "minisql > Bye!" << std::endl;
        for(auto &i:im_map) {
            delete i.second;
        }
        exit(0);
    }

    catch (const e_unknown_file_path &e) {
        std::cout << "minisql > ERROR! Unknown file path!" << std::endl;
    }

    catch (const e_table_exist &e) {
        std::cout << "minisql > ERROR! Table exists!" << std::endl;
    }

    catch (const e_table_not_exist &e) {
        std::cout << "minisql > ERROR! Table doesn't exist!" << std::endl;
    }

    catch (const e_attribute_not_exist &e) {
        std::cout << "minisql > ERROR! The attribute doesn't exist." << std::endl;
    }

    catch (const e_index_exist &e) {
        std::cout << "minisql > ERROR! The index has existed." << std::endl;
    }

    catch (const e_index_not_exist &e) {
        std::cout << "minisql > ERROR! The index doesn't existed." << std::endl;
    }

    catch (const e_index_full &e) {
        std::cout << "minisql > ERROR! The number of index of the table has been full." << std::endl;
    }

    catch (const e_tuple_type_conflict &e) {
        std::cout << "minisql > ERROR! The tuple inserted is not compatible with the table." << std::endl;
    }

    catch (const e_data_type_conflict &e) {
        std::cout << "minisql > ERROR! The data type is not compatible with the table." << std::endl;
    }

    catch (const e_invalid_pk &e) {
        std::cout << "minisql > ERROR! PK undefined." << std::endl;
    }

    catch (const e_unique_conflict &e) {
        std::cout << "minisql > ERROR! The same unique data has been inserted." << std::endl;
    }

    catch (const e_primary_key_conflict &e) {
        std::cout << "minisql > ERROR! The data with same pk has been inserted." << std::endl;
    }

    catch (const e_index_define_error &e) {
        std::cout << "minisql > ERROR! Index created in normal attribute." << std::endl;
    }
}

void Interpreter::exec_create_table(std::string &sql) {
    std::map<std::string, std::string> name_attr;
    std::string table_name;
    attributes_set attrs;
    vector<std::string> order;
    int pk, cnt = 0;
    auto res = check_create_table(sql, table_name, name_attr, order);
    if (res == successful) {
        std::map<std::string, int> tmp_at;
        for (auto &i:name_attr) {
            if (i.first != table_name) {
                tmp_at[i.first] = cnt;
                attrs.name[cnt] = i.first;
                attrs.num++;
                if (str2num<int>(i.second) >= -1 and str2num<int>(i.second) <= 255) {
                    attrs.type[cnt] = str2num<int>(i.second);
                } else if (str2num<int>(i.second) == -1000) {
                    attrs.type[cnt] = -1;
                    attrs.unique[cnt] = true;
                } else if (str2num<int>(i.second) == -100) {
                    attrs.type[cnt] = 0;
                    attrs.unique[cnt] = true;
                } else {
                    attrs.type[cnt] = str2num<int>(i.second) / 1000;
                    attrs.unique[cnt] = true;
                }
                cnt++;
            }
        }
        if (tmp_at.count(name_attr[table_name]) > 0)
            tmp_at[table_name] = tmp_at[name_attr[table_name]];
        else
            throw e_invalid_pk();
        pk = tmp_at[table_name];
        change_order(attrs, order, pk);
        Index i;
        auto start = chrono::system_clock::now();
//        i.num = 1;
//        i.name[0] = attrs.name[attrs.primary_key]+"pk";
//        i.location[0] = attrs.primary_key;
        if (api.create_table(table_name, attrs, i, pk) != successful)
            throw e_table_exist();
        else {
            auto end = chrono::system_clock::now();
            auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "minisql > Success in " << duration.count() << " ms" << std::endl;
        }
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_drop_table(std::string &sql) {
    std::string dropped_table;
    auto res = check_drop_table(sql, dropped_table);
    if (res == successful) {
        auto start = chrono::system_clock::now();
        if (api.drop_table(dropped_table) != successful)
            throw e_table_not_exist();
        else {
            auto end = chrono::system_clock::now();
            auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "minisql > Success in " << duration.count() << " ms" << std::endl;
        }
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::execfile(std::string &sql) {
    auto split_res = split(sql, ' ');
    if (split_res.size() > 2)
        throw e_syntax_error();
    else {
        std::fstream sql_file(split_res[1]);
        if (sql_file.fail())
            throw e_unknown_file_path();
        else {
            std::stringstream ss;
            ss << sql_file.rdbuf();
            std::string tmp = ss.str();
            auto tmp_sqls = split(tmp, ';');
            for (auto i = tmp_sqls.begin(); i < tmp_sqls.end(); ++i) {
                normalize(*i);
                script = *i;
                execute();
            }
        }
    }
}

void Interpreter::exec_quit() {
    throw e_exit_command();
}

void Interpreter::exec_insert_table(std::string &sql) {
    std::vector<std::string> vals;
    std::string table_name;
    auto res = check_insert_table(sql, table_name, vals);
    if (res == successful) {
        auto start = chrono::system_clock::now();
        if (api.insert_table(table_name, vals) == successful) {
            auto end = chrono::system_clock::now();
            auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "minisql > Success in " << duration.count() << " ms" << std::endl;
        }
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_create_index(std::string &sql) {
    std::string table_name, attr, idx_name;
    auto res = check_create_index(sql, table_name, attr, idx_name);
    if (res == successful) {
        auto start = chrono::system_clock::now();
        if (api.create_index(idx_name, table_name, attr) == successful) {
            auto end = chrono::system_clock::now();
            auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "minisql > Success in " << duration.count() << " ms" << std::endl;
        }
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_delete_table(std::string &sql) {
    std::string table_name;
    std::vector<std::string> conds;
    auto res = check_delete_table(sql, table_name, conds);
    if (res == successful) {
        auto start = chrono::system_clock::now();
        auto num = api.delete_table(table_name, conds);
        auto end = chrono::system_clock::now();
        auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (num >= 0)
            std::cout << "minisql > " << num << " records deleted in " << duration.count() << " ms" << std::endl;
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_drop_index(std::string &sql) {
    std::string dropped_idx;
    auto res = check_drop_index(sql, dropped_idx);
    if (res == successful) {
        auto start = chrono::system_clock::now();
        if (API::drop_index(dropped_idx) == successful) {
            auto end = chrono::system_clock::now();
            auto duration =
                    std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            std::cout << "minisql > Success in " << duration.count() << " ms" << std::endl;
        }
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_select(std::string &sql) {
    std::map<std::string, std::vector<std::string> > clause_content;
    auto res = check_select(sql, clause_content);
    if (res == successful) {
        auto start = chrono::system_clock::now();
        auto table = api.select(clause_content["attr"], clause_content["from"], clause_content["where"]);
        auto end = chrono::system_clock::now();
        auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (table.getTupleSize()) {
            if (clause_content["attr"].size() == 1 and clause_content["attr"][0] == "*") {
                for (int i = 0; i < table.getAttr().num; ++i) {
                    std::cout << "| " << table.getAttr().name[i] << " ";
                }
                std::cout << "|" << std::endl;
                for (int j = 0; j < table.getTupleSize(); ++j) {
                    for (int i = 0; i < table.getTuple()[j].getTupleSize(); ++i) {
                        auto type = table.getTuple()[j].getData()[i].type;
                        if (type == -1)
                            std::cout << "| " << table.getTuple()[j].getData()[i].int_data << " ";
                        else if (!type)
                            std::cout << "| " << table.getTuple()[j].getData()[i].float_data << " ";
                        else
                            std::cout << "| " << table.getTuple()[j].getData()[i].char_data << " ";
                    }
                    std::cout << "|" << std::endl;
                }
                std::cout << "minisql > Success in " << duration.count() << " ms with " << table.getTupleSize()
                          << " rows" << std::endl;
            } else if (!clause_content["attr"].empty()) {
                for (int i = 0; i < clause_content["attr"].size(); ++i) {
                    std::cout << "| " << clause_content["attr"][i] << " ";
                }
                std::cout << "|" << std::endl;
                auto loca_res = exibit_location(clause_content["from"][0], clause_content["attr"]);
                for (int j = 0; j < table.getTupleSize(); ++j) {
                    for (int loca_re : loca_res) {
                        auto type = table.getTuple()[j].getData()[loca_re].type;
                        if (type == -1)
                            std::cout << "| " << table.getTuple()[j].getData()[loca_re].int_data << " ";
                        else if (!type)
                            std::cout << "| " << table.getTuple()[j].getData()[loca_re].float_data << " ";
                        else
                            std::cout << "| " << table.getTuple()[j].getData()[loca_re].char_data << " ";
                    }
                    std::cout << "|" << std::endl;
                }
                std::cout << "minisql > Success in " << duration.count() << " ms with " << table.getTupleSize()
                          << " rows" << std::endl;
            } else {
                throw e_syntax_error();
            }
        } else {
            std::cout << "minisql > Result is empty!" << std::endl;
        }
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::change_order(attributes_set &attrs, vector<std::string> &order, int &pk) {
    attributes_set n_attrs;
    n_attrs.num = attrs.num;
    for (int i = 0; i < order.size(); ++i) {
        for (int j = 0; j < attrs.num; ++j) {
            if (order[i] == attrs.name[j]) {
                n_attrs.name[i] = attrs.name[j];
                n_attrs.unique[i] = attrs.unique[j];
                n_attrs.type[i] = attrs.type[j];
            }
        }
    }
    for (int k = 0; k < order.size(); ++k) {
        if (n_attrs.name[k] == attrs.name[pk]) {
            pk = k, n_attrs.primary_key = pk;
            break;
        }
    }
    attrs = n_attrs;
}

std::vector<int> Interpreter::exibit_location(std::string &table_name, std::vector<std::string> &attr) {
    auto allattrs = catalog_manager::getAllattrs(table_name);
    vector<int> locations;
    for (auto &i:attr) {
        for (int j = 0; j < allattrs.num; j++) {
            if (i == allattrs.name[j]) {
                locations.push_back(j);
            }
        }
    }
    return locations;
}

std::string Interpreter::toLower(std::string str) {
    for (auto i = str.begin(); i < str.end(); ++i) {
        if (*i >= 'A' and *i <= 'Z')
            *i += 32;
    }
    return str;
}

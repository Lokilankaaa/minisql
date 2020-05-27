//
// Created by 薛伟 on 2020/4/29.
//

#include "interpreter.h"

#include <utility>
#include <sstream>
#include <string>
#include <iostream>
#include "error.h"
#include <vector>

std::string Interpreter::toLower(string str) {
    for (auto i = str.begin(); i < str.end(); ++i) {
        if (*i >= 'A' and *i <= 'Z')
            *i += 32;
    }
    return str;
}

void Interpreter::get_script() {
    std::string tmp;
    do {
        std::cout << ">>> ";
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
        if (one_sql[i] == '\t' or one_sql[i] == '\n')
            one_sql[i] = ' ';
    }

    for (auto i = one_sql.begin(); i < one_sql.end();) {
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

    one_sql = toLower(one_sql);
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
        else if (script.substr(0, 4) == "exit" and script[5] == '\0')
            exec_quit();
        else if (script.substr(0, 8) == "execfile")
            execfile(script);
        else
            throw e_syntax_error();
    }

    catch (const e_syntax_error &e) {
        std::cout << ">>> ERROR! Unrecognized syntax!" << std::endl;
    }

    catch (const e_exit_command &e) {
        std::cout << ">>> Bye!" << std::endl;
        exit(0);
    }

    catch (const e_unknown_file_path &e) {
        std::cout << ">>> Unknown file path!" << std::endl;
    }

    catch (const e_table_exist &e) {
        std::cout << ">>> table exists!" << std::endl;
    }

    catch (const e_table_not_exist &e) {
        std::cout << ">>> table doesn't exist!" << std::endl;
    }
}

void Interpreter::exec_create_table(string &sql) {
    map<std::string, std::string> name_attr;
    std::string table_name;
    attributes_set attrs;
    int pk, cnt = 0;
    auto res = check_create_table(sql, table_name, name_attr);
    if (res == successful) {
        map<std::string, int> tmp_at;
        for (auto &i:name_attr) {
            if (i.first != table_name) {
                tmp_at[i.first] = cnt;
                attrs.name[cnt] = i.first;
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
        pk = tmp_at[table_name];
        if (api.create_table(table_name, attrs, pk) != successful)
            throw e_table_exist();
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_drop_table(string &sql) {
    std::string dropped_table;
    auto res = check_drop_table(sql, dropped_table);
    if (res == successful) {
        if (api.drop_table(dropped_table) != successful)
            throw e_table_not_exist();
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::execfile(string &sql) {
    auto split_res = split(sql, ' ');
    if (split_res.size() > 2)
        throw e_syntax_error();
    else {
        fstream sql_file(split_res[1]);
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

void Interpreter::exec_create_index(string &sql) {
    std::string table_name, attr, idx_name;
    auto res = check_create_index(sql, table_name, attr, idx_name);
    if (res == successful) { ;
    } else {
        throw e_syntax_error();
    }
}


void Interpreter::exec_drop_index(string &sql) {
    std::string dropped_idx;
    auto res = check_drop_index(sql, dropped_idx);
    if (res == successful) { ;
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_delete_table(string &sql) {
    std::string table_name;
    std::vector<std::string> conds;
    auto res = check_delete_table(sql, table_name, conds);
    if (res == successful) { ;
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_insert_table(string &sql) {
    std::vector<std::string> vals;
    std::string table_name;
    auto res = check_insert_table(sql, table_name, vals);
    if (res == successful) { ;
    } else {
        throw e_syntax_error();
    }
}

void Interpreter::exec_select(string &sql) {
    std::map<std::string, std::vector<std::string> > clause_content;
    auto res = check_select(sql, clause_content);
    if (res == successful) { ;
    } else {
        throw e_syntax_error();
    }
}


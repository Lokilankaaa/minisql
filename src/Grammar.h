//
// Created by 薛伟 on 2020/5/3.
//

#ifndef MINISQL_GRAMMAR_H
#define MINISQL_GRAMMAR_H

#include <string>
#include <vector>
#include <map>
#include "error.h"
#include <sstream>


class Grammar {
protected:
    static void _split_(const std::string &s, char delim, std::vector<std::string> &elems);

    static std::vector<std::string> split(const std::string &s, char delim);

public:
    Grammar();

    static Error
    check_create_table(std::string &sql, std::string &table_name, std::map<std::string, std::string> &name_attr);

    static Error check_select(std::string &sql, std::map<std::string, std::vector<std::string> > &clause_content);

    static Error
    check_create_index(std::string &sql, std::string &table_name, std::string &attr, std::string &idx_name);

    static Error check_drop_index(std::string &sql, std::string &dropped_idx);

    static Error check_drop_table(std::string &sql, std::string &table_name);

    static Error check_insert_table(std::string &sql, std::string &table, std::vector<std::string> &vals);

    static Error check_delete_table(std::string &sql, std::string &table, std::vector<std::string> &conds);

};

template<class type>
std::string num2str(type num) {
    return std::to_string(num);
}

template<class type>
type str2num(std::string &str) {
    std::istringstream iss(str);
    type n;
    iss >> n;
    return n;
}


#endif //MINISQL_GRAMMAR_H

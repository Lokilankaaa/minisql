//
// Created by 薛伟 on 2020/4/29.
//

#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H

#include <string>
#include <vector>
#include "API.h"
#include "Grammar.h"
#include "Catalog_manager.h"
#include <chrono>

class Interpreter : public Grammar {
private:
    std::string script;

    API api{};

    static void normalize(std::string &one_sql);

    static std::string toLower(std::string str);

    static void change_order(attributes_set &attrs, std::vector<std::string> &order, int &pk);

    std::vector<int> exibit_location(std::string &table_name, std::vector<std::string> &attr);



public:
    Interpreter();

    ~Interpreter();

    void get_script();

    void execute();

    void exec_select(std::string &sql);

    void exec_create_table(std::string &sql);

    void exec_create_index(std::string &sql);

    void exec_drop_table(std::string &sql);

    static void exec_drop_index(std::string &sql);

    void exec_delete_table(std::string &sql);

    void exec_insert_table(std::string &sql);

    static void exec_quit();

    void execfile(std::string &sql);

};

#endif //MINISQL_INTERPRETER_H
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

class Interpreter : public Grammar {
private:
    string script;

    static void normalize(std::string &one_sql);

    static std::string toLower(std::string str);

public:
    Interpreter();

    ~Interpreter();

    void get_script();

    void execute();

    void exec_select(string &sql);

    void exec_create_table(string &sql);

    void exec_create_index(string &sql);

    void exec_drop_table(string &sql);

    void exec_drop_index(string &sql);

    void exec_delete_table(string &sql);

    void exec_insert_table(string &sql);

    void exec_quit();

    void execfile(string &sql);

};

#endif //MINISQL_INTERPRETER_H
//
// Created by 薛伟 on 2020/4/29.
//

#ifndef MINISQL_INTERPRETER_H
#define MINISQL_INTERPRETER_H
#include <string>
#include <vector>
#include "API.h"
#include "Grammar.h"

using namespace std;

class Interpreter: public Grammar{
private:
    string script;
    vector<string> sqls;
    void parse();
public:
    Interpreter();
    ~Interpreter();
    void get_script(string script);

};

#endif //MINISQL_INTERPRETER_H

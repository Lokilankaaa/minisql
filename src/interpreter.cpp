//
// Created by 薛伟 on 2020/4/29.
//

#include "interpreter.h"

#include <utility>

void Interpreter::get_script(string script) {
    this->script = std::move(script);
    parse();

}

void Interpreter::parse() {
    string tmp;
    for (auto &i:this->script) {
        if (i == '(' || i == ')' || i == ',' || i == '\n') {
            tmp += ' ';
        } else if (i != ';') {
            tmp += i;
        } else {
            sqls.push_back(tmp);
            tmp = "";
        }
    }
}

Interpreter::Interpreter() = default;

Interpreter::~Interpreter() = default;

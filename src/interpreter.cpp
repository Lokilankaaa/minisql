//
// Created by 薛伟 on 2020/4/29.
//

#include "interpreter.h"

#include <utility>
#include <sstream>

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

void Interpreter::_split_(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> Interpreter::split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    _split_(s, delim, elems);
    return elems;
}

void Interpreter::execute() {

}

void Interpreter::SELECT() {

}

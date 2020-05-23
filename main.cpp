//
// Created by 薛伟 on 2020/4/29.
//

#include "src/interpreter.h"
#include <iostream>

int main(){
    Interpreter i;
    vector<std::string> tmp;
//    string sql="create table test(id varchar(10), idd varchar(20));create table test2(id char(10), idd int);";
    fstream f("./test.txt");
    i.get_script();
    i.execute();
    return 0;
}
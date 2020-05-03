//
// Created by 薛伟 on 2020/4/29.
//

#include "src/interpreter.h"
#include <iostream>

int main(){
    Interpreter i;
    string sql="create table test(id varchar(10), idd varchar(20));create table test2(id varchar(10), idd varchar(20));";
    i.get_script(sql);
    return 0;
}
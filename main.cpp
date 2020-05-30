//
// Created by 薛伟 on 2020/4/29.
//

#include "src/interpreter.h"
#include "src/buffer_manager.h"

BufferManager buf_manager;

int main(){
    Interpreter i;
    while (true) {
        i.get_script();
        i.execute();
    }
    return 0;
}
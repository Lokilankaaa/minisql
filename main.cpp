//
// Created by 薛伟 on 2020/4/29.
//

#include "interpreter.h"
#include "buffer_manager.h"

BufferManager buf_manager;

map<string, IndexManager*> im_map;

int main(){
    Interpreter i;
    while (true) {
        i.get_script();
        i.execute();
    }
    return 0;
}

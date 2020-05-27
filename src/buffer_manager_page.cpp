//
// Created by lfy on 2020/5/18.
//

#include "buffer_manager_page.h"

//初始化一个page
buffer_manager_page::buffer_manager_page() {
    for(int i=0; i<PAGESIZE; i++){
        oneBuffer[i] = '\0';
    }
    fileName = "";
    fileBlockId = -1;
    allPinNum = -1;
    isAvaliable = true;
    isModify = false;
    relBit = false;
}


//用于重新初始化该页面
void buffer_manager_page::resetPage(){
    for(int i=0; i<PAGESIZE; i++){
        oneBuffer[i] = '\0';
    }
    fileName = "";
    fileBlockId = -1;
    allPinNum = -1;
    isAvaliable = true;
    isModify = false;
    relBit = false;
}

//设置所有的私有成员变量
void buffer_manager_page::setAllData(std::string file_name, int file_block_id, int all_pin_num, bool is_avaliable, bool is_modify, bool rel_bit){
    fileName = file_name;
    fileBlockId = file_block_id;
    allPinNum = all_pin_num;
    isAvaliable = is_avaliable;
    isModify = is_modify;
    relBit = rel_bit;
}






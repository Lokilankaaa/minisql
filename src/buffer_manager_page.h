//
// Created by lfy on 2020/5/18.
//

#ifndef MINISQL_BUFFER_RECORD_BUFFER_MANAGER_PAGE_H
#define MINISQL_BUFFER_RECORD_BUFFER_MANAGER_PAGE_H


/****该类被封装，不作为对外的接口使用，仅用于buffer_manager****/
/****该类用于记录内存中的每一页****/
/****每一页存储磁盘文件对应的一个块****/


//定义每一页的大小为4096字节，即4KB
#define PAGESIZE 4096


#include <iostream>
#include <string>


class buffer_manager_page {
public:
    //构造函数
    buffer_manager_page();

    //使用inline关键字提高效率
    //设置来自的文件名和获取该文件名的函数
    inline void setFileName(std::string file_name){fileName = file_name;}
    inline std::string getFileName(){return fileName;}
    //设置改页在所属文件中的块号的设置与获取
    inline void setBlockId(int block_id){fileBlockId = block_id;}
    inline int getBlockId(){return fileBlockId;}
    //该页被钉住的次数
    inline void setAllPinNum(int all_pin_num){allPinNum = all_pin_num;}
    //返回该页被钉住的次数
    inline int getAllPinNum(){return allPinNum;}
    //设置与获取改页是否可用
    inline void setAvaliable(bool is_avaliable){isAvaliable = is_avaliable;}
    inline bool getAvaliable(){return isAvaliable;}
    //设置与获取改页是否被修改的状态
    inline void setModify(bool is_modify){isModify = is_modify;}
    inline bool getModify(){return isModify;}
    //设置与获取该页用于时钟替换策略的使用位
    inline void setRefBit(bool ref_bit){relBit = ref_bit;}
    inline bool getRefBit(){return relBit;}
    //以指针的形式获取这个buffer，从而使得buffer_manager对其进行控制
    inline char* getBuffer(){return oneBuffer;}
    //用于统一对所有的私有成员进行设置，防止一个一个调用带来的麻烦
    void setAllData(std::string file_name, int file_block_id, int all_pin_num, bool is_avaliable, bool is_modify, bool rel_bit);

    //用于重置改页，相当于删除该页原来的内容
    void resetPage();
private:
    char oneBuffer[PAGESIZE];   //每一页的大小是PAGESIZE
    std::string fileName;       //这一页来自的文件名称
    int fileBlockId;            //这一页来自的文件的块号
    int allPinNum;              //这一页被钉住的次数
    bool isAvaliable;           //这一页是否是空闲的，如果是则可以将新的块加载进该页面
    bool isModify;              //这一页是否被修改
    bool relBit;                //这一页的使用位，用于时钟替换策略
};


#endif

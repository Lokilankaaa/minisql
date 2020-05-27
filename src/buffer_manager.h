//
// Created by lfy on 2020/5/18.
//

#ifndef MINISQL_BUFFER_RECORD_BUFFER_MANAGER_H
#define MINISQL_BUFFER_RECORD_BUFFER_MANAGER_H

//BufferManager类: 提供用于操作缓冲区的接口
/*  使用说明:
 *  首先实例化一个BufferManager类。
 *  通过getPage(file_name, block_id)接口可以得到对应文件对应块在内存中的句柄,即可读取和修改页的内容
 *  通过getPageId(file_name, block_id)接口获取块在内存中的页号
 *  如果修改了对应页的内容，需要调用modifyPage(page_id)接口来标记该页已经被修改，否则修改可能会丢失
 *  如果当前操作需要持续使用某一页，则需要调用pinPage(page_id)接口来将该页钉住，防止被替换
 *  如果不再需要使用改页，通过unpinPage(page_id)接口将该页解除
 *  通过flushPage(page_id, file_name, block_id)接口，可以将内存中的一页写到文件中的一块
 */


//最大页数为100，用于默认构造函数
#define MAXFRAMESIZE 100

#include "buffer_manager_page.h"
#include <string>
#include <iostream>

class BufferManager {
public:
    //构造函数，默认构造函数构造最大的缓冲池，重构的构造函数根据frame_size构造缓冲池
    BufferManager();
    BufferManager(int frame_size);
    //析构函数
    ~BufferManager();
    // 通过页号得到页的句柄(一个页的头地址)，可以读取修改其中的内容
    char* getPage(std::string file_name , int block_id);
    // 标记page_id所对应的页已经被修改
    void modifyPage(int page_id);
    // 钉住一个页
    void pinPage(int page_id);
    // 解除一个页的钉住状态(需要注意的是一个页可能被多次钉住，该函数只能解除一次)
    // 如果对应页的allPinNum为0，则返回-1，表示改页没有被钉住
    int unpinPage(int page_id);
    // 将对应内存页写入对应文件的对应块。这里的返回值为0或-1，代表文件是否成功被打开
    // 但是因为函数内部通过r+打开文件，这是总能成功的，因此返回值无太大意义
    int flushPage(int page_id , std::string file_name , int block_id);
    // 获取对应文件的对应块在内存中的页号，没有找到返回-1
    int getPageId(std::string file_name , int block_id);
private:
    buffer_manager_page* Frames;    //缓冲池，是一个Page的数组，通过堆分配内存空间，更加灵活
    int frame_size_;                //记录总共的页数
    int current_position_;          //用于时钟替换策略的位置变量
    //因为可以采用默认初始化和重构的初始化，因此他们统一调用该函数来初始化
    void initialize(int frame_size);
    //通过时钟替换策略获取一个闲置的页的页号，使用者不需要关心获取的方式
    int getEmptyPageId();
    //将对应文件的对应块载入对应的内存页，文件不存在返回-1，否则返回1
    int loadDiskBlock(int page_id , std::string file_name , int block_id);
};


#endif

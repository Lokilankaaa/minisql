//
// Created by lfy on 2020/5/18.
//

#include "buffer_manager.h"

//两个构造函数都通过调用initialize函数来进行初始化，以做到统一标准
BufferManager::BufferManager(int frame_size) {
    initialize(frame_size);
}


//实际的初始化函数，根据传入的参数决定缓冲池的大小
void BufferManager::initialize(int frame_size) {
    Frames = new buffer_manager_page[frame_size];
    frame_size_ = frame_size;
    current_position_ = 0;
}



//析构函数
//在程序结束时候需要将缓冲池里的所有页写回磁盘，防止数据的丢失
//写回磁盘的操作调用了flushPage函数
BufferManager::~BufferManager(){
    for(int i=0; i<frame_size_; i++){
        std::string file_name;
        int block_id;
        //依次获取每页的名称和所在页的块id
        file_name = Frames[i].getFileName();
        block_id = Frames[i].getBlockId();
        flushPage(i, file_name, block_id);
    }
}

//该函数根据文件名和id将对应的块写回文件
int BufferManager::flushPage(int page_id, std::string file_name , int block_id){
    FILE * f = fopen(file_name.c_str(), "r+");
    if(f == NULL) return -1;
    //设置要写入的位置
    fseek(f, PAGESIZE*block_id, SEEK_SET);
    //获取对应页的句柄
    char * buffer = Frames[page_id].getBuffer();
    //将该页内存的内容写回
    fwrite(buffer, PAGESIZE, 1, f);
    fclose(f);
    return 0;
}



//根据file_name和block_id返回指向该页的句柄，从而能够实现读取和修改
//这里首先调用了getPageId在缓冲池查找相应的页号
//如果没有找到则需要通过时钟替换策略找到要被替换的块getEmptyPageId，并调用loadDiskBlock从磁盘中将该块加载下来
//之后将该页的使用位置1，然后返回对应的buffer
char* BufferManager::getPage(std::string file_name , int block_id){
    int pageId = getPageId(file_name, block_id);
    if(pageId == -1){
        pageId = getEmptyPageId();
        loadDiskBlock(pageId, file_name, block_id);
    }
    //首次装入内存中以及之后的每次访问，使用位置1
    Frames[pageId].setRefBit(true);
    return Frames[pageId].getBuffer();
}

//遍历所有缓冲池，查找是否对应文件名和block_id的页号
int BufferManager::getPageId(std::string file_name , int block_id){
    std::string tempFileName;
    int tempBlockId;
    for(int i=0; i<frame_size_; i++){
        tempFileName = Frames[i].getFileName();
        tempBlockId = Frames[i].getBlockId();
        if(tempBlockId==block_id && tempFileName==file_name){
            //如果存在返回对应的i
            return i;
        }
    }
    //如果不存在则返回-1;
    return -1;
}

//使用时钟替换策略查找一个闲置的页
//即找到第一个refBit为false的页
int BufferManager::getEmptyPageId(){
    //首先查看是否有已存在的闲置页
    for(int i=0; i<frame_size_; i++){
        if(Frames[i].getAvaliable() == true)
            return i;
    }
    //如果缓冲池已经被填满，则需要替换一个已存在的页
    //这里采用时钟替换策略
    while (true){
        //每当遇到一个使用位为1的页框时，操作系统就将该位重新置为0
        if(Frames[current_position_].getRefBit()== true){
            Frames[current_position_].setRefBit(false);
        }
            //否则如果改页的allPinNum为0，代表改页没有被锁住，因此可以删除以腾出缓冲池空间
        else if(Frames[current_position_].getAllPinNum()<=0){
            //查看改页有没有被修改，如果被修改则需要将其写回磁盘后才能进行删除
            if(Frames[current_position_].getModify() == true){
                std::string file_name =  Frames[current_position_].getFileName();
                int block_id = Frames[current_position_].getBlockId();
                flushPage(current_position_, file_name, block_id);
            }
            //之后只需要直接删除改页(即重新初始化即可)
            Frames[current_position_].resetPage();
            //然后返回找到的页号
            return current_position_;
        }
        //类似时钟一样循环地移动，直到找到第一个可以替换的页，实现ringBuffer的效果
        current_position_ = (current_position_ + 1) % frame_size_;
    }
}

//根据文件名和块号从磁盘中加载一块数据 到Frames缓冲区的page_id处
int BufferManager::loadDiskBlock(int page_id , std::string file_name , int block_id){
    //首先清空改页，放置前面某处操作失误使得数据残留
    Frames[page_id].resetPage();
    //以读取的模式打开磁盘文件
    FILE *f = fopen(file_name.c_str(), "r");
    //打开失败则返回-1
    if(f==NULL) return -1;
    //将文件指针定位到第block_id块处
    fseek(f, PAGESIZE*block_id, SEEK_SET);
    //根据page_id获取一个页的句柄
    char *emptyBuffer = Frames[page_id].getBuffer();
    //从磁盘中读取对应的块到内存页，并关闭文件
    fread(emptyBuffer, PAGESIZE, 1, f);
    fclose(f);
    //对该页进行的相关数据进行设置
    Frames[page_id].setAllData(file_name,block_id,0, false, false, true);
    //如果成功返回1;
    return 1;
}



//如果修改了该页需要调用该函数接口标记修改
void BufferManager::modifyPage(int page_id) {
    Frames[page_id].setModify(true);
}

//增加该页被钉住的次数，因为同一个页可能被不同模块使用，因此钉住的次数需要累加
void BufferManager::pinPage(int page_id) {
    int tempPinNum = Frames[page_id].getAllPinNum();
    tempPinNum++;
    Frames[page_id].setAllPinNum(tempPinNum);
}

//解除一次钉住的状态，只有钉住的状态归零以后才有可能被写回磁盘
int BufferManager::unpinPage(int page_id){
    int tempPinNum = Frames[page_id].getAllPinNum();
    if(tempPinNum <= 0) return -1;
    else{
        Frames[page_id].setAllPinNum(tempPinNum-1);
        return 0;
    }
}

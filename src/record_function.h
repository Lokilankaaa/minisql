//
// Created by 陆飞洋 on 2020/5/20.
//

#ifndef MINISQL_BUFFER_RECORD_RECORD_FUNCTION_H
#define MINISQL_BUFFER_RECORD_RECORD_FUNCTION_H

#include <sstream>


//该接口不作为对外接口，仅为record_manager的操作提供便利



//把不同类型的数据转换为string类型后写入内存块
template <typename T>
void writeInto(char *p, int &offset, T oneData){
    std::stringstream is;
    is << oneData;
    std::string stringData = is.str();
    for(char i : stringData){
        p[offset++] = i;
    }
}

//用于比较传入的两个数据是否满足条件
template<typename T>
bool satisfyWithCondition(T first, T second, Constraint rel){
    if(rel.conSymbol==LESS){
        return first < second;
    }
    else if(rel.conSymbol==LESS_OR_EQUAL){
        return first <= second;
    }
    else if(rel.conSymbol==EQUAL){
        return first == second;
    }
    else if(rel.conSymbol==MORE_OR_EQUAL){
        return first >= second;
    }
    else if(rel.conSymbol==MORE){
        return first > second;
    }
    else if(rel.conSymbol==NOT_EQUAL){
        return first != second;
    }
    return false;
}


//用于获取不同类型数据的长度
template<typename T>
int getDataLength(T data){
    std::stringstream is;
    is << data;
    return is.str().length();
}

#endif

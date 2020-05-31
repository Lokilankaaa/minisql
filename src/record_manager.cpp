//
// Created by lfy on 2020/5/18.
//

#include "record_manager.h"


//获取该文件一共有多少个block
int RecordManager::countBlockNum(std::string table_name){
    char *p;
    int count = -1;
    while(true){
        p = buf_manager.getPage(table_name, count+1);
        count++;
        if(p[0]=='\0') break;
    }
    return count;
}

//从传入指针的内存位置读取一个TUPLE(即一条记录的数据信息)
TUPLE RecordManager::readOneTuple(const char* p, attributes_set attr){
    TUPLE all_tuple;
    //初始化到起始位置
    p = p+5;
    for(int i=0;i<attr.num;i++){
        data temp_data;
        temp_data.type = attr.type[i];
        //先拷贝数据到inData
        char inData[300];
        int j;
        for(j=0; *p!=' ';j++,p++){
            inData[j] = *p;
        }
        //为读取下一个属性准备
        p++;
        inData[j] = '\0';
        //将数据存入temp_data
        std::string temp_str(inData);
        if(temp_data.type==-1) {
            //int
            std::stringstream is(temp_str);
            is >> temp_data.int_data;
        }else if(temp_data.type==0){
            //float
            std::stringstream is(temp_str);
            is >> temp_data.float_data;
        } else{
            //char
            temp_data.char_data = temp_str;
        }
        all_tuple.addData(temp_data);
    }
    if(*p == '1') all_tuple.setState();
    return all_tuple;
}

//获取一个TUPLE的长度(即一条记录的长度)
int RecordManager::getTupleLength(char* p){
    char temp[20];
    int i;
    for(i=0; p[i]!=' ';i++) temp[i] = p[i];
    temp[i] = '\0';
    std::string change_str(temp);
    return stoi(change_str);
}

//带索引查找满足条件的块,结果保存在传入的allBlockId
void RecordManager::searchBlockWithIndex(std::string table_name, std::string target_attr, Constraint target_cons, std::vector<int> &allBlockId){
    IndexManager im(table_name);
    data temp_data;
    std::string temp_path = "index_on_"+target_attr+"_"+table_name;
    if(target_cons.conSymbol==MORE || target_cons.conSymbol==MORE_OR_EQUAL){
        if(target_cons.conData.type==-1){
            temp_data.type = -1;
            temp_data.int_data = POSINF;
        }
        else if(target_cons.conData.type==0){
            temp_data.type = 0;
            temp_data.float_data = POSINF;
        }
        else{
            temp_data.type = -2;
        }
        im.SearchRange(temp_path, target_cons.conData, temp_data, allBlockId);
    }
    else if(target_cons.conSymbol==LESS || target_cons.conSymbol==LESS_OR_EQUAL){
        if(target_cons.conData.type==-1){
            temp_data.type = -1;
            temp_data.int_data = NEGINF;
        }
        else if(target_cons.conData.type==0){
            temp_data.type = 0;
            temp_data.float_data = NEGINF;
        }
        else{
            temp_data.type = 1;
            temp_data.char_data="";
        }
        im.SearchRange(temp_path, temp_data, target_cons.conData, allBlockId);
    }
    else{
        im.SearchRange(temp_path, target_cons.conData, target_cons.conData, allBlockId);
    }
}

//在相应的块中查询满足条件的记录,结果保存在要输出的表的vector<Tuple>&中
void RecordManager::searchRecordAccordCons(std::string table_name, int block_id, attributes_set target_attr, int attr_index, Constraint target_cons, std::vector<TUPLE> &result){
    std::string temp_name = DATA_FILE_PATH + table_name;
    //通过buffer获得对应的块
    char *p = buf_manager.getPage(temp_name, block_id);
    char *temp = p;
    //遍历块中的所有记录
    while (true){
        if(*p == '\0' || p>temp+PAGESIZE) break;
        TUPLE tuple = readOneTuple(p, target_attr);
        if(tuple.getState() == true){
            //如果该条记录已经被删除
            p = p + getTupleLength(p);
            continue;
        }
        //如果哦该条记录没有被删除，则根据属性类型选择符合条件的记录
        std::vector<data> single_data = tuple.getData();
        if(target_attr.type[attr_index]==-1){
            //int
            if(satisfyWithCondition(single_data[attr_index].int_data, target_cons.conData.int_data, target_cons)==true){
                result.push_back(tuple);
            }
        }
        else if(target_attr.type[attr_index]==0){
            //float
            if(satisfyWithCondition(single_data[attr_index].float_data, target_cons.conData.float_data, target_cons)==true){
                result.push_back(tuple);
            }
        }
        else{
            //char
            if(satisfyWithCondition(single_data[attr_index].char_data, target_cons.conData.char_data, target_cons)==true){
                result.push_back(tuple);
            }
        }
        p = p + getTupleLength(p);
    }
}

//判断要插入的记录是否和其他记录有冲突
bool RecordManager::judgeConflict(const std::vector<TUPLE>& table_tuples, std::vector<data> &target_value, int attr_index){
    for(int i=0; i<table_tuples.size(); i++){
        if(table_tuples[i].getState()== true) continue;
        std::vector<data> allData = table_tuples[i].getData();
        if(target_value[attr_index].type==-1){
            //int
            if(target_value[attr_index].int_data == allData[attr_index].int_data) return true;
        }
        else if(target_value[attr_index].type==0){
            //float
            if(target_value[attr_index].float_data == allData[attr_index].float_data) return true;
        }
        else{
            //char
            if(target_value[attr_index].char_data == allData[attr_index].char_data) return true;
        }
    }
    return false;
}

//insertRecord调用的函数，表示在该位置插入一条记录
void RecordManager::insertOneRecord(char *p, int offset, int len, const std::vector<data> &record_values){
    //将长度转换为字符串
    std::stringstream stream;
    stream << len;
    std::string stringLen = stream.str();
    //将长度补齐为4个字节
    while (stringLen.length() < 4) stringLen = "0" + stringLen;
    //将代表记录的长度写入
    for(int i=0; i<stringLen.length(); i++, offset++) p[offset] = stringLen[i];
    //依次写入各个数据
    for(int i=0; i<record_values.size(); i++){
        p[offset++] = ' ';    //每个属性前加一个空格
        data temp_data = record_values[i];
        if(temp_data.type==-1){
            //int
            writeInto(p, offset, temp_data.int_data);
        }
        else if(temp_data.type==0){
            //float
            writeInto(p, offset, temp_data.float_data);
        }
        else{
            //string
            writeInto(p, offset, temp_data.char_data);
        }
    }
    //增加结尾的一些字符
    p[offset] = ' ';
    p[offset + 1] = '0';
    p[offset + 2] = '\n';
}

//deleteRecord—>deleteRecordAccordCons—>调用的函数，真正地删除一个数据
char* RecordManager::deleteOneRecord(char *p){
    int len = getTupleLength(p);
    p = p + len;
    *(p-2) = '1';
    return p;
}


int RecordManager::deleteRecordAccordCons(std::string table_name, int block_id, attributes_set target_attr, std::vector<int> attr_index, std::vector<Constraint> target_cons){
    std::string temp_path = DATA_FILE_PATH + table_name;
    char *p = buf_manager.getPage(temp_path, block_id);
    char *temp = p;
    //记录删除的元素的个数
    int count=0;
    while (true){
        if(*p=='\0' || p>=temp+PAGESIZE) break;
        TUPLE tuple = readOneTuple(p, target_attr);
        std::vector<data> allData = tuple.getData();
        bool judge = true;
        for(int i=0; i<attr_index.size(); i++){
            //遍历查看该项是否满足删除的条件
            if(target_attr.type[attr_index[i]]==-1){
                //int
                if(satisfyWithCondition(allData[attr_index[i]].int_data, target_cons[i].conData.int_data, target_cons[i])==false){
                    judge = false;
                }
            }
            else if(target_attr.type[attr_index[i]]==0){
                //float
                if(satisfyWithCondition(allData[attr_index[i]].float_data, target_cons[i].conData.float_data, target_cons[i])==false){
                    judge = false;
                }
            }
            else{
                //char
                if(satisfyWithCondition(allData[attr_index[i]].char_data, target_cons[i].conData.char_data, target_cons[i])==false){
                    judge = false;
                }
            }
        }
        if(judge==true){
            //代表满足条件
            p = deleteOneRecord(p);
            count++;
        } else{
            //如果不满足条件则下一条记录
            p = p + getTupleLength(p);
        }
    }
    //标记该块已经被修改
    int page_id = buf_manager.getPageId(temp_path, block_id);
    buf_manager.modifyPage(page_id);
    return count;
}



//功能：为表创建相应的文件
//异常：无异常处理，交由catalog managar处理
//输入：所要建立的表的名字
//输出：void
void RecordManager::createTableFile(std::string table_name){
    std::string temp_path = DATA_FILE_PATH + table_name;
    FILE *f = fopen(temp_path.c_str(),"w");
    fclose(f);
}

//功能：删除表相对应的文件
//异常：无异常处理，交由catalog managar处理
//输入：所要删除的表的名字
//输出：void
void RecordManager::dropTableFile(std::string table_name){
    std::string temp_path = DATA_FILE_PATH + table_name;
    remove(temp_path.c_str());
}

//功能：根据表名返回整张表
//异常：如果表不存在则抛出e_table_not_exist异常
//输入：table_name:表名, result_table_name:返回的table的table_name,默认为tmp_table
//输出：代表整张表的table类型对象
table RecordManager::selectRecord(std::string table_name, std::string result_table_name){
    std::string temp_path = DATA_FILE_PATH + table_name;
    //得到有多少block
    int block_num = countBlockNum(temp_path);
    //如果该文件为空，则将其置为1，读取'\0'字节
    if(block_num<=0) block_num=1;
    //获取该table的属性集合
    catalog_manager cate;
    attributes_set allAttr = cate.getAllattrs(table_name);
    //构建返回的table实例
    table res_table(table_name, allAttr);
    //遍历所有的块，获得想要的数据
    for(int i=0; i<block_num; i++){
        char *p = buf_manager.getPage(temp_path, i);
        char *temp = p;
        while(true){
            if(*p=='\0' || p>=temp+PAGESIZE){
                break;
            }
            else{
                TUPLE one_tuple = readOneTuple(p, allAttr);
                if(one_tuple.getState() == false){
                    //if this tuple has not been deleted
                    res_table.addTuple(one_tuple);
                }
                p = p + getTupleLength(p);
            }
        }
    }
    return res_table;
}


//功能：带条件的查询，返回满足Constraint条件的记录的table
//异常：如果表不存在，则抛出e_table_not_exist异常;如果属性不存在，则抛出e_attribute_not_exist异常;如果where中；两个数据不匹配，则抛出e_data_type_conflict异常
//输入：(表名, where子句的属性名, where子句的约束条件, 生成的表名)
//输出：满足where子句条件的table
table RecordManager::selectRecord(std::string table_name, std::string target_attr, Constraint target_cons, std::string result_table_name){
    std::string temp_path = DATA_FILE_PATH + table_name;
    //获取该table的属性集合
    catalog_manager cate;
    attributes_set allAttr = cate.getAllattrs(table_name);
    //index用于记录对应的条件的属性的下标
    int index = -1;
    //judge判断该属性是否存在索引
    bool judge = false;
    //遍历获取目标属性的
    for(int i=0; i<allAttr.num; i++){
        if(allAttr.name[i] == target_attr){
            //找到了对应的属性
            index = i;
            //查找是否存在索引
            if(allAttr.hasIndex[i] == true) judge = true;
            break;
        }
    }
    //如果目标属性不存在，则抛出异常
    if(index == -1) throw e_attribute_not_exist();
    //如果限制的条件对应的数据类型不符
    if(allAttr.type[index] != target_cons.conData.type) throw e_data_type_conflict();

    //接下来构建返回值table
    table res_table(result_table_name, allAttr);
    if(judge==true && target_cons.conSymbol != NOT_EQUAL){
        std::vector<int> all_Blocks;
        //使用索引查找
        searchBlockWithIndex(table_name, target_attr, target_cons, all_Blocks);
        std::vector<TUPLE> save;
        for(int i=0; i<all_Blocks.size(); i++){
            searchRecordAccordCons(table_name, all_Blocks[i], allAttr, index, target_cons, save);
        }
        for(auto i=save.begin();i!=save.end(); i++) res_table.addTuple(*i);
    }
    else{
        //不使用索引查找
        std::vector<TUPLE> save;
        int count_block = countBlockNum(temp_path);
        if(count_block <= 0) count_block = 1;
        for(int i=0; i<count_block; i++){
            searchRecordAccordCons(table_name, i, allAttr, index, target_cons, save);
        }
        for(auto i=save.begin();i!=save.end(); i++) res_table.addTuple(*i);
    }
    return res_table;
}

//功能：向对应表中插入一条记录
//异常：如果代表记录的元祖类型不匹配,抛出e_tuple_type_conflict;如果主键冲突,抛出e_unique_conflict;
//     如果与已存在的主键冲突,抛出e_primary_key_conflict;如果表不存在，抛出e_table_not_exist
//输入：(表名,要插入的记录TUPLE)
//输出：void
void RecordManager::insertRecord(std::string table_name, TUPLE& tuple){
    std::string temp_path = DATA_FILE_PATH + table_name;
    catalog_manager cate;
    attributes_set allAttr = cate.getAllattrs(table_name);
    //检查属性是不是一一对应
    std::vector<data> values = tuple.getData();
    for(int i=0; i<values.size(); i++){
        if(values[i].type != allAttr.type[i])
            throw e_tuple_type_conflict();
    }
    //检查是否存在主键冲突
    table temp_table = selectRecord(table_name);
    const std::vector<TUPLE> &allTuples = temp_table.getTuple();
    if(allAttr.primary_key >= 0){
        if(judgeConflict(allTuples, values, allAttr.primary_key) == true)
            throw e_primary_key_conflict();
    }
    //检查是否存在unique冲突
    for(int i=0; i<allAttr.num; i++){
        if(allAttr.unique[i] == true){
            if(judgeConflict(allTuples, values, i)== true){
                throw e_unique_conflict();
            }
        }
    }

    //接下来处理数据的插入
    int count_block = countBlockNum(temp_path);
    if(count_block <= 0)
        count_block = 1;
    char *p = buf_manager.getPage(temp_path, count_block-1);
    //寻找一个空闲的位置
    int i,j,len=0;
    for(i=0; p[i]!='\0'&&i<PAGESIZE;i++);
    for(j=0; j<values.size(); j++){
        data the_data = values[j];
        if(the_data.type==-1){
            //int
            len = len + getDataLength(the_data.int_data);
        }
        else if(the_data.type==0){
            //float
            len = len + getDataLength(the_data.float_data);
        }
        else{
            //char
            len = len + the_data.char_data.length();
        }
    }
    //加上其他位置的length
    len = len + values.size() + 7;
    int which_block;
    if(PAGESIZE - i>=len){
        //如果剩下的空间够插入一个新的数据
        which_block = count_block - 1;
        insertOneRecord(p, i, len, values);
        int page_id = buf_manager.getPageId(temp_path, which_block);
        buf_manager.modifyPage(page_id);
    }
    else{
        //如果剩下的空间不够插入一个新的数据
        which_block = count_block;
        char *p = buf_manager.getPage(table_name, which_block);
        insertOneRecord(p, 0, len, values);
        int page_id = buf_manager.getPageId(temp_path, which_block);
        buf_manager.modifyPage(page_id);
    }

    //最后还需要更新一下索引
    IndexManager im(table_name);
    for(int i=0; i<allAttr.num; i++){
        if(allAttr.hasIndex[i] == true){
            //如果某个属性存在索引
            std::string theName = allAttr.name[i];
            std::string thePath = "index_on_"+theName+"_"+table_name;
            std::vector<data> allData = tuple.getData();
            im.InsertIndex(thePath, allData[i], which_block);
        }
    }
}


//功能：删除对应表中的所有记录，保留表文件
//异常：如果表不存在，则抛出e_table_not_exist异常
//输入：想要清空数据的表名
//输出：删除的记录数量
int RecordManager::deleteRecord(std::string table_name){
    std::string temp_path = DATA_FILE_PATH + table_name;
    int count_block = countBlockNum(temp_path);
    if(count_block <= 0) return 0;

    catalog_manager cate;
    attributes_set allAttr = cate.getAllattrs(table_name);
    IndexManager im(table_name);
    int count = 0;
    for(int i=0; i<count_block; i++){
        //获取每一块
        char *p = buf_manager.getPage(temp_path, i);
        char *temp = p;
        while (true){
            if(*p =='\0' || p >= temp+PAGESIZE) break;
            TUPLE tuple = readOneTuple(p, allAttr);
            for(int j=0; j<allAttr.num; j++){
                if(allAttr.hasIndex[j]== true){
                    //如果有索引的话
                    std::string theName = allAttr.name[i];
                    std::string thePath = "index_on_"+theName+"_"+table_name;
                    std::vector<data> allData = tuple.getData();
                    im.DeleteIndexByKey(thePath, allData[j]);
                }
            }
            //将该条记录删除
            p = deleteOneRecord(p);
            count++;
        }
        //标记修改了buffer
        int page_id = buf_manager.getPageId(temp_path, i);
        buf_manager.modifyPage(page_id);
    }
    //返回被删除的数据的数量
    return count;
}

//功能：对表中的记录创建索引
//异常：如果表不存在，则抛出e_table_not_exist异常;如果对应属性不存在，则抛出e_attribute_not_exist异常
//输入：(索引管理类对象的引用,表名,属性名)
//输出：void
void RecordManager::createIndex(IndexManager &index_manager, std::string table_name, std::string target_attr){
    std::string temp_path = DATA_FILE_PATH + table_name;
    catalog_manager cate;
    attributes_set allAttr = cate.getAllattrs(table_name);
    //用于标记目标属性的下标
    int index = -1;
    for(int i=0; i<allAttr.num; i++){
        if(allAttr.name[i] == target_attr){
            index = i;
            break;
        }
    }
    //如果没有找到对应的属性
    if(index == -1){
        throw e_attribute_not_exist();
    }
    //接下来创建对应的索引
    int count_block = countBlockNum(temp_path);
    if(count_block <= 0) count_block=1;
    //获取对应表的索引文件
    std::string indexFilePath = "index_on_"+target_attr+"_"+table_name;
    for(int i=0; i<count_block; i++){
        char *p = buf_manager.getPage(temp_path, i);
        char *temp = p;
        while(true){
            if(*p=='\0' || p >= temp+PAGESIZE) break;
            //读取一条记录
            TUPLE one_tuple = readOneTuple(p, allAttr);
            if(one_tuple.getState()== false){
                std::vector<data> allData = one_tuple.getData();
                index_manager.InsertIndex(indexFilePath, allData[index], i);
            }
            p = p + getTupleLength(p);
        }
    }
}

//功能带有多个条件的删除，删除满足vector<Constraint>所有条件的记录
//异常：如果对应属性不存在，则抛出e_attribute_not_exist异常，如果where子句数据类型不匹配，则抛出e_data_type_conflict异常
//输入：(表名,属性集合,约束集合)
//输出：删除数据的数量
int RecordManager::deleteRecord(std::string table_name, std::vector<std::string> target_attr, std::vector<Constraint> target_cons){
    std::string temp_path = DATA_FILE_PATH + table_name;
    catalog_manager cate;
    attributes_set allAttr = cate.getAllattrs(table_name);

    //构建表示目标属性下标和是否有索引的vector容器
    std::vector<int> index;
    std::vector<bool> flag;
    //判断是否某块上存在索引
    int judge = false;
    for(int i=0; i<target_attr.size(); i++){
        index.push_back(-1);
        flag.push_back(false);
        for(int j=0; j<allAttr.num; j++){
            if(allAttr.name[j] == target_attr[i]){
                index[i] = j;
                if(allAttr.hasIndex[j] == true){
                    flag[i] = true;
                    if(target_cons[i].conSymbol!=NOT_EQUAL) judge = true;
                }
                break;
            }
        }
        //如果没有找到对应的index则抛出异常
        if(index[i] == -1) throw e_attribute_not_exist();
    }
    //遍历查看条件和数据是否匹配
    for(int i=0; i<target_cons.size(); i++){
        if(target_cons[i].conData.type != allAttr.type[index[i]]) throw e_data_type_conflict();
    }

    int count = 0;
    if(judge==true){
        //如果某一项存在索引而且该项的条件不是NOT_EQUAL
        std::set<int> final;
        for(int i=0; i<flag.size(); i++){
            //找出每一个条件所满足的block
            if(flag[i]==true && target_cons[i].conSymbol != NOT_EQUAL){
                std::vector<int> block_ids;
                searchBlockWithIndex(table_name, target_attr[i], target_cons[i], block_ids);
                for(int j=0; j<block_ids.size(); j++){
                    //插入到set当中
                    final.insert(block_ids[i]);
                }
            }
        }
        for(auto i=final.begin(); i!=final.end(); i++){
            count += deleteRecordAccordCons(table_name, *i, allAttr, index, target_cons);
        }
    }
    else{
        //如果不存在索引那么就需要遍历所有的块了
        int block_num = countBlockNum(temp_path);
        if(block_num==0) return 0;
        for(int i=0; i<block_num; i++){
            count += deleteRecordAccordCons(table_name, i, allAttr, index, target_cons);
        }
    }
    return count;
}
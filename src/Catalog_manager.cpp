//
// Created by 薛伟 on 2020/4/29.
//

#include "Catalog_manager.h"
#include "error.h"
#include "Grammar.h"


Error catalog_manager::createtable(string &table_name, attributes_set &attrs, int pk) {
    if (hastable(table_name) != -1) {
        return table_exist;
    } else {
        attrs.unique[pk] = true;
        //head info the length of meta data
        std::string meta_data = " ";
        //table_name
        meta_data += table_name + " ";
        //num of attrs
        meta_data += num2str(attrs.num) + " ";
        //attr_name type unique
        for (int i = 0; i < attrs.num; ++i) {
            meta_data += attrs.name[i] + " " + num2str(attrs.type[i]) + " " + num2str(attrs.unique[i]) + " ";
        }
        //pk
        meta_data += num2str(pk) + " ";
        //index num
        meta_data += "0 ";
        meta_data += "\n#";
        meta_data = num2str(meta_data.size() - 1) + meta_data;

        int block_num = getBlockNum(CATALOG_FILE_PATH);
        if (!block_num)
            block_num = 1;
        for (int current_block = 0; current_block < block_num; ++current_block) {
            auto buffer = buf_manager.getPage(CATALOG_FILE_PATH, current_block);
            auto page_id = buf_manager.getPageId(CATALOG_FILE_PATH, current_block);
            int length;
            for (length = 0; length < MAXPAGESIZE && buffer[length] != '#' && buffer[length]; ++length) {}
            if (length + meta_data.size() < PAGESIZE) {
                if (length && buffer[length - 1] == '#')
                    buffer[length - 1] = '\0';
                else if (buffer[length] == '#')
                    buffer[length] = '\0';
                strcat(buffer, meta_data.c_str());
                buf_manager.modifyPage(page_id);
                return successful;
            }
        }
        auto buffer = buf_manager.getPage(CATALOG_FILE_PATH, block_num);
        auto page_id = buf_manager.getPageId(CATALOG_FILE_PATH, block_num);
        strcat(buffer, meta_data.c_str());
        buf_manager.modifyPage(page_id);
    }
    return successful;
}

int catalog_manager::hastable(string &table_name) {
    int block_num = getBlockNum(CATALOG_FILE_PATH);
    if (!block_num)
        block_num = 1;
    for (int current_block = 0; current_block < block_num; current_block++) {
        auto buffer = buf_manager.getPage(CATALOG_FILE_PATH, current_block);
        std::string buf_cp(buffer);
        auto res = split(buf_cp, '\n');
        for (auto &i:res) {
            if (i != "#") {
                auto tmp_res = split(i, ' ');
                if (tmp_res[1] == table_name)
                    return current_block;
            }
        }
    }
    return -1;
}

Error catalog_manager::droptable(string &table_name) {
    auto block_num = hastable(table_name);
    if (block_num == -1) {
        return table_not_exist;
    } else {
        auto buffer = buf_manager.getPage(CATALOG_FILE_PATH, block_num);
        auto page_id = buf_manager.getPageId(CATALOG_FILE_PATH, block_num);
        std::string buf_cp(buffer);
        int buf_size = buf_cp.size();
        auto split_res = split(buf_cp, '\n');
        for (auto i = split_res.begin(); i < split_res.end(); i++) {
            auto tmp_res = split(*i, ' ');
            if (tmp_res[1] == table_name) {
                split_res.erase(i);
                break;
            }
        }
        std::string tmp;
        for (auto &i:split_res) {
            tmp += i + "\n";
        }
        tmp = tmp.substr(0, tmp.size() - 1);
        strncpy(buffer, tmp.c_str(), tmp.size());
        memset(buffer + tmp.size(), 0, buf_size - tmp.size());
        buf_manager.modifyPage(page_id);
    }
    return successful;
}

int catalog_manager::getBlockNum(const string &file_name) {
    char *p;
    int num = -1;
    do {
        p = buf_manager.getPage(file_name, ++num);
    } while (p[0]);
    return num;
}

Error catalog_manager::createindex(string &table_name, string &attr_name, string &index_name) {
    return table_exist;
}

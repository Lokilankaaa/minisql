//
// Created by 薛伟 on 2020/4/29.
//

#include "Catalog_manager.h"
#include "error.h"
#include "Grammar.h"


Error catalog_manager::createtable(std::string &table_name, attributes_set &attrs, Index &index, int pk) {
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
        meta_data += num2str(index.num) + " ";
        for (int j = 0; j < index.num; ++j) {
            meta_data += num2str(index.location[j]) + " " + index.name[j] + " ";
        }
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
        buf_manager.flushPage(page_id, CATALOG_FILE_PATH, block_num);
    }
    return successful;
}

int catalog_manager::hastable(std::string &table_name) {
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

Error catalog_manager::droptable(std::string &table_name) {
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

int catalog_manager::getBlockNum(const std::string &file_name) {
    char *p;
    int num = -1;
    do {
        p = buf_manager.getPage(file_name, ++num);
    } while (p[0]);
    return num;
}

attributes_set catalog_manager::getAllattrs(std::string &table_name) {
    auto block_num = hastable(table_name);
    auto buffer = buf_manager.getPage(CATALOG_FILE_PATH, block_num);
    std::string buf_cp(buffer);
    auto split_res = split(buf_cp, '\n');
    attributes_set attrs;
    int cnt = 0;
    for (auto &i:split_res) {
        auto tmp_res = split(i, ' ');
        if (tmp_res[1] == table_name) {
            int j = 3;
            for (; j < 3 + 3 * str2num<int>(tmp_res[2]); j += 3) {
                attrs.name[cnt] = tmp_res[j];
                attrs.type[cnt] = str2num<int>(tmp_res[j + 1]);
                attrs.unique[cnt] = str2num<int>(tmp_res[j + 2]);
                cnt++;
            }
            attrs.num = cnt;
            attrs.primary_key = str2num<int>(tmp_res[j]);
            break;
        }
    }
    return attrs;
}

bool catalog_manager::hasattribute(std::string &table_name, std::string &attr_name) {
    auto attrs = getAllattrs(table_name);
    for (int i = 0; i < attrs.num; ++i) {
        if (attr_name == attrs.name[i])
            return true;
    }
    return false;
}

Index catalog_manager::getAllindex(std::string &table_name) {
    auto block_num = hastable(table_name);
    auto buffer = buf_manager.getPage(CATALOG_FILE_PATH, block_num);
    std::string buf_cp(buffer);
    auto split_res = split(buf_cp, '\n');
    Index index;
    int cnt = 0;
    for (auto &i:split_res) {
        auto tmp_res = split(i, ' ');
        if (tmp_res[1] == table_name) {
            int offset = str2num<int>(tmp_res[2]);
            int pos = 2 + 3 * offset + 2;
            int tt = str2num<int>(tmp_res[pos]);
            if (tt == 0)
                break;
            for (int j = pos + 1; j < pos + 1 + 2 * tt; j += 2) {
                index.name[cnt] = tmp_res[j + 1];
                index.location[cnt] = str2num<int>(tmp_res[j]);
                cnt++;
            }
            index.num = cnt;
            break;
        }
    }
    return index;
}

Error catalog_manager::createindex(std::string &table_name, std::string &attr_name, std::string &index_name) {
    if (hastable(table_name) == -1)
        throw e_table_not_exist();
    if (!hasattribute(table_name, attr_name))
        throw e_attribute_not_exist();
    auto index = getAllindex(table_name);
    if (index.num == 10)
        throw e_index_full();
    
    if (!check_unique(table_name, attr_name))
        throw e_index_define_error();

    auto attrs = getAllattrs(table_name);
    for (int i = 0; i < index.num; ++i) {
        if (index.name[i] == index_name)
            throw e_index_exist();
        if (attrs.name[index.location[i]] == attr_name)
            throw e_index_exist();
    }
    index.name[index.num] = index_name;
    for (int j = 0; j < attrs.num; ++j) {
        if (attrs.name[j] == attr_name) {
            index.location[index.num] = j;
            index.num++;
            break;
        }
    }
    droptable(table_name);
    createtable(table_name, attrs, index, attrs.primary_key);
    return successful;
}

Error catalog_manager::dropindex(std::string &table_name, std::string &index_name) {
    auto n_index = getAllindex(table_name);
    for (int i = 0; i < n_index.num; ++i) {
        if (n_index.name[i] == index_name) {
            n_index.name[i] = n_index.name[n_index.num - 1];
            n_index.location[i] = n_index.location[n_index.num - 1];
            n_index.num--;
            auto attrs = getAllattrs(table_name);
            droptable(table_name);
            createtable(table_name, attrs, n_index, attrs.primary_key);
            return successful;
        }
    }
    return successful;
}

int catalog_manager::getIndexType(std::string &index_name, std::string &table_name) {
    auto idxs = getAllindex(table_name);
    auto attrs = getAllattrs(table_name);
    int pos;
    for (int i = 0; i < idxs.num; i++) {
        if (idxs.name[i] == index_name)
            pos = idxs.location[i];
    }
    return attrs.type[pos];
}

bool catalog_manager::check_unique(std::string &table_name, const std::string &attr_name) {
    auto attrs = getAllattrs(table_name);
    int pos = 0;
    for (int i = 0; i < attrs.num; ++i) {
        if (attrs.name[i] == attr_name)
            pos = i;
    }
    return attrs.unique[pos];
}

//
// Created by 薛伟 on 2020/5/3.
//

#include <sstream>
#include "Grammar.h"


Grammar::Grammar() = default;

void split_(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split_(s, delim, elems);
    for (auto i = elems.begin(); i < elems.end();) {
        if ((*i).empty()) {
            elems.erase(i);
            i = elems.begin();
            continue;
        }
        i++;
    }
    return elems;
}

Error
Grammar::check_create_table(std::string &sql, std::string &table_name, std::map<std::string, std::string> &name_attr, std::vector<std::string> &order) {
    auto split_res = split(sql, ' ');
    auto end_pos = split_res.size() - 1;
    table_name = split_res[2];
    name_attr[table_name] = num2str(0);//table name:primary key
    if (split_res[3] == "(" and split_res[end_pos] == ")") {
        for (int i = 4; i <= end_pos;) {
            if (split_res[i] == "primary" and split_res[i + 1] == "key") {
                i += 2;
                if (name_attr[split_res[2]] != "0")
                    return syntax_error;
                else {
                    if (split_res[i] == "(" and split_res[i + 2] == ")") {
                        name_attr[split_res[2]] = split_res[i + 1];
                        order.push_back(table_name);
                        if (split_res[i + 3] == ",") {
                            i += 4;
                        } else if (split_res[i + 3] == ")") {
                            break;
                        } else {
                            return syntax_error;
                        }
                    } else
                        return syntax_error;
                }
            } else if (split_res[i] != "primary" and
                       (split_res[i + 1] == "char" or split_res[i + 1] == "int" or split_res[i + 1] == "float")) {
                if (split_res[i + 1] == "int") {
                    int s = -1;
                    if (split_res[i + 2] == "unique") {
                        s *= 1000;
                    }
                    name_attr[split_res[i]] = num2str(s);
                    order.push_back(split_res[i]);
                    i += (s == -1 ? 2 : 3);
                } else if (split_res[i + 1] == "float") {
                    int s = 0;
                    if (split_res[i + 2] == "unique")
                        s -= 100;
                    name_attr[split_res[i]] = num2str(s);
                    order.push_back(split_res[i]);
                    i += (s == 0 ? 2 : 3);
                } else {
                    if (split_res[i + 2] == "(" and split_res[i + 4] == ")" and str2num<int>(split_res[i + 3])) {
                        int s = str2num<int>(split_res[i + 3]);
                        if (split_res[i + 5] == "unique")
                            s *= 1000;
                        name_attr[split_res[i]] = num2str(s);
                        order.push_back(split_res[i]);
                        i += (s < 1000 ? 5 : 6);
                    } else {
                        return syntax_error;
                    }
                }
                if (split_res[i] == "," or (split_res[i] == ")" and i == end_pos))
                    i++;
                else
                    return syntax_error;
            } else {
                return syntax_error;
            }
        }
        for (auto j = order.begin(); j < order.end(); ++j) {
            if(*j == table_name) {
                order.erase(j);
                break;
            }
        }
    } else
        return syntax_error;
    if (name_attr[split_res[2]] == "0")
        return syntax_error;
    return successful;
}

Error Grammar::check_select(std::string &sql, std::map<std::string, std::vector<std::string> > &clause_content) {
    auto split_res = split(sql, ' ');
    auto t1 = new std::vector<std::string>, t2 = new std::vector<std::string>, t3 = new std::vector<std::string>;
    clause_content["attr"] = *t1;
    clause_content["from"] = *t2;
    clause_content["where"] = *t3;
    for (auto it = split_res.begin(); it < split_res.end();) {
        if (*it == "select") {
            it++;
            while (*it != "from" and it < split_res.end()) {
                if (*it == ",") {
                    it++;
                    continue;
                } else
                    clause_content["attr"].push_back(*it);
                it++;
            }
            if (*it == "from")
                continue;
            else
                return syntax_error;
        } else if (*it == "from") {
            it++;
            while (*it != "where" and it < split_res.end()) {
                if (*it == ",") {
                    it++;
                    continue;
                } else
                    clause_content["from"].push_back(*it);
                it++;
            }
            if (*it == "where")
                continue;
            else
                return successful;
        } else if (*it == "where") {
            it++;
            while (it < split_res.end()) {
                if (*it == "and") {
                    it++;
                    continue;
                }
                std::string tmp;
                tmp += *it + " ";
                it++;
                if (*it == "=" or *it == ">" or *it == "<" or (*it == "<" and *(it + 1) == ">") or
                    (*it == ">" and *(it + 1) == "=") or
                    (*it == "<" and *(it + 1) == "=")) {
                    tmp += *it;
                    it++;
                    if (*it == "=" or *it == ">")
                        tmp += *it, it++;
                    tmp += " ";
                } else
                    return syntax_error;
                tmp += *it, it++;
                clause_content["where"].push_back(tmp);
            }
        } else
            return syntax_error;
    }
    return successful;
}

Error Grammar::check_create_index(std::string &sql, std::string &table_name, std::string &attr,
                                  std::string &idx_name) {
    //create index idx_name on table_name (attr_name)
    auto split_res = split(sql, ' ');
    if (split_res.size() > 8)
        return syntax_error;
    idx_name = split_res[2];
    if (split_res[3] == "on") {
        table_name = split_res[4];
    } else {
        return syntax_error;
    }
    if (split_res[5] == "(" and split_res[7] == ")")
        attr = split_res[6];
    else
        return syntax_error;
    return successful;
}

Error Grammar::check_drop_index(std::string &sql, std::string &dropped_idx) {
    auto split_res = split(sql, ' ');
    if (split_res.size() > 3)
        return syntax_error;
    dropped_idx = split_res[2];
    return successful;
}

Error Grammar::check_drop_table(std::string &sql, std::string &table_name) {
    auto split_res = split(sql, ' ');
    if (split_res.size() > 3)
        return syntax_error;
    table_name = split_res[2];
    return successful;
}

Error Grammar::check_insert_table(std::string &sql, std::string &table, std::vector<std::string> &vals) {
    auto split_res = split(sql, ' ');
    if (split_res[1] == "into" and split_res[3] == "values" and split_res[4] == "(" and
        split_res[split_res.size() - 1] == ")") {
        table = split_res[2];
        for (int i = 5; i < split_res.size() - 1; ++i) {
            if (split_res[i] == ",")
                continue;
            else if (split_res[i] == "'") {
                return syntax_error;
            } else
                vals.push_back(split_res[i]);
        }
    } else
        return syntax_error;
    return successful;
}

Error Grammar::check_delete_table(std::string &sql, std::string &table, std::vector<std::string> &conds) {
    auto split_res = split(sql, ' ');
    if (split_res[1] != "from")
        return syntax_error;
    table = split_res[2];
    if (split_res.size() == 3) {
        return successful;
    } else {
        if (split_res[3] == "where") {
            auto it = split_res.begin() + 4;
            while (it < split_res.end()) {
                if (*it == "and") {
                    it++;
                    continue;
                }
                std::string tmp;
                tmp += *it + " ";
                it++;
                if (*it == "=" or *it == ">" or *it == "<" or (*it == "<" and *(it + 1) == ">") or
                    (*it == ">" and *(it + 1) == "=") or
                    (*it == "<" and *(it + 1) == "=")) {
                    tmp += *it;
                    it++;
                    if (*it == "=" or *it == ">")
                        tmp += *it, it++;
                    tmp += " ";
                } else
                    return syntax_error;
                tmp += *it, it++;
                conds.push_back(tmp);
            }
            return successful;
        } else {
            return syntax_error;
        }
    }
}
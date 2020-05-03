//
// Created by 薛伟 on 2020/5/3.
//

#include "Grammar.h"

Grammar::Grammar() {
    root.content = "Nil";
    root.is_leaf = false;
    for (auto &i:sql_type) {
        if (i == "Nil" || i == "table" || i == "database" || i == "index")
            continue;
        auto tmp = new node;
        tmp->content = i;
        if (i == "create" || i == "drop") {
            for (int j = 0; j < 3; ++j) {
                auto tmp2 = new node;
                tmp2->content = sql_type[11 + j];
                tmp2->is_leaf = true;
                tmp->children.push_back(tmp2);
            }
            tmp->is_leaf = false;
        } else {
            tmp->is_leaf = true;
        }
        root.children.push_back(tmp);
    }
}

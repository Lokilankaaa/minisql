//
// Created by 薛伟 on 2020/5/3.
//

#ifndef MINISQL_GRAMMAR_H
#define MINISQL_GRAMMAR_H
#include <string>
#include <vector>

using namespace std;

struct node {
    string content;
    vector<node *>children;
    bool is_leaf;
};


class Grammar {
protected:
    string sql_type[14]={
            "Nil",
            "create",
            "delete",
            "drop",
            "select",
            "insert",
            "update",
            "quit",
            "execfile",
            "use",
            "help",
            "table",
            "database",
            "index"
    };
    node root;
public:
    Grammar();
};


#endif //MINISQL_GRAMMAR_H

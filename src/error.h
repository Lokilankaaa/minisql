//
// Created by 薛伟 on 2020/5/17.
//

#ifndef MINISQL_ERROR_H
#define MINISQL_ERROR_H

#include <exception>

enum Error {
    successful,
    syntax_error,
    invalid_identifier,
    invalid_value,
    invalid_attr_for_index,
    illegal_tuple_insert,
    insert_table_exist,
    table_not_exist,
    tuple_not_exist
};



#endif //MINISQL_ERROR_H

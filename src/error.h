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
    table_exist,
    tuple_not_exist,
    attr_not_exist
};

class e_syntax_error : public std::exception {

};

class e_unknown_file_path : public std::exception {

};

class e_table_exist : public std::exception {

};

class e_table_not_exist : public std::exception {

};

class e_attribute_not_exist : public std::exception {

};

class e_index_exist : public std::exception {

};

class e_index_not_exist : public std::exception {

};

class e_tuple_type_conflict : public std::exception {

};

class e_data_type_conflict : public std::exception {

};

class e_index_full : public std::exception {

};

class e_exit_command : public std::exception {

};

class e_unique_conflict : public std::exception {

};

class e_index_define_error : public std::exception {

};

class e_invalid_pk : public std::exception {

};

//lfy: add e_primary_key_conflict to represent primary key conflict
class e_primary_key_conflict : public std::exception{

};


#endif //MINISQL_ERROR_H

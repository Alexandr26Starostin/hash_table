#ifndef HASH_TABLE_H
#define HASH_TABLE_H

errors_in_hash_table_t create_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table);
errors_in_hash_table_t delete_hash_table (inf_hash_table_t* ptr_inf_hash_table);
errors_in_hash_table_t use_hash_table    (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table);

#endif
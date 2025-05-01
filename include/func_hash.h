#ifndef FUNC_HASH_H
#define FUNC_HASH_H

const size_t BASE_NUMBER_DJB2 = 5381;
const size_t SHIFTING_IN_GFB2 = 5;

size_t hash_sum_ascii          (char* str);
size_t hash_sum_of_squares     (char* str);
size_t hash_djb2      		   (char* str);
size_t hash_ascii_first_symbol (char* str);
size_t hash_mul_all_ascii      (char* str);
size_t hash_average            (char* str);
size_t hash_crc32              (char* str);

errors_in_hash_table_t print_inf_about_func_hash (list_t* hash_table, char* str_name_func_hash, char* name_inf_file);

#endif
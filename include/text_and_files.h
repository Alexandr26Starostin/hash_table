#ifndef TEXT_AND_FILES_H
#define TEXT_AND_FILES_H

errors_in_hash_table_t find_name_func_hash    (int argc, char** argv, size_t (**ptr_func_hash) (char*));
errors_in_hash_table_t get_text_from_file     (FILE* file_words, char** ptr_text, size_t* ptr_count_words);
errors_in_hash_table_t print_str_32_bytes     (char* str);
errors_in_hash_table_t print_words_for_search (char* words_for_search, size_t count_words);

#endif
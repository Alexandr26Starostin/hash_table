#ifndef TEXT_AND_FILES_H
#define TEXT_AND_FILES_H

errors_in_hash_table_t find_name_func_hash      (int argc, char** argv, size_t (**ptr_func_hash) (char*));
errors_in_hash_table_t get_word_from_file       (FILE* file_words, char* word);
errors_in_hash_table_t get_words_for_search     (int argc, char** argv, inf_for_search_t* ptr_inf_for_search);
errors_in_hash_table_t delete_words_for_search  (inf_for_search_t inf_for_search);
errors_in_hash_table_t print_str_32_bytes       (char* str);
errors_in_hash_table_t initialize_aligned_alloc (char* array);

#endif
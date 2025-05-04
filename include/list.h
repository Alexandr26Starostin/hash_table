#ifndef LIST_H
#define LIST_H

errors_in_hash_table_t ctor_list 		             (list_t* ptr_list);
errors_in_hash_table_t dtor_list		             (list_t* ptr_list);
errors_in_hash_table_t add_element_in_list           (list_t* ptr_list, data_t data);
errors_in_hash_table_t print_list 		   			 (list_t* ptr_list);
size_t                 find_element_in_list			 (list_t* ptr_list, data_t data);
errors_in_hash_table_t get_element_from_index        (list_t* ptr_list, size_t find_index_element, data_t* ptr_return_data);

#endif
#ifndef LIST_H
#define LIST_H

errors_in_hash_table_t ctor_list 		      (list_t* ptr_list);
errors_in_hash_table_t dtor_list		      (list_t* ptr_list);
errors_in_hash_table_t add_element_in_list    (list_t* ptr_list, data_t data);
errors_in_hash_table_t print_list 		   	  (list_t* ptr_list);
extern "C" size_t      find_element_in_list	  (list_t* ptr_list, data_t data);
errors_in_hash_table_t get_element_from_index (list_t* ptr_list, size_t find_index_element, data_t* ptr_return_data);

//----------------------------------------------------------------------------------------------------------------------------------

#ifndef INTRINSICS

bool compare_element (data_t element_1, data_t element_2);

#else

#include <immintrin.h>

[[gnu::always_inline]] inline bool compare_element (data_t element_1, data_t element_2);   //data_t == char*

[[gnu::always_inline]] inline bool compare_element (data_t element_1, data_t element_2)
{
	//data_t == char[32]
	
	assert (element_1);
	assert (element_2);

	__m256i result_cmp = _mm256_cmpeq_epi64 (*(__m256i*) element_1, *(__m256i*) element_2);

	return ! (_mm256_movemask_epi8 (result_cmp) - MASK_IF_ELEM_EQUAL);
}

#endif

//----------------------------------------------------------------------------------------------------------------------------------

#endif
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "const_in_hash_table.h"
#include "text_and_files.h"
#include "list.h"

static errors_in_hash_table_t dtor_nodes (node_t* element);

errors_in_hash_table_t print_m256i (__m256i elem);

//------------------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t ctor_list (list_t* ptr_list)   
{
	assert (ptr_list);

	ptr_list -> size_of_list = MIN_SIZE_OF_LIST;

	node_t* first_element = (node_t*) calloc (COUNT_NODES_IN_CALLOC, sizeof (node_t));
	if (first_element == NULL)
	{
		printf ("Error in %s:%d\nHave not memory for first_element in try to create list\n\n", __FILE__, __LINE__);
		return HAVE_NOT_MEMORY_FOR_FIRST_ELEMENT;
	}

	first_element -> next_element    = NULL;
	first_element -> counter_element = MIN_COUNTER_ELEMENT;

	// #ifdef INTRINSICS
	// first_element -> data = (char*) aligned_alloc (ALIGNMENT, MAX_BYTES_IN_WORD * sizeof (char));
	// #else
	// first_element -> data = (char*) calloc (MAX_BYTES_IN_WORD, sizeof (char));
	// #endif

	// if (first_element -> data == NULL)
	// {
	// 	printf ("Error in %s:%d\nHave not memory for first_element -> data in try to create list\n\n", __FILE__, __LINE__);
	// 	return NOT_MEMORY_FOR_ELEMENT;
	// }

	// #ifdef INTRINSICS
	// initialize_aligned_alloc (first_element -> data);
	// #endif

	first_element -> data = NULL;
	
	// if (strncpy(first_element -> data, "\0", MAX_LEN_WORD) == NULL)
	// {
	// 	printf ("Error in %s:%d\n"
	// 			"Failed strncpy();\n\n", __FILE__, __LINE__);

	// 	return FAILED_STRNCPY;
	// }

	ptr_list -> head = first_element;
	ptr_list -> tail = first_element;

	return NOT_ERROR;
}

errors_in_hash_table_t dtor_list (list_t* ptr_list)
{
	assert (ptr_list);

	dtor_nodes (ptr_list -> head);

	ptr_list -> size_of_list = SIZE_OF_DELETE_LIST;

	ptr_list -> head = NULL;
	ptr_list -> tail = NULL;

	return NOT_ERROR; 
}

static errors_in_hash_table_t dtor_nodes (node_t* element)
{
	if (! element) {return NOT_ERROR;}   //find next_element after tail

	dtor_nodes (element -> next_element);
	//free (element -> data);
	free (element);

	return NOT_ERROR;
}

errors_in_hash_table_t add_element_in_list (list_t* ptr_list, data_t data)
{
	assert (ptr_list);
	assert (data);

	node_t* find_element = (ptr_list -> head) -> next_element;

	//print_str_32_bytes (data);

	while (find_element != NULL)
	{
		//print_str_32_bytes (find_element -> data);

		if (compare_element (find_element -> data, data))  //find_element -> data == data
		{
			(find_element -> counter_element)++;   //find data in list
			return NOT_ERROR;
		}

		find_element = find_element -> next_element;
	}

	//not find data in list  

	node_t* element = (node_t*) calloc (COUNT_NODES_IN_CALLOC, sizeof (node_t));
	if (element == NULL)
	{
		printf ("Error in %s:%d\nHave not memory for element in try to add element (%s) in list\n\n", __FILE__, __LINE__, data);
		return NOT_MEMORY_FOR_ELEMENT;
	}

	element -> next_element    = NULL;
	element -> counter_element = MIN_COUNTER_ELEMENT;
	element -> data            = data;

	(ptr_list -> tail) -> next_element = element;

	ptr_list -> tail = element;

	(ptr_list -> size_of_list)++;

	return NOT_ERROR;
}

errors_in_hash_table_t print_list (list_t* ptr_list)
{
	assert (ptr_list);

	printf ("----------------------------------------------------\n"
			"list:\n"
			"size_of_list == %ld\n\n"
			"elements in list:\n"
			"index  counter_element  data\n", ptr_list -> size_of_list);

	size_t  max_index = ptr_list -> size_of_list;
	node_t* element   = ptr_list -> head;

	for (size_t index = 1; index < max_index; index++)
	{
		printf ("%5ld  %15ld  %s\n", index, element -> counter_element, 
										     element -> data);

		element = element -> next_element;
	}

	printf ("----------------------------------------------------\n\n");

	return NOT_ERROR;
}

extern "C" size_t find_element_in_list	(list_t* ptr_list, data_t data)
{
	assert (ptr_list);
	assert (data);

	node_t* find_element = (ptr_list -> head) -> next_element;

	size_t index_element = 0;

	while (find_element != NULL)
	{
		if (compare_element (find_element -> data, data))  //find_element -> data == data
		{
			#ifdef DEBUG
				printf ("Find data (%s) in list:\nindex_element in list == %ld\n", data, index_element);
			#endif

			return find_element -> counter_element + DIFFERENCE_COUNT_WORDS;
		}

		index_element++;   //!!! we count index_element in test too

		find_element = find_element -> next_element;
	}

	#ifdef DEBUG
		printf ("Cannot find data (%s) in list\n", data);
	#endif

	return NOT_FIND_ELEMENT;
}

errors_in_hash_table_t get_element_from_index (list_t* ptr_list, size_t find_index_element, data_t* ptr_return_data)
{
	assert (ptr_list);
	assert (ptr_return_data);

	size_t max_index_in_list = (ptr_list -> size_of_list) - 1;

	if (find_index_element > max_index_in_list)
	{
		printf ("Error in %s:%d\n"
				"Try to get value from outside of list\n"
				"max_index_in_list  == %ld\n"
				"find_index_element == %ld", __FILE__, __LINE__, max_index_in_list, find_index_element);

		return TRY_TO_GET_VALUE_OUTSIDE_LIST;
	}

	size_t index = 0;

	node_t* element = ptr_list -> head;

	for (; index < find_index_element; index++)
	{
		element = element -> next_element;
	}

	*ptr_return_data = element -> data;

	return NOT_ERROR;
}

#ifndef INTRINSICS

bool compare_element (data_t element_1, data_t element_2)
{
	//data_t == char*
	
	assert (element_1);
	assert (element_2);

	size_t index = 0;

	char symbol_1 = '\0';
	char symbol_2 = '\0';

	while ((symbol_1 = element_1[index]) != '\0' and (symbol_2 = element_2[index]) != '\0')
	{
		// printf ("symbol_1 = %2X\n", symbol_1);
		// printf ("symbol_2 = %2X\n", symbol_2);
		// getchar ();

		if (symbol_1 - symbol_2)
		{
			return false;   //element_1 != element_2
		}

		index++;
	}

	if (element_1[index] == '\0' and element_2[index] == '\0')
		return true;     //element_1 == element_2

	return false;
}

#endif

errors_in_hash_table_t print_m256i (__m256i elem)
{
	char* str = (char*) &elem;

	for (size_t index = 0; index < MAX_BYTES_IN_WORD; index++)
	{
		printf ("%2X", str[index]);
	}
	printf ("\n\n");

	return NOT_ERROR;
}

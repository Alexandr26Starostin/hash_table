#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "const_in_hash_table.h"
#include "list.h"

static errors_in_hash_table_t dtor_nodes (node_t* element);

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
	first_element -> data            = POISON;

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
	if (element == NULL) {return NOT_ERROR;}   //find next_element after tail

	dtor_nodes (element -> next_element);
	free (element);

	return NOT_ERROR;
}

errors_in_hash_table_t add_element_in_list (list_t* ptr_list, data_t data)   //user must check data on NULL if data - ptr
{
	assert (ptr_list);

	node_t* find_element = ptr_list -> head;

	while (find_element != NULL)
	{
		if (find_element -> data == data)
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
		printf ("Error in %s:%d\nHave not memory for element in try to add element (%ld) in list\n\n", __FILE__, __LINE__, data);
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

	for (size_t index = 0; index < max_index; index++)
	{
		printf ("%5ld  %15ld  %ld\n", index, element -> counter_element, 
										     element -> data);

		element = element -> next_element;
	}

	printf ("----------------------------------------------------\n\n");

	return NOT_ERROR;
}

errors_in_hash_table_t find_element_in_list	(list_t* ptr_list, data_t data)
{
	assert (ptr_list);

	node_t* find_element = ptr_list -> head;

	size_t index_element = 0;

	while (find_element != NULL)
	{
		if (find_element -> data == data)
		{
			printf ("Find data (%ld) in list:\nindex == %ld\n\n", data, index_element);
			return NOT_ERROR;
		}

		index_element++;

		find_element = find_element -> next_element;
	}

	printf ("Cannot find data (%ld) in list\n\n", data);

	return NOT_ERROR;
}

errors_in_hash_table_t find_element_in_list_for_test (list_t* ptr_list, data_t data)
{
	assert (ptr_list);

	node_t* find_element = ptr_list -> head;

	size_t index_element = 0;

	while (find_element != NULL)
	{
		if (find_element -> data == data)
			return NOT_ERROR;

		index_element++;

		find_element = find_element -> next_element;
	}

	return NOT_ERROR;
}
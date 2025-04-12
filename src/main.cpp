#include <stdio.h>

#include "const_in_hash_table.h"
#include "list.h"

int main ()
{
	printf ("Hello, I'm hash_table\n");

	list_t list_with_long = {};

	ctor_list (&list_with_long);

	add_element_in_list (&list_with_long, 100);
	add_element_in_list (&list_with_long, 200);
	add_element_in_list (&list_with_long, 300);
	add_element_in_list (&list_with_long, 100);
	add_element_in_list (&list_with_long, 600);
	add_element_in_list (&list_with_long, -100);
	add_element_in_list (&list_with_long, 700);
	add_element_in_list (&list_with_long, 200);
	add_element_in_list (&list_with_long, 200);

	print_list (&list_with_long);

	find_element_in_list (&list_with_long, 700);
	find_element_in_list (&list_with_long, -1);
	find_element_in_list (&list_with_long, -5);
	find_element_in_list (&list_with_long, 100);
	find_element_in_list (&list_with_long, -100);
	find_element_in_list (&list_with_long, 0);

	data_t get_value_test = INITIALIZATION_OF_DATA_T;

	get_element_from_index  (&list_with_long, 5, &get_value_test);

	printf ("get_value_test == %ld\n\n", get_value_test);

	dtor_list (&list_with_long);

	return NOT_ERROR;
}
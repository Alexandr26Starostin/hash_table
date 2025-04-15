#include <stdio.h>

#include "const_in_hash_table.h"
#include "list.h"
#include "hash_table.h"

int main (int argc, char** argv)
{
	printf ("Hello, I'm hash_table\nargv == %d\n\n", argc);

	// list_t list_with_long = {};

	// ctor_list (&list_with_long);

	// add_element_in_list (&list_with_long, "Hello");
	// add_element_in_list (&list_with_long, "Alena");
	// add_element_in_list (&list_with_long, "Eda");
	// add_element_in_list (&list_with_long, "Hello");
	// add_element_in_list (&list_with_long, ")");
	// add_element_in_list (&list_with_long, "Sasha");
	// add_element_in_list (&list_with_long, "MIPT");
	// add_element_in_list (&list_with_long, "Alena");
	// add_element_in_list (&list_with_long, "Alena");

	// print_list (&list_with_long);

	// find_element_in_list (&list_with_long, "MIPT");
	// find_element_in_list (&list_with_long, "-1");
	// find_element_in_list (&list_with_long, "-5");
	// find_element_in_list (&list_with_long, "Hello");
	// find_element_in_list (&list_with_long, "Sasha");
	// find_element_in_list (&list_with_long, "0");

	// data_t get_value_test = INITIALIZATION_OF_DATA_T;

	// get_element_from_index  (&list_with_long, 5, &get_value_test);

	// printf ("get_value_test == %s\n\n", get_value_test);

	// dtor_list (&list_with_long);

	//----------------------------------------------------------------------------------------

	errors_in_hash_table_t status = create_hash_table (/*argc,*/ argv);

	return status;
}
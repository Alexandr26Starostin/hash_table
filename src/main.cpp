#include <stdio.h>

#include "const_in_hash_table.h"
#include "hash_table.h"

int main (int argc, char** argv)
{
	inf_hash_table_t inf_hash_table = {};

	inf_hash_table.func_hash = NULL;

	errors_in_hash_table_t status = create_hash_table (argc, argv, &inf_hash_table);
	if (status) {return status;}

	//----------------------------------------------------------------------

	status = use_hash_table (argc, argv, &inf_hash_table);
	if (status) {delete_hash_table (inf_hash_table.hash_table); return status;}

	//----------------------------------------------------------------------

	delete_hash_table (inf_hash_table.hash_table);

	return status;
}
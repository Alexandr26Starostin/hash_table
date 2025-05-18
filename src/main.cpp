#include <stdio.h>

#include "const_in_hash_table.h"

size_t TABLE_FOR_CRC32 [CRC32_TABLE_SIZE] = {};

#include "hash_table.h"

int main (int argc, char** argv)
{
	inf_hash_table_t inf_hash_table = {};
	inf_hash_table.func_hash = NULL;

	inf_hash_table_t* ptr_inf_hash_table = &inf_hash_table;

	errors_in_hash_table_t status = create_hash_table (argc, argv, ptr_inf_hash_table);
	if (status) {return status;}

	//----------------------------------------------------------------------

	status = use_hash_table (argc, argv, ptr_inf_hash_table);
	if (status) {delete_hash_table (ptr_inf_hash_table); return status;}

	//----------------------------------------------------------------------

	delete_hash_table (ptr_inf_hash_table);

	return status;
}
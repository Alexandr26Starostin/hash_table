#include <stdio.h>

#include "const_in_hash_table.h"
#include "hash_table.h"

int main (int argc, char** argv)
{
	errors_in_hash_table_t status = create_hash_table (argc, argv);
	if (status) {return status;}

	return status;
}
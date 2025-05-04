#include <stdio.h>
#include <x86intrin.h>
//#include <immintrin.h>

#include "const_in_hash_table.h"
#include "hash_table.h"

int main (int argc, char** argv)
{
	// u_int64_t ticks_begin = __rdtsc();
    // printf("ticks in begin == %ld\n\n", ticks_begin);

	//----------------------------------------------------------------------

	inf_hash_table_t inf_hash_table = {};

	inf_hash_table.func_hash = NULL;

	errors_in_hash_table_t status = create_hash_table (argc, argv, &inf_hash_table);
	if (status) {return status;}
	//----------------------------------------------------------------------

	// u_int64_t ticks_end = __rdtsc();
    // printf("ticks in end == %ld\n\n", ticks_end);

	// printf ("ticks on program == %ld\n\n", ticks_end - ticks_begin);

	//----------------------------------------------------------------------

	// ticks_begin = __rdtsc();
    
	// int test_value = (int) status;
	// test_value++;

	// ticks_end = __rdtsc();

	// printf ("ticks on test_value == %ld\n\n", ticks_end - ticks_begin);

	// ticks_begin = __rdtsc();
	// ticks_end   = __rdtsc();

	// printf ("ticks on error == %ld\n\n", ticks_end - ticks_begin);

	//----------------------------------------------------------------------

	status = use_hash_table (argc, argv, &inf_hash_table);
	if (status) {delete_hash_table (inf_hash_table.hash_table); return status;}


	// printf ("The\\n -- %ld\n", inf_hash_table.func_hash ("The\n"));
	// printf ("The   -- %ld\n", inf_hash_table.func_hash ("The"));
	// printf ("the\\n -- %ld\n", inf_hash_table.func_hash ("the\n"));
	// printf ("the   -- %ld\n\n", inf_hash_table.func_hash ("the"));

	delete_hash_table (inf_hash_table.hash_table);

	return status;
}
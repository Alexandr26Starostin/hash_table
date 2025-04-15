#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "const_in_hash_table.h"
#include "list.h"
#include "func_hash.h"
#include "hash_table.h"

static errors_in_hash_table_t ctor_lists_in_hash_table (list_t* hash_table);
static errors_in_hash_table_t dtor_lists_in_hash_table (list_t* hash_table, size_t max_index_delete_list);
static errors_in_hash_table_t fill_hash_table          (list_t* hash_table, FILE* file_words);

//----------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t create_hash_table (/*int argc, */ char** argv)
{
	assert (argv);

	errors_in_hash_table_t status = NOT_ERROR;

	list_t hash_table[COUNT_BUCKETS] = {};

	status = ctor_lists_in_hash_table (hash_table);
	if (status)	{return status;}

	FILE* file_words = fopen (argv[INDEX_FILE_WORDS_IN_ARGV], "r");
	if (file_words == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_words\n"
				"Error in fclose(); or you don't write name of file with words: "
				"./hash_table <file>\n\n", __FILE__, __LINE__);
			
		dtor_lists_in_hash_table (hash_table, COUNT_BUCKETS - 1);

		return CANNOT_OPEN_FILE;
	}
	
	//--------------------------------------------------------------------------------------------------------------------

	status = fill_hash_table (hash_table, file_words);
	if (status)
	{
		printf ("Error in %s:%d\n"
				"Failed fill_hash_table\n\n", __FILE__, __LINE__);

		if (fclose (file_words) == EOF)
			printf ("Error in %s:%d\n"
					"Cannot close file_words\n\n", __FILE__, __LINE__);

		dtor_lists_in_hash_table (hash_table, COUNT_BUCKETS - 1);

		return FAILED_HASH_TABLE;
	}

	//--------------------------------------------------------------------------------------------------------------------

	dtor_lists_in_hash_table (hash_table, COUNT_BUCKETS - 1);

	if (fclose (file_words) == EOF)
	{
		printf ("Error in %s:%d\n"
				"Cannot close file_words\n\n", __FILE__, __LINE__);

		return CANNOT_CLOSE_FILE;
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t ctor_lists_in_hash_table (list_t* hash_table)
{
	assert (hash_table);

	errors_in_hash_table_t status = NOT_ERROR;

	for (size_t index_bucket = 0; index_bucket < COUNT_BUCKETS; index_bucket++)
	{
		status = ctor_list (hash_table + index_bucket);
		if (status)
		{
			printf ("Error in %s:%d\n"
					"Failed ctor_list of list by index == %ld\n\n", __FILE__, __LINE__, index_bucket);

			dtor_lists_in_hash_table (hash_table, index_bucket - 1);

			return FAILED_CTOR_LIST;
		}
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t dtor_lists_in_hash_table (list_t* hash_table, size_t max_index_delete_list)
{
	assert (hash_table);

	for (size_t index_bucket = 0; index_bucket <= max_index_delete_list; index_bucket++)
	{
		dtor_list (hash_table + index_bucket);
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_hash_table (list_t* hash_table, FILE* file_words)
{
	assert (hash_table);
	assert (file_words);

	errors_in_hash_table_t status = NOT_ERROR;

	char word[MAX_LEN_WORD] = "";

	// if (setvbuf (file_words, word, _IOLBF, MAX_LEN_WORD))   //return 0 ==> correct work
	// {
	// 	printf ("Error in %s:%d\n"
	// 			"Failed setvbuf: retrun value != 0\n\n", __FILE__, __LINE__);

	// 	return FAILED_SETVBUF;
	// }

	while (! feof (file_words))      //while (not end of file)
	{
		fgets (word, MAX_LEN_WORD, file_words);

		size_t index_bucket = hash_sum_ascii (word);

		status = add_element_in_list (hash_table + index_bucket, word);
		if (status) {return status;}
	}

	status = print_inf_about_func_hash (hash_table, "hash\\_sum\\_ascii", "./inf_about_hash_funcs/hash_sum_ascii.tex");	
	return status;
}
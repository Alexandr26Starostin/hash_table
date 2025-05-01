#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "const_in_hash_table.h"
#include "list.h"
#include "func_hash.h"
#include "hash_table.h"

static errors_in_hash_table_t ctor_lists_in_hash_table (list_t* hash_table);
static errors_in_hash_table_t dtor_lists_in_hash_table (list_t* hash_table, size_t max_index_delete_list);
static errors_in_hash_table_t fill_hash_table          (list_t* hash_table, FILE* file_words, int argc, char** argv);
static errors_in_hash_table_t find_name_func_hash      (int argc, char** argv, size_t (**ptr_func_hash) (char*));

#ifdef PRINT_INF_ABOUT_HASH_FUNC
	static errors_in_hash_table_t get_inf_func_hash (int argc, char** argv, list_t* hash_table);
#endif

//----------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t create_hash_table (int argc, char** argv)
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

	status = fill_hash_table (hash_table, file_words, argc, argv);
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

static errors_in_hash_table_t fill_hash_table (list_t* hash_table, FILE* file_words, int argc, char** argv)
{
	assert (hash_table);
	assert (file_words);
	assert (argv);

	errors_in_hash_table_t status = NOT_ERROR;

	char word[MAX_LEN_WORD] = "";

	size_t (*func_hash) (char*) = NULL;

	status = find_name_func_hash (argc, argv, &func_hash);
	if (status) {return status;}
	
	//-------------------------------------------------------------------------------------

	while (! feof (file_words))      //while (not end of file)
	{
		fgets (word, MAX_LEN_WORD, file_words);

		size_t index_bucket = func_hash (word);

		status = add_element_in_list (hash_table + index_bucket, word);
		if (status)
		{
			printf ("Error in %s:%d\n"
					"Failed ctor_list of list by index == %ld\n\n", __FILE__, __LINE__, index_bucket);

			return status;
		}
	}

	#ifdef PRINT_INF_ABOUT_HASH_FUNC
		status = get_inf_func_hash (argc, argv, hash_table);
		if (status) {return status;}
	#endif

	return status;
}

static errors_in_hash_table_t find_name_func_hash (int argc, char** argv, size_t (**ptr_func_hash) (char*))
{
	assert (argv);
	assert (ptr_func_hash);

	find_flag_t status_of_flag = NOT_FIND_FLAG;

	int index_argc = MIN_VALUE_INDEX_ARGC;
	for (;index_argc < argc; index_argc++)
	{
		if (! strcmp (argv[index_argc], "-hash"))
		{
			status_of_flag = FIND_FLAG;

			index_argc++;
			break;
		}
	}

	if (! status_of_flag)
	{
		printf ("Error in %s:%d\n"
				"Not find flag: -hash <name_func_hash>\n\n", __FILE__, __LINE__);

		return NOT_FIND_FLAG_HASH;
	}

	char* name_func_hash = argv[index_argc];

	if (! strcmp (name_func_hash, "hash_djb2"))
		*ptr_func_hash = hash_djb2;

	else if (! strcmp (name_func_hash, "hash_sum_of_squares"))
		*ptr_func_hash = hash_sum_of_squares;

	else if (! strcmp (name_func_hash, "hash_sum_ascii"))     
		*ptr_func_hash = hash_sum_ascii;

	else if (! strcmp (name_func_hash, "hash_ascii_first_symbol"))     
		*ptr_func_hash = hash_ascii_first_symbol;

	else if (! strcmp (name_func_hash, "hash_mul_all_ascii"))    
		*ptr_func_hash = hash_mul_all_ascii;

	else if (! strcmp (name_func_hash, "hash_average"))     
		*ptr_func_hash = hash_average;

	else if (! strcmp (name_func_hash, "hash_crc32"))     
		*ptr_func_hash = hash_crc32;

	else
	{
		printf ("Error in %s:%d\n"
				"Not defined func_hash with name: %s\n\n", __FILE__, __LINE__, name_func_hash);

		return NOT_DEFINED_NAME_FUNC_HASH;
	}

	return NOT_ERROR;
}

//------------------------------------------------------------------------------------------------------------------------------
#ifdef PRINT_INF_ABOUT_HASH_FUNC

static errors_in_hash_table_t get_inf_func_hash (int argc, char** argv, list_t* hash_table)
{
	assert (argv);
	assert (hash_table);

	find_flag_t status_of_flag = NOT_FIND_FLAG;

	int index_argc = MIN_VALUE_INDEX_ARGC;
	for (;index_argc < argc; index_argc++)
	{
		if (! strcmp (argv[index_argc], "-INF_FUNC_HASH"))
		{
			status_of_flag = FIND_FLAG;

			index_argc++;
			break;
		}
	}

	if (! status_of_flag)
	{
		printf ("Error in %s:%d\n"
				"Not find flag: -INF_FUNC_HASH <name_func_hash_for_LaTex> <file_for_inf.tex>\n\n", __FILE__, __LINE__);

		return NOT_FIND_FLAG_INF_FUNC_HASH;
	}

	errors_in_hash_table_t status = print_inf_about_func_hash (hash_table, argv[index_argc], argv[index_argc + 1]);
	
	return status;
}

#endif
//------------------------------------------------------------------------------------------------------------------------------

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>
//#include <immintrin.h>

#include "const_in_hash_table.h"
#include "list.h"
#include "func_hash.h"
#include "hash_table.h"

#define CLOSE_FILE_(file, name_file)                  					\
	if (fclose (file) == EOF)       								   	\
	{                                                   				\
		printf ("Error in %s:%d\n"										\
				"Cannot close %s\n\n", __FILE__, __LINE__, name_file);	\
																		\
		return CANNOT_CLOSE_FILE;										\
	}	

#define FIND_FLAG_(name_flag, error_message, code_error)				\
	find_flag_t status_of_flag = NOT_FIND_FLAG;							\
																		\
	int index_argc = MIN_VALUE_INDEX_ARGC;								\
	for (;index_argc < argc; index_argc++)								\
	{																	\
		if (! strcmp (argv[index_argc], name_flag))						\
		{																\
			status_of_flag = FIND_FLAG;									\
																		\
			index_argc++;												\
			break;														\
		}																\
	}																	\
																		\
	if (! status_of_flag)												\
	{																	\
		printf ("Error in %s:%d\n%s\n\n",								\
				__FILE__, __LINE__, error_message);						\
																		\
		return code_error;										     	\
	}


static errors_in_hash_table_t ctor_lists_in_hash_table (list_t* hash_table);
static errors_in_hash_table_t get_word_from_file       (FILE* file_words, char* word);
static errors_in_hash_table_t fill_hash_table          (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table, FILE* file_words);
static errors_in_hash_table_t find_name_func_hash      (int argc, char** argv, size_t (**ptr_func_hash) (char*));
static size_t                 len_file                 (FILE* file);  
static errors_in_hash_table_t get_symbols_for_search   (int argc, char** argv, char** ptr_symbols_from_file);
static errors_in_hash_table_t get_words_for_search 	   (inf_for_search_t* ptr_inf_for_search);
static errors_in_hash_table_t fill_words_for_search    (inf_for_search_t* ptr_inf_for_search);
       errors_in_hash_table_t print_words_for_search   (char** words_for_search, size_t count_words);
static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search);

#ifdef PRINT_INF_ABOUT_HASH_FUNC
	static errors_in_hash_table_t get_inf_func_hash (int argc, char** argv, list_t* hash_table);
#endif

//----------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t create_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table)
{
	assert (ptr_inf_hash_table);

	errors_in_hash_table_t status = NOT_ERROR;

	list_t* hash_table = ptr_inf_hash_table -> hash_table;

	status = ctor_lists_in_hash_table (hash_table);
	if (status)	{return status;}

	FILE* file_words = fopen (argv[INDEX_FILE_WORDS_IN_ARGV], "r");
	if (file_words == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_words\n"
				"Error in fopen(); or you don't write name of file with words: "
				"./hash_table <file>\n\n", __FILE__, __LINE__);
			
		delete_hash_table (hash_table);

		return CANNOT_OPEN_FILE;
	}
	
	//--------------------------------------------------------------------------------------------------------------------

	status = fill_hash_table (argc, argv, ptr_inf_hash_table, file_words);
	if (status)
	{
		printf ("Error in %s:%d\n"
				"Failed fill_hash_table\n\n", __FILE__, __LINE__);

		CLOSE_FILE_(file_words, "file_words");

		delete_hash_table (hash_table);

		return FAILED_HASH_TABLE;
	}

	CLOSE_FILE_(file_words, "file_words");

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

			delete_hash_table (hash_table);

			return FAILED_CTOR_LIST;
		}
	}

	return NOT_ERROR;
}

errors_in_hash_table_t delete_hash_table (list_t* hash_table)
{
	assert (hash_table);

	for (size_t index_bucket = 0; index_bucket < COUNT_BUCKETS; index_bucket++)
	{
		dtor_list (hash_table + index_bucket);
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table, FILE* file_words)
{
	assert (ptr_inf_hash_table);
	assert (ptr_inf_hash_table -> hash_table);
	assert (file_words);
	assert (argv);
	//ptr_inf_hash_table -> func_hash == NULL

	errors_in_hash_table_t status = NOT_ERROR;

	char word[MAX_LEN_WORD] = "";

	size_t (*func_hash) (char*) = NULL;
	list_t* hash_table          = ptr_inf_hash_table -> hash_table;

	status = find_name_func_hash (argc, argv, &func_hash);
	if (status) {return status;}
	
	//-------------------------------------------------------------------------------------

	while (! feof (file_words))      //while (not end of file)
	{
		get_word_from_file (file_words, word);   //return NOT_ERROR;
	
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

	ptr_inf_hash_table -> func_hash = func_hash;

	return status;
}

static errors_in_hash_table_t get_word_from_file (FILE* file_words, char* word)
{
	assert (file_words);
	assert (word);

	fgets (word, MAX_LEN_WORD, file_words);

	char   symbol = '\0';
	size_t index  = 0;

	for (; index < MAX_LEN_WORD; index++)
	{
		symbol = word[index];

		if (symbol == '\n')
		{
			word[index] = '\0';
			return NOT_ERROR;
		}

		if (symbol == '\0')
		{
			return NOT_ERROR;
		}
	}

	word[index - 1] = '\0';

	return NOT_ERROR;
}

static errors_in_hash_table_t find_name_func_hash (int argc, char** argv, size_t (**ptr_func_hash) (char*))
{
	assert (argv);
	assert (ptr_func_hash);

	FIND_FLAG_("-hash", "Not find flag: -hash <name_func_hash>", NOT_FIND_FLAG_HASH)

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

	FIND_FLAG_("-INF_FUNC_HASH", "Not find flag: -INF_FUNC_HASH <name_func_hash_for_LaTex> <file_for_inf.tex>", 
			   NOT_FIND_FLAG_INF_FUNC_HASH)

	errors_in_hash_table_t status = print_inf_about_func_hash (hash_table, argv[index_argc], argv[index_argc + 1]);
	
	return status;
}

#endif
//------------------------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t use_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table)
{
	assert (argv);
	assert (ptr_inf_hash_table);
	assert (ptr_inf_hash_table -> hash_table);
	assert (ptr_inf_hash_table -> func_hash);

	inf_for_search_t inf_for_search = {};

	errors_in_hash_table_t status = get_symbols_for_search (argc, argv, &(inf_for_search.symbols_from_file));
	if (status) {return status;}

	status = get_words_for_search (&inf_for_search);
	if (status)
	{
		free (inf_for_search.symbols_from_file);   
		return status;
	}

	//print_words_for_search (inf_for_search.words_for_search, inf_for_search.count_words);

	find_words_in_hash_table (*ptr_inf_hash_table, inf_for_search);   //return NOT_ERROR;

	free (inf_for_search.words_for_search);
	free (inf_for_search.symbols_from_file);

	return NOT_ERROR;
}

static errors_in_hash_table_t get_symbols_for_search (int argc, char** argv, char** ptr_symbols_from_file)
{
	assert (argv);
	assert (ptr_symbols_from_file);

	FIND_FLAG_("-find", "Not find flag: -find <file_with_words_for_search>", NOT_FIND_FLAG_FIND)

	FILE* file_search = fopen (argv[index_argc], "r");
	if (file_search == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_search\n"
				"Error in fopen(); or you don't write name of file with words for search:"
				"-find <file_with_words_for_search>\n\n", __FILE__, __LINE__);

		return CANNOT_OPEN_FILE;
	}

	size_t count_bytes = len_file (file_search);

	char* symbols_from_file = (char*) calloc (count_bytes + COUNT_ADDITIONAL_ELEMENT , sizeof (char));	
	if (symbols_from_file == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for symbols_from_file from calloc\n\n", __FILE__, __LINE__);

		CLOSE_FILE_(file_search, "file_search")

		return NOT_MEMORY_FROM_CALLOC;
	}

	size_t count_read_symbols = fread (symbols_from_file, sizeof (char), count_bytes, file_search);
	if (count_read_symbols != count_bytes)
	{
		printf ("Error in %s:%d\n"
				"fread: count_read_symbols != count_bytes (count_all_symbols)\n\n", __FILE__, __LINE__);

		free (symbols_from_file);

		CLOSE_FILE_(file_search, "file_search")

		return ERROR_IN_FREAD;
	}

	CLOSE_FILE_(file_search, "file_search")

	*ptr_symbols_from_file = symbols_from_file;

	return NOT_ERROR;
}

static size_t len_file (FILE* file)     
{  
    assert (file);

    fseek (file, 0, SEEK_END);
    const size_t count_memory = ftell (file);
    fseek (file, 0, SEEK_SET);

    return count_memory;
}

static errors_in_hash_table_t get_words_for_search (inf_for_search_t* ptr_inf_for_search)
{
	assert (ptr_inf_for_search);

	char** words_for_search = (char**) calloc (MIN_SIZE_WORDS_FOR_SEARCH, sizeof (char*));
	if (words_for_search == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for words_for_search from calloc\n\n", __FILE__, __LINE__);

		return NOT_MEMORY_FROM_CALLOC;
	}

	size_t count_words = 0;

	ptr_inf_for_search -> words_for_search = words_for_search;
	ptr_inf_for_search -> count_words      = count_words;

	errors_in_hash_table_t status = fill_words_for_search (ptr_inf_for_search);
	if (status) 
	{
		//free (words_for_search);   CANNOT: words_for_search == NULL
		return status;
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_words_for_search (inf_for_search_t* ptr_inf_for_search)
{
	assert (ptr_inf_for_search);

	char* symbols_from_file = ptr_inf_for_search -> symbols_from_file;
	assert (symbols_from_file);

	char** words_for_search = ptr_inf_for_search -> words_for_search;
	assert (words_for_search);

	size_t size_words_for_search = MIN_SIZE_WORDS_FOR_SEARCH;
	size_t index_word 			 = 0;
	size_t index_symbol          = 0;
	char   symbol                = '\0';

	words_for_search[index_word++] = symbols_from_file;

	for (; (symbol = symbols_from_file[index_symbol]) != '\0'; index_symbol++)
	{
		if (symbol != '\n') {continue;}

		//symbol == '\n'

		symbols_from_file[index_symbol] = '\0';

		words_for_search[index_word++] = symbols_from_file + index_symbol + 1;

		if (index_word != size_words_for_search) {continue;}

		//index_word == size_words_for_search

		size_words_for_search *= INCREASE_SIZE_IN_REALLOC;

		words_for_search = (char**) realloc (words_for_search, size_words_for_search * sizeof (char*));
		
		if (words_for_search == NULL)
		{
			printf ("Error in %s:%d\n"
					"Not have memory for realloc of words_for_search\n"
					"index_word == %ld\n\n", __FILE__, __LINE__, index_word);

			return ERROR_IN_REALLOC;
		}		
	}

	ptr_inf_for_search -> words_for_search = words_for_search;
	ptr_inf_for_search -> count_words      = index_word;

	return NOT_ERROR;
}

errors_in_hash_table_t print_words_for_search (char** words_for_search, size_t count_words)
{
	assert (words_for_search);

	printf ("------------------------------------------------------------------------\nwords_for search:\n\n");

	for (size_t index_word; index_word < count_words; index_word++)
	{
		printf ("%s\n", words_for_search[index_word]);
	}

	printf ("------------------------------------------------------------------------\n\n");

	return NOT_ERROR;
}

static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
{
	assert (inf_hash_table.hash_table);
	assert (inf_hash_table.func_hash);
	assert (inf_for_search.words_for_search);

	size_t  count_words      = inf_for_search.count_words;
	size_t  index_bucket     = 0;
	char*   word             = NULL;
	char**  words_for_search = inf_for_search.words_for_search;
	list_t* hash_table       = inf_hash_table.hash_table;
	
	size_t (*func_hash) (char*) = inf_hash_table.func_hash;

	//--------------------------------------------------------------------------

	size_t max_index_word = count_words;

	#ifdef TEST_PROGRAM
	for (size_t iterations = 1; iterations <= count_words; iterations++)
	{
		max_index_word = iterations;

		size_t ticks_before_cycle = __rdtsc();
	#endif

		for (size_t index_word = 0; index_word < max_index_word; index_word++)
		{
			word = words_for_search[index_word];

			index_bucket = func_hash (word);

			#ifndef TEST_PROGRAM
			printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n", word, index_bucket);
			size_t counter_word = find_element_in_list (hash_table + index_bucket, word);
			printf ("counter_word == %ld\n-----------------------------------------\n\n", counter_word);
			#endif

			#ifdef TEST_PROGRAM
			find_element_in_list (hash_table + index_bucket, word);
			#endif
		}

	#ifdef TEST_PROGRAM
		size_t ticks_after_cycle = __rdtsc();

		printf ("iteration == %4ld\t\tticks == %ld\n", iterations, ticks_after_cycle - ticks_before_cycle);
	}
	#endif

	return NOT_ERROR;
}
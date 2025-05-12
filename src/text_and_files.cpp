#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "const_in_hash_table.h"
#include "func_hash.h"
#include "text_and_files.h"

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

static errors_in_hash_table_t fill_words_for_search (inf_for_search_t* ptr_inf_for_search, char** words_for_search, FILE* file_search);

//---------------------------------------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t get_word_from_file (FILE* file_words, char* word)
{
	assert (file_words);
	assert (word);

	char word_for_filling[MAX_BYTES_IN_WORD] = "\0";

	fgets (word_for_filling, MAX_LEN_WORD, file_words);

	char   symbol = '\0';
	size_t index  = 0;

	for (; index < MAX_LEN_WORD; index++)
	{
		symbol = word_for_filling[index];

		if (symbol == '\n')
		{
			word_for_filling[index] = '\0';
			break;;
		}

		if (symbol == '\0')
		{
			break;
		}
	}

	//word[index] = '\0';    //word[index - 1] = '\0';

	// printf ("%s\n", word_for_filling);

	// getchar ();

	

	if (strncpy(word, word_for_filling, MAX_LEN_WORD) == NULL)
	{
		printf ("Error in %s:%d\n"
				"Failed strncpy();\n\n", __FILE__, __LINE__);
	}

	return NOT_ERROR;
}

errors_in_hash_table_t get_words_for_search (int argc, char** argv, inf_for_search_t* ptr_inf_for_search)
{
	assert (argv);
	assert (ptr_inf_for_search);

	char** words_for_search = (char**) calloc (MIN_SIZE_WORDS_FOR_SEARCH, sizeof (char*));
	if (words_for_search == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for words_for_search from calloc\n\n", __FILE__, __LINE__);

		return NOT_MEMORY_FROM_CALLOC;
	}

	ptr_inf_for_search -> words_for_search = words_for_search;
	
	//-------------------------------------------------------------------------------------------------------------------

	FIND_FLAG_("-find", "Not find flag: -find <file_with_words_for_search>", NOT_FIND_FLAG_FIND)

	FILE* file_search = fopen (argv[index_argc], "r");
	if (file_search == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_search\n"
				"Error in fopen(); or you don't write name of file with words for search:"
				"-find <file_with_words_for_search>\n\n", __FILE__, __LINE__);

		free (words_for_search);

		return CANNOT_OPEN_FILE;
	}

	//-------------------------------------------------------------------------------------------------------------------

	errors_in_hash_table_t status = fill_words_for_search (ptr_inf_for_search, words_for_search, file_search);
	if (status) 
	{
		CLOSE_FILE_(file_search, "file_search")
		//free (words_for_search);   CANNOT: words_for_search == NULL
		return status;
	}

	CLOSE_FILE_(file_search, "file_search")

	return NOT_ERROR;
}

errors_in_hash_table_t initialize_aligned_alloc (char* array)
{
	assert (array);

	for (size_t index = 0; index < MAX_BYTES_IN_WORD; index++)
	{
		array[index] = '\0';
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_words_for_search (inf_for_search_t* ptr_inf_for_search, char** words_for_search, FILE* file_search)
{
	assert (ptr_inf_for_search);
	assert (words_for_search);
	assert (file_search);

	size_t size_words_for_search = MIN_SIZE_WORDS_FOR_SEARCH;
	size_t count_word 			 = 0;

	while (! feof (file_search))      //while (not end of file)
	{
		words_for_search[count_word] = (char*) aligned_alloc (ALIGNMENT, MAX_BYTES_IN_WORD * sizeof (char));
		
		initialize_aligned_alloc (words_for_search[count_word]);

		get_word_from_file (file_search, words_for_search[count_word]);   //return NOT_ERROR;

		// printf ("%s\n", words_for_search[count_word]);
		// getchar ();

		count_word++;

		if (count_word != size_words_for_search) {continue;}

		//index_word == size_words_for_search

		size_words_for_search *= INCREASE_SIZE_IN_REALLOC;

		words_for_search = (char**) realloc (words_for_search, size_words_for_search * sizeof (char*));
		
		if (words_for_search == NULL)
		{
			printf ("Error in %s:%d\n"
					"Not have memory for realloc of words_for_search\n"
					"index_word == %ld\n\n", __FILE__, __LINE__, count_word);

			return ERROR_IN_REALLOC;
		}		
	}

	ptr_inf_for_search -> words_for_search = words_for_search;
	ptr_inf_for_search -> count_words      = count_word;

	return NOT_ERROR;
}

errors_in_hash_table_t delete_words_for_search (inf_for_search_t inf_for_search)
{
	char** words_for_search = inf_for_search.words_for_search;
	assert (words_for_search);	

	size_t count_words = inf_for_search.count_words;

	for (size_t index = 0; index < count_words; index++)
	{
		free (words_for_search[index]);
	}

	free (words_for_search);

	return NOT_ERROR;
}

errors_in_hash_table_t find_name_func_hash (int argc, char** argv, size_t (**ptr_func_hash) (char*))
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

	else if (! strcmp (name_func_hash, "hash_crc32_asm"))     
	{	
		//printf ("hash_crc32_asm == %p\n\n", hash_crc32_asm);
		*ptr_func_hash = hash_crc32_asm;
		//printf ("func_hash == %p\n\n", *ptr_func_hash);
	}
	else
	{
		printf ("Error in %s:%d\n"
				"Not defined func_hash with name: %s\n\n", __FILE__, __LINE__, name_func_hash);

		return NOT_DEFINED_NAME_FUNC_HASH;
	}

	return NOT_ERROR;
}

errors_in_hash_table_t print_str_32_bytes (char* str)
{
	assert (str);

	printf ("%s\n", str);

	for (size_t index = 0; index < MAX_BYTES_IN_WORD; index++)
	{
		printf ("%2X", str[index]);
	}
	printf ("\n\n");

	return NOT_ERROR;
}

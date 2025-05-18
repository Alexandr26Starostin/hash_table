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

errors_in_hash_table_t print_text (char* text, size_t count_words);

//---------------------------------------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t get_text_from_file (FILE* file_words, char** ptr_text, size_t* ptr_count_words)
{
	assert (file_words);
	assert (ptr_text);
	assert (ptr_count_words);

	size_t count_words = 0;

	fscanf (file_words, "%ld\n", &count_words);

	size_t count_bytes_for_text = count_words * MAX_BYTES_IN_WORD;

	#ifndef INTRINSICS
	char* text = (char*) calloc (count_bytes_for_text, sizeof (char));
	#else
	char* text = (char*) aligned_alloc (ALIGNMENT, count_bytes_for_text * sizeof (char));
	#endif

	if (text == NULL)
	{
		printf ("Error in %s:%d\n"
				"Not memory for text from calloc\n\n", __FILE__, __LINE__);

		return NOT_MEMORY_FROM_CALLOC;
	}

	#ifdef INTRINSICS
	memset (text, 0, count_bytes_for_text * sizeof (char));
	#endif

	char word_from_file[MAX_BYTES_IN_WORD] = "\0";

	char   symbol = '\0';

	for (size_t index_word = 0; index_word < count_bytes_for_text; index_word += MAX_BYTES_IN_WORD)
	{
		fgets (word_from_file, MAX_LEN_WORD, file_words);

		size_t index_symbol = 0;

		for (; index_symbol < MAX_LEN_WORD; index_symbol++)
		{
			symbol = word_from_file[index_symbol];

			if (symbol == '\n')
			{
				word_from_file[index_symbol] = '\0';
				break;
			}

			if (symbol == '\0')
			{
				break;
			}
		}

		if (strncpy(text + index_word, word_from_file, MAX_LEN_WORD) == NULL)
		{
			printf ("Error in %s:%d\n"
					"Failed strncpy();\n\n", __FILE__, __LINE__);
		}

	}

	#ifdef PRINT_TEXT_FROM_FILE 
	print_text (text, count_words);
	// getchar ();
	#endif

	*ptr_text        = text;
	*ptr_count_words = count_words;

	return NOT_ERROR;
}

errors_in_hash_table_t print_text (char* text, size_t count_words)
{
	assert (text);

	size_t count_bytes = count_words * MAX_BYTES_IN_WORD;

	printf ("------------------------------------------------------------------------\ntext text from file:\n\n");

	for (size_t index_word = 0; index_word < count_bytes; index_word += MAX_BYTES_IN_WORD)
	{
		print_str_32_bytes (text + index_word);
	}

	printf ("\n------------------------------------------------------------------------\n\n");

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
	{
		extern size_t TABLE_FOR_CRC32[CRC32_TABLE_SIZE];
		hash_crc32_gen_table (TABLE_FOR_CRC32);   
		
		*ptr_func_hash = hash_crc32;
	}

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

errors_in_hash_table_t print_words_for_search (char* words_for_search, size_t count_words)
{
	assert (words_for_search);

	size_t count_bytes = count_words * MAX_BYTES_IN_WORD;

	printf ("------------------------------------------------------------------------\nwords_for search:\n\n");

	for (size_t index_word = 0; index_word < count_bytes; index_word += MAX_BYTES_IN_WORD)
	{
		printf ("%s\n", words_for_search + index_word);
	}

	printf ("------------------------------------------------------------------------\n\n");

	return NOT_ERROR;
}

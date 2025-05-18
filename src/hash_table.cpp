#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include "const_in_hash_table.h"
#include "list.h"
#include "func_hash.h"
#include "text_and_files.h"
#include "cpe.h"
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
static errors_in_hash_table_t fill_hash_table          (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table, FILE* file_words);

#ifdef TEST_PROGRAM
	static errors_in_hash_table_t find_words_in_hash_table (int argc, char** argv, inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search);
#else
	static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search);
#endif

#ifdef PRINT_INF_ABOUT_HASH_FUNC
	static errors_in_hash_table_t get_inf_func_hash (int argc, char** argv, list_t* hash_table);
#endif

#ifndef INLINE_ASM
static search_in_cash_t search_element_in_cash (cash_t cash_with_words, char* word, element_in_cash_t* ptr_word_from_cash);
#endif

//----------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t create_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table)
{
	assert (ptr_inf_hash_table);

	errors_in_hash_table_t status = NOT_ERROR;

	list_t* hash_table = ptr_inf_hash_table -> hash_table;

	status = ctor_lists_in_hash_table (hash_table);
	if (status)	{delete_hash_table (ptr_inf_hash_table); return status;}

	FILE* file_words = fopen (argv[INDEX_FILE_WORDS_IN_ARGV], "r");
	if (file_words == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_words\n"
				"Error in fopen(); or you don't write name of file with words: "
				"./hash_table <file>\n\n", __FILE__, __LINE__);
			
		delete_hash_table (ptr_inf_hash_table);

		return CANNOT_OPEN_FILE;
	}
	
	//--------------------------------------------------------------------------------------------------------------------

	status = fill_hash_table (argc, argv, ptr_inf_hash_table, file_words);
	if (status)
	{
		printf ("Error in %s:%d\n"
				"Failed fill_hash_table\n\n", __FILE__, __LINE__);

		CLOSE_FILE_(file_words, "file_words");

		delete_hash_table (ptr_inf_hash_table);

		return status;
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

			return FAILED_CTOR_LIST;
		}
	}

	return NOT_ERROR;
}

errors_in_hash_table_t delete_hash_table (inf_hash_table_t* ptr_inf_hash_table)
{
	char*   text       = ptr_inf_hash_table -> text_for_fill;
	list_t* hash_table = ptr_inf_hash_table -> hash_table;

	assert (text);
	assert (hash_table);

	for (size_t index_bucket = 0; index_bucket < COUNT_BUCKETS; index_bucket++)
	{
		dtor_list (hash_table + index_bucket);
	}

	free (text);

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table, FILE* file_words)
{
	assert (ptr_inf_hash_table);
	assert (ptr_inf_hash_table -> hash_table);
	assert (file_words);
	assert (argv);
	//ptr_inf_hash_table -> func_hash == NULL
	//ptr_inf_hash_table ->text_for_fill == NULL

	errors_in_hash_table_t status = NOT_ERROR;

	size_t (*func_hash) (char*) = NULL;
	list_t* hash_table          = ptr_inf_hash_table -> hash_table;

	status = find_name_func_hash (argc, argv, &func_hash);
	if (status) {return status;}

	ptr_inf_hash_table -> func_hash     = func_hash;

	//-------------------------------------------------------------------------------------

	char*  text        = NULL;
	size_t count_words = 0;

	status = get_text_from_file (file_words, &text, &count_words);
	if (status) {return status;}

	ptr_inf_hash_table -> text_for_fill = text;

	//-------------------------------------------------------------------------------------

	size_t count_bytes = count_words * MAX_BYTES_IN_WORD;

	for (size_t index_word = 0; index_word < count_bytes; index_word += MAX_BYTES_IN_WORD)
	{
		char* ptr_on_word = text + index_word;

		size_t index_bucket = func_hash (ptr_on_word);

		status = add_element_in_list (hash_table + index_bucket, ptr_on_word);
		if (status)
		{
			printf ("Error in %s:%d\n"
					"Failed ctor_list of list by index == %ld\n\n", __FILE__, __LINE__, index_bucket);

			return status;
		}
	}

	// while (! feof (file_words))      //while (not end of file)
	// {
	// 	   //return NOT_ERROR;
	
	// 	//print_str_32_bytes (word);

	// 	

	// 	//printf ("%s\t%ld\n", word, index_bucket);

	// 	status = add_element_in_list (hash_table + index_bucket, word);
	// 	if (status)
	// 	{
	// 		printf ("Error in %s:%d\n"
	// 				"Failed ctor_list of list by index == %ld\n\n", __FILE__, __LINE__, index_bucket);

	// 		return status;
	// 	}
	// }

	//printf ("\n");

	#ifdef PRINT_INF_ABOUT_HASH_FUNC
	status = get_inf_func_hash (argc, argv, hash_table);
	if (status) {return status;}
	#endif

	return status;
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

	FIND_FLAG_("-find", "Not find flag: -find <file_with_words_for_search>", NOT_FIND_FLAG_FIND)

	FILE* file_search = fopen (argv[index_argc], "r");
	if (file_search ==NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_search\n"
				"Error in fopen(); or you don't write name of file with words for search:"
				"-find <file_with_words_for_search>\n\n", __FILE__, __LINE__);

		return CANNOT_OPEN_FILE;
	}

	char*  words_for_search       = NULL;
	size_t count_words_for_search = 0;

	errors_in_hash_table_t status = get_text_from_file (file_search, &words_for_search, &count_words_for_search);
	if (status) {return status;}

	inf_for_search.words_for_search = words_for_search;
	inf_for_search.count_words      = count_words_for_search;

	#ifdef PRINT_WORDS_FOR_SEARCH
	print_words_for_search (inf_for_search.words_for_search, inf_for_search.count_words);
	#endif

	#ifdef TEST_PROGRAM
	find_words_in_hash_table (argc, argv, *ptr_inf_hash_table, inf_for_search);   //return NOT_ERROR;
	#else
	find_words_in_hash_table (*ptr_inf_hash_table, inf_for_search);   //return NOT_ERROR;
	#endif

	free (words_for_search);
	CLOSE_FILE_(file_search, "file_search");

	return NOT_ERROR;
}

#ifdef TEST_PROGRAM
static errors_in_hash_table_t find_words_in_hash_table (int argc, char** argv, inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
#else
static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
#endif
{
	char*   words_for_search = inf_for_search.words_for_search;
	list_t* hash_table       = inf_hash_table.hash_table;
	
	size_t (*func_hash) (char*) = inf_hash_table.func_hash;

	assert (words_for_search);
	assert (hash_table);
	assert (func_hash);

	#ifdef TEST_PROGRAM
	assert (argv);
	#endif

	size_t  count_words      = inf_for_search.count_words;

	#ifndef INLINE_ASM
	size_t  index_bucket     = 0;
	char*   word             = NULL;
	
	cash_t 			   cash_with_words  = {};
	element_in_cash_t  word_from_cash   = {};
	#endif

	//--------------------------------------------------------------------------

	#ifdef INLINE_ASM
	size_t max_index_word = count_words - count_words % 2;
	#else
	size_t max_index_word = count_words * MAX_BYTES_IN_WORD;
	#endif


	#ifdef TEST_PROGRAM

	size_t index_cpe = 0;

	inf_test_t* tests = (inf_test_t*) calloc (count_words * COUNT_REPEATING, sizeof (inf_test_t));
	if (tests == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for ticks from calloc\n\n", __FILE__, __LINE__);

		return NOT_MEMORY_FROM_CALLOC;
	}

	inf_cpe_t cpe_result = {.max_iterations = count_words, 
							.tests          = tests};

	#endif

	#ifndef DEBUG
	for (size_t index_repeating = 0; index_repeating < COUNT_REPEATING; index_repeating++)
	{
	#endif

	#ifdef TEST_PROGRAM
		#ifdef INLINE_ASM
		size_t iterations = 2;
		#else
		size_t iterations = 1;
		#endif

		for (; iterations <= count_words; iterations++)
		{
			#ifdef INLINE_ASM
			max_index_word = iterations - iterations % 2;
			#else
			max_index_word = iterations * MAX_BYTES_IN_WORD;
			#endif

			size_t ticks_before_cycle = __rdtsc();
	#endif

			#ifndef INLINE_ASM

			for (size_t index_word = 0; index_word < max_index_word; index_word += MAX_BYTES_IN_WORD)
			{
				word = words_for_search + index_word;

				// printf ("--------------------------------------------------------\n");
				// print_str_32_bytes ((inf_hash_table.text_for_fill) + index_word);
				// printf ("bucket = %ld\n\n", func_hash ((inf_hash_table.text_for_fill) + index_word));

				// print_str_32_bytes (word);
				// printf ("--------------------------------------------------------\n");
				// printf ("bucket = %ld\n\n", func_hash (word));
				// getchar ();

				search_in_cash_t status_cash = search_element_in_cash (cash_with_words, word, &word_from_cash);
				if (status_cash)
				{
					#ifdef DEBUG
					printf ("-----------------------------------------\ninf about search word in cash: %s\n\nindex_bucket == %ld\n"
							"counter_word == %ld\n-----------------------------------------\n\n", 
							word, word_from_cash.index_bucket, word_from_cash.count_words_in_text);
					#endif

					//getchar ();

					continue;
				}

				index_bucket = func_hash (word);

				//printf ("%s\t%ld\n", word, index_bucket);

				#ifdef DEBUG
				printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n", word, index_bucket);				
				#endif

				size_t counter_word = find_element_in_list (hash_table + index_bucket, word);

				#ifdef DEBUG
				printf ("counter_word == %ld\n-----------------------------------------\n\n", counter_word);
				#endif

				//------------------------------------------------------------
				//add_element_in_cash

				size_t index_free = cash_with_words.index_free;

				element_in_cash_t* ptr_old_el_in_cash = cash_with_words.elements_in_cash + index_free;

				ptr_old_el_in_cash -> count_words_in_text = counter_word;
				ptr_old_el_in_cash -> index_bucket        = index_bucket;
				ptr_old_el_in_cash -> word 				  = word;

				cash_with_words.index_free = (index_free + 1) % SIZE_CASH_WITH_WORDS;
				//------------------------------------------------------------
			}

			#else

			char* word_1 = words_for_search;
			char* word_2 = words_for_search + MAX_BYTES_IN_WORD;

			// size_t step_in_cycle = MAX_BYTES_IN_WORD * 2;

			// for (size_t index_word = 0; index_word < max_index_word; index_word += 2)
			// {
			// 	size_t index_bucket_1 = hash_crc32_asm (word_1);
			// 	size_t index_bucket_2 = hash_crc32_asm (word_2);

			// 	find_element_in_list (hash_table + index_bucket_1, word_1);
			// 	find_element_in_list (hash_table + index_bucket_2, word_2);

			// 	word_1 += step_in_cycle;
			// 	word_2 += step_in_cycle;
			// }

			asm volatile (
				".intel_syntax noprefix\n\t"

				"push r8\n\t"
				"push r9\n\t"
				"push r10\n\t"
				"push r11\n\t"  //save others registers

				//--------------------------------------------------------------------
			
				"mov rcx, %[max_index]\n\t"   //rcx = max_index_word
				"mov r13, %[word_1]\n\t"      //r13 = word_1
				"mov r14, %[word_2]\n\t"      //r14 = word_2
				"mov r15, %[hash_table]\n\t"  //r15 = hash_table

				"find_next_two_elem:\n\t"

				//---------------------------------------------------------
				"push rcx\n\t"

					"mov rdi, r13\n\t"
					"call hash_crc32_asm\n\t"    //size_t index_bucket_1 = hash_crc32_asm (word_1);

					"mov rdx, 24\n\t"
					"mul rdx\n\t"
					"mov rdi, r15\n\t"
					"add rdi, rax\n\t"
					"mov rsi, r13\n\t"
					"call find_element_in_list\n\t"   //find_element_in_list (hash_table + index_bucket_1, word_1);

					//-------------------------------------------

					"mov rdi, r14\n\t"
					"call hash_crc32_asm\n\t"   //size_t index_bucket_2 = hash_crc32_asm (word_2);

					"mov rdx, 24\n\t"
					"mul rdx\n\t"
					"mov rdi, r15\n\t"
					"add rdi, rax\n\t"
					"mov rsi, r14\n\t"
					"call find_element_in_list\n\t"   //find_element_in_list (hash_table + index_bucket_2, word_2);
				
				"add r13, 64\n\t"     //word_1 += step_in_cycle;
				"add r14, 64\n\t"     //word_2 += step_in_cycle;

				"pop rcx\n\t"
				//---------------------------------------------------------

				"dec rcx\n\t"
				"loop find_next_two_elem\n\t"   //for (size_t index_word = max_index_word; index_word != 0; index_word -= 2) {}
				
				//--------------------------------------------------------------------

				"pop r11\n\t"
				"pop r10\n\t"
				"pop r9\n\t"
				"pop r8\n\t"   //save others registers
			
				//-----------------------------------------------------------------------------------------------------
				:
				:[max_index] "r" (max_index_word), 
				[word_1] "r" (word_1), 
				[word_2]  "r" (word_2),
				[hash_table] "r" (hash_table)
				:"rcx", "r13", "r14", "r15", "rax", "rdi", "rsi", "rdx", "cc", "memory"
				);

			#endif

	#ifdef TEST_PROGRAM
			size_t ticks_after_cycle = __rdtsc();

			size_t ticks = ticks_after_cycle - ticks_before_cycle;

			// printf ("iteration == %4ld\t\tticks == %ld\n", iterations, ticks);
			// getchar ();
			
			(tests[index_cpe]).ticks      = ticks;
			(tests[index_cpe]).iterations = iterations;
			index_cpe++;
		}
	#endif

	#ifndef DEBUG
	}
	#endif

	#ifdef TEST_PROGRAM

	#ifdef PRINT_CPE_RESULT
	print_cpe_result   (cpe_result);
	#endif

	#ifdef TEST_PROGRAM
	analyze_cpe_result (argc, argv, cpe_result);
	#endif

	free (tests);
	#endif
	
	return NOT_ERROR;
}

//---------------------------------------------------------------------------------------------------------------------------

#ifndef INLINE_ASM

static search_in_cash_t search_element_in_cash (cash_t cash_with_words, char* word, element_in_cash_t* ptr_word_from_cash)
{
	assert (word);
	assert (ptr_word_from_cash);

	element_in_cash_t* elements_in_cash = cash_with_words.elements_in_cash;

	for (size_t index_el = 0; index_el < SIZE_CASH_WITH_WORDS; index_el++)
	{
		element_in_cash_t element = elements_in_cash[index_el];

		char* word_el = element.word;
		if (word_el == NULL) {break;}

		if (compare_element (word, word_el))
		{
			*ptr_word_from_cash = element;

			return FIND_IN_CASH;
		}
	}

	return NOT_FIND_IN_CASH;
}

#endif



// search_in_cash_t status = NOT_FIND_IN_CASH;

// bool (*ptr_compare_element) (char*, char*) = compare_element;

// asm volatile (
// 		".intel_syntax noprefix\n\t"  	 		
		
// 		"mov rsi, %[word]\n\t"   //rsi = word 
// 		"mov r14, %[ptr]\n\t"    //r14 = ptr_word_from_cash

// 		"mov rbx, %[cash]\n\t"          //rbx = cash_with_words 
// 		"add rbx, 8\n\t"                //rbx = elements_in_cash = cash_with_words.elements_in_cash
// 		"xor ecx, ecx\n\t"              //rcx = index_el = 0
// 		"mov r12, 4\n\t"                //r12 = SIZE_CASH_WITH_WORDS = 4

// 		"check_next_cash:\n\t"

		
// 		"mov rax, rcx\n\t"
// 		"mov r15, 24\n\t"
// 		"mul r15\n\t"        //rdx changed
// 		"mov r13, [rbx + rax]\n\t"    //r13 = [rbx + rcx * 24]
// 									  //r13 = elements_in_cash[index_el];
// 									  //r13 = word_el = element.word;
		

// 		"test r13, r13\n\t"        
// 		"jz word_is_null\n\t"      //if (word_el == NULL) {break;}
// 		//--------------------------------------------------------------------------------------------------------
      
// 		"push rsi\n\t"
// 		"push rcx\n\t"     //save registers

// 		"mov rdi, r13\n\t"   //prepare arg
// 		//rsi not changed and rsi = word

// 		"call %[ptr_func]\n\t"   //al = compare_element (word, word_el);

// 		"pop rcx\n\t" 
// 		"pop rsi\n\t"    //save registers
		
// 		//--------------------------------------------------------------------------------------------------------

// 		"test al, al\n\t"
// 		"jz not_elem_from_cash\n\t"  //if (al == false) {continue;}

// 		//----------------------------------------------------------------------------------------------------------
  
// 		"mov rax, rcx\n\t"
// 		"mov r15, 24\n\t"
// 		"mul r15\n\t"     //rdx changed
// 		"add r13, rax\n\t"  //r13 = rbx + rcx * 24

// 		//-------------------------------------------------------------------------------------------------
// 		//*ptr_word_from_cash = element;

// 		"mov r15, [r13]\n\t"
// 		"mov [r14], r15\n\t"       //ptr_word_from_cash -> word = element.word

// 		"mov r15, [r13 + 8]\n\t"
// 		"mov [r14 + 8], r15\n\t"   //ptr_word_from_cash -> index_bucket = element.index_bucket

// 		"mov r15, [r13 + 16]\n\t"
// 		"mov [r14 + 16], r15\n\t"  //ptr_word_from_cash -> count_words_in_text = element.count_words_in_text

// 		"mov %0, 1\n\t"      //status = FIND_IN_CASH;
// 		"jmp end_asm\n\t"    //break;

// 		//-----------------------------------------------------------------------------------------------------------

// 		"not_elem_from_cash:\n\t"

// 		"inc rcx\n\t"         
// 		"cmp rcx, r12\n\t"
// 		"jnz check_next_cash\n\t"  //for (rcx = 0; rcx < r12; rcx++) {...}   //r12 = 4

// 		"word_is_null:\n\t"

// 		"mov %0, 0\n\t"  //status = NOT_FIND_IN_CASH;
// 		"end_asm:\n\t"

// 		//-----------------------------------------------------------------------------------------------------
// 		:"=r" (status)
// 		:[cash] "r" (&cash_with_words), 
// 		 [word] "r" (word), 
// 		 [ptr]  "r" (ptr_word_from_cash),
// 		 [ptr_func] "r" (ptr_compare_element)
// 		:"rbx", "rcx", "rdi", "rsi", "rdx", "r12", "r13", "r14", "r15", "rax", "memory", "cc"
// 		);

// return status;
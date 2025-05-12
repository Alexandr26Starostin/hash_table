#ifndef CONST_IN_HASH_TABLE_H
#define CONST_IN_HASH_TABLE_H

#include <immintrin.h>

#ifdef PRINT_ALL
	#define PRINT_INF_ABOUT_HASH_FUNC
	#define PRINT_WORDS_FOR_SEARCH
	#define PRINT_CPE_RESULT
	#define TEST_PROGRAM
#endif

#define INTRINSICS

#define INLINE_ASM

// #define PRINT_INF_ABOUT_HASH_FUNC

//#define TEST_PROGRAM

//#define DEBUG_CRC32_

//typedef long data_t; 
typedef char* data_t;

const __u_int POLYNOMIAL   		         = 0x04C11DB7;
const __u_int MASK_HIGH_BIT_IN_REGISTER  = 0x80000000;
const __u_int MASK_FIRST_BIT             = 0x00000001;
const char    MASK_HIGH_BIT_BYTE         = (char) 0x80;
const size_t  COUNT_SHIFTING_BITS        = 1;
const size_t  BITS_IN_BYTE               = 8;

const size_t SIZE_OF_DELETE_LIST       = 0;
const size_t MIN_SIZE_OF_LIST          = 1;
const size_t COUNT_NODES_IN_CALLOC     = 1;
const size_t MIN_COUNTER_ELEMENT       = 1;
const data_t INITIALIZATION_OF_DATA_T  = 0;
const size_t INDEX_FILE_WORDS_IN_ARGV  = 1;
const size_t MAX_LEN_WORD              = 31;
const size_t MAX_BYTES_IN_WORD         = 32;
const int    MIN_VALUE_INDEX_ARGC      = 2;
const size_t MIN_SIZE_WORDS_FOR_SEARCH = 64;
const size_t INCREASE_SIZE_IN_REALLOC  = 2;
const size_t NOT_FIND_ELEMENT          = 0;
const size_t COUNT_ADDITIONAL_ELEMENT  = 1;
const size_t MIN_COUNT_STRS            = 1;
const size_t COUNT_LONG_IN_M256I       = 4;
const int    MASK_IF_ELEM_EQUAL        = -1;
const size_t ALIGNMENT                 = 32;

const size_t COUNT_BUCKETS             = 1024;    //727
const size_t COUNT_REPEATING           = 15;
const size_t SIZE_CASH_WITH_WORDS      = 4;
//const char*  POISON                   = "\0";

enum errors_in_hash_table_t
{
	NOT_ERROR                         = 0,
	HAVE_NOT_MEMORY_FOR_FIRST_ELEMENT = 1,
	NOT_MEMORY_FOR_ELEMENT            = 2,
	TRY_TO_GET_VALUE_OUTSIDE_LIST     = 3,
	CANNOT_OPEN_FILE                  = 4,
	CANNOT_CLOSE_FILE                 = 5,
	FAILED_CTOR_LIST                  = 6,
	FAILED_HASH_TABLE                 = 7,
	FAILED_SETVBUF                    = 8,
	FAILED_STRNCPY                    = 9,
	NOT_FIND_FLAG_HASH                = 10,
	NOT_DEFINED_NAME_FUNC_HASH        = 11,
	NOT_FIND_FLAG_INF_FUNC_HASH       = 12,
	NOT_FIND_FLAG_FIND                = 13,
	NOT_MEMORY_FROM_CALLOC            = 14,
	ERROR_IN_FREAD                    = 15,
	ERROR_IN_REALLOC                  = 16,
	NOT_FIND_FLAG_PLOT                = 17
};

enum find_flag_t
{
	NOT_FIND_FLAG = 0,
	FIND_FLAG     = 1
};

enum search_in_cash_t
{
	NOT_FIND_IN_CASH = 0,
	FIND_IN_CASH     = 1
};

struct node_t
{
	node_t* next_element;
	size_t  counter_element;
	data_t  data;
	// long    nop_long_1;
	// long    nop_long_2;
	// long    nop_long_3;     //sizeof (data + 3 * nop_long) == 32;
};

struct list_t 
{
	size_t  size_of_list;
	node_t* head;
	node_t* tail; 	
};

struct inf_hash_table_t
{
	size_t (*func_hash) (char*);
	list_t hash_table[COUNT_BUCKETS];
};

struct inf_for_search_t
{
	//char*  symbols_from_file;
	char** words_for_search;
	size_t count_words;
};

#ifdef TEST_PROGRAM

struct inf_test_t 
{
	size_t iterations;
	size_t ticks;
};

struct inf_cpe_t
{
	size_t      max_iterations;
	inf_test_t* tests;
};
#endif

struct element_in_cash_t
{
	char*  word;
	size_t index_bucket;
	size_t count_words_in_text;
};

struct cash_t
{
	size_t index_free;
	element_in_cash_t elements_in_cash[SIZE_CASH_WITH_WORDS];
};

#endif
#ifndef CONST_IN_HASH_TABLE_H
#define CONST_IN_HASH_TABLE_H

#include <immintrin.h>
#include <limits.h>

//#define DEBUG
//#define TEST_PROGRAM
//#define INTRINSICS
//#define INLINE_ASM

//typedef long data_t; 
typedef char* data_t;

const __u_int POLYNOMIAL   		         = 0x04C11DB7;    //0x741B8CD7;  //0x1EDC6F41;     //0x04C11DB7;
const __u_int MASK_HIGH_BIT_IN_REGISTER  = 0x80000000;
const __u_int MASK_FIRST_BIT             = 0x00000001;
const char    MASK_HIGH_BIT_BYTE         = (char) 0x80;
const size_t  COUNT_SHIFTING_BITS        = 1;
const size_t  BITS_IN_BYTE               = 8;
const size_t BASE_NUMBER_DJB2            = 5381;
const size_t SHIFTING_IN_GFB2            = 5;

const size_t SIZE_OF_DELETE_LIST       = 0;
const size_t MIN_SIZE_OF_LIST          = 1;
const size_t COUNT_NODES_IN_CALLOC     = 1;
const size_t MIN_COUNTER_ELEMENT       = 0;
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
const size_t DIFFERENCE_COUNT_WORDS    = 1;

const size_t COUNT_BUCKETS             = 1024;    //727  1024   7313
const size_t COUNT_REPEATING           = 10;      //70000  10
const size_t SIZE_CASH_WITH_WORDS      = 4;

const size_t CRC32_POLYNOMIAL = 0xEDB88320;
const size_t CRC32_INIT_CRC   = 0xFFFFFFFF;
const size_t CRC32_TABLE_SIZE = UCHAR_MAX + 1;
const size_t MASK_FOR_2_BYTES = 0xFF;

enum errors_in_hash_table_t
{
	NOT_ERROR                         = 0,
	HAVE_NOT_MEMORY_FOR_FIRST_ELEMENT = 1,
	NOT_MEMORY_FOR_ELEMENT            = 2,
	TRY_TO_GET_VALUE_OUTSIDE_LIST     = 3,
	CANNOT_OPEN_FILE                  = 4,
	CANNOT_CLOSE_FILE                 = 5,
	FAILED_CTOR_LIST                  = 6,
	FAILED_STRNCPY                    = 7,
	NOT_FIND_FLAG_HASH                = 8,
	NOT_DEFINED_NAME_FUNC_HASH        = 9,
	NOT_FIND_FLAG_INF_FUNC_HASH       = 10,
	NOT_FIND_FLAG_FIND                = 11,
	NOT_MEMORY_FROM_CALLOC            = 12,
	ERROR_IN_FREAD                    = 13,
	ERROR_IN_REALLOC                  = 14,
	NOT_FIND_FLAG_PLOT                = 15
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
};

struct list_t   //size = 8 + 8 + 8 = 24
{
	size_t  size_of_list;
	node_t* head;
	node_t* tail; 	
};

struct inf_hash_table_t
{
	size_t (*func_hash) (char*);
	char*  text_for_fill;
	list_t hash_table[COUNT_BUCKETS];
};

struct inf_for_search_t
{
	char*  words_for_search;
	size_t count_words;
};

//--------------------------------------

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

//---------------------------

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
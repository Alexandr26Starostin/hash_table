#ifndef CONST_IN_HASH_TABLE_H
#define CONST_IN_HASH_TABLE_H

#define PRINT_INF_ABOUT_HASH_FUNC

//typedef long data_t; 
typedef char* data_t;

const size_t SIZE_OF_DELETE_LIST      = 0;
const size_t MIN_SIZE_OF_LIST         = 1;
const size_t COUNT_NODES_IN_CALLOC    = 1;
const size_t MIN_COUNTER_ELEMENT      = 1;
const data_t INITIALIZATION_OF_DATA_T = 0;
const size_t COUNT_BUCKETS            = 727;
const size_t INDEX_FILE_WORDS_IN_ARGV = 1;
const size_t MAX_LEN_WORD             = 32;
const int    MIN_VALUE_INDEX_ARGC     = 2;
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
	NOT_FIND_FLAG_INF_FUNC_HASH       = 12
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

enum find_flag_t
{
	NOT_FIND_FLAG = 0,
	FIND_FLAG     = 1
};

#endif
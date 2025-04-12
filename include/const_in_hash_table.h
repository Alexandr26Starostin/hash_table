#ifndef CONST_IN_HASH_TABLE_H
#define CONST_IN_HASH_TABLE_H

enum errors_in_hash_table_t
{
	NOT_ERROR                         = 0,
	HAVE_NOT_MEMORY_FOR_FIRST_ELEMENT = 1,
	NOT_MEMORY_FOR_ELEMENT            = 2,
	TRY_TO_GET_VALUE_OUTSIDE_LIST     = 3
};

typedef long data_t; 

struct node_t
{
	node_t* next_element;
	size_t  counter_element;
	data_t 	data;
};

struct list_t 
{
	size_t  size_of_list;
	node_t* head;
	node_t* tail; 	
};

const size_t SIZE_OF_DELETE_LIST      = 0;
const size_t MIN_SIZE_OF_LIST         = 1;
const size_t COUNT_NODES_IN_CALLOC    = 1;
const size_t MIN_COUNTER_ELEMENT      = 1;
const data_t POISON                   = -666;
const data_t INITIALIZATION_OF_DATA_T = 0;

#endif
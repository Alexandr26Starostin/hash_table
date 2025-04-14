#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

const int    MIN_VALUE_ARGV  = 1;
const char*  READ_FILE_FLAG  = "-r"; 
const char*  WRITE_FILE_FLAG = "-w";
const size_t MAX_LEN_WORD 	 = 64;
const int    MIN_SHIFTING    = 1;

enum errors_in_prepare_t
{
	NOT_ERROR                   = 0,
	CANNOT_OPEN_READ_FILE       = 1,
	CANNOT_OPEN_WRITE_FILE      = 2,
	CANNOT_CLOSE_READ_FILE      = 3,
	CANNOT_CLOSE_WRITE_FILE     = 4,
	HAVE_NOT_MEMORY_FROM_CALLOC = 5
};

static errors_in_prepare_t open_files        (int argc, char** argv, FILE** ptr_file_text, FILE** ptr_file_words);
static errors_in_prepare_t write_words       (FILE* file_text, FILE* file_words);
static size_t              len_file          (FILE* file);
static size_t 			   add_word_in_array (char* array, char* word);

//-----------------------------------------------------------------------------------------------------

int main (int argc, char** argv)
{
	FILE* file_text  = NULL;
	FILE* file_words = NULL;

	errors_in_prepare_t status = open_files (argc, argv, &file_text, &file_words);
	if (status) {return status;}

	status = write_words (file_text, file_words);
	//if (status) {return status;}

	if (fclose (file_text) == EOF)
	{
		printf ("Errors in %s:%d\n"
				"Cannot close file for reading\n\n", __FILE__, __LINE__);
		return CANNOT_CLOSE_READ_FILE;
	}

	if (fclose (file_words) == EOF)
	{
		printf ("Errors in %s:%d\n"
				"Cannot close file for writing\n\n", __FILE__, __LINE__);
		return CANNOT_CLOSE_WRITE_FILE;
	}
	
	return status;
}

//-----------------------------------------------------------------------------------------------------------------

static errors_in_prepare_t open_files (int argc, char** argv, FILE** ptr_file_text, FILE** ptr_file_words)
{
	assert (argv);
	assert (ptr_file_text);
	assert (ptr_file_words);

	FILE* file_text  = NULL;
	FILE* file_words = NULL;

	for (int index = MIN_VALUE_ARGV; index < argc; index++)
	{
		if (! strcmp (READ_FILE_FLAG, argv[index]))
		{
			file_text = fopen (argv[++index], "r");
			continue;
		}
		
		if (! strcmp (WRITE_FILE_FLAG, argv[index]))
			file_words = fopen (argv[++index], "w+");
		
	}

	if (file_text == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file for reading\n"
				"Problem is in: fopen (<name>, \"r\")\n"
				"or you use program wrong: you must write name of file for reading: -r <name_file>\n\n", __FILE__, __LINE__);

		return CANNOT_OPEN_READ_FILE;
	}

	if (file_words == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file for writing\n"
				"Problem is in: fopen (<name>, \"w\")\n"
				"or you use program wrong: you must write name of file for writing: -w <name_file>\n\n", __FILE__, __LINE__);

		fclose (file_text);

		return CANNOT_OPEN_WRITE_FILE;
	}

	*ptr_file_text  = file_text;
	*ptr_file_words = file_words;

	return NOT_ERROR;
}

static errors_in_prepare_t write_words (FILE* file_text, FILE* file_words)
{
	assert (file_text);
	assert (file_words);

	size_t len_file_text = len_file (file_text);

	char* text = (char*) calloc (len_file_text + 1, sizeof (char));
	if (text == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for text from calloc\n\n", __FILE__, __LINE__);

		return HAVE_NOT_MEMORY_FROM_CALLOC;
	}

	char* words = (char*) calloc (len_file_text + 1, sizeof (char));
	if (words == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for words from calloc\n\n", __FILE__, __LINE__);

		return HAVE_NOT_MEMORY_FROM_CALLOC;
	}
	
	fread (text, sizeof (char), len_file_text, file_text);

	char word_from_text[MAX_LEN_WORD] = "";

	size_t index_words = 0;
	size_t index_text  = 0;

	int shifting = MIN_SHIFTING;

	for (;index_text < len_file_text;)
	{
		shifting = MIN_SHIFTING;   //if find [^ ,;!?{}\":&()<>.\n]

		word_from_text[0] = '\0';

		sscanf (text + index_text, "%32[^ ,;!?{}\":&()<>.\n]%n", word_from_text, &shifting);

		word_from_text[shifting] = '\n';

		if (word_from_text[1] != '\n' || word_from_text[0] != '\0')	
			index_words += add_word_in_array (words + index_words, word_from_text);
			
		index_text += (size_t) shifting;  
	}

	fwrite (words, sizeof (char), index_words, file_words);

	free (text);
	free (words);

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

static size_t add_word_in_array (char* array, char* word)
{
	assert (array);
	assert (word);

	size_t index = 0;

	char symbol = '\0';

	while ((symbol = word[index]) != '\n')
		array[index++] = symbol;

	if (index != 0)
		array[index++] = '\n';
	
	return index;
}
#include <stdio.h>
#include <assert.h>

#include "const_in_hash_table.h"
#include "func_hash.h"

//--------------------------------------------------------------------------

size_t hash_sum_ascii (char* str)
{
	assert (str);

	size_t sum 	 = 0;
	
	while ((*str) != '\0')
	{
		sum += (size_t) str[0];
		str++;
	}

	return sum % COUNT_BUCKETS;
}

errors_in_hash_table_t print_inf_about_func_hash (list_t* hash_table, const char* str_name_func_hash, const char* name_inf_file)
{
	assert (hash_table);
	assert (str_name_func_hash);
	assert (name_inf_file);

	FILE* inf_file = fopen (name_inf_file, "w");
	if (inf_file == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file (%s)\n\n", __FILE__, __LINE__, name_inf_file);

		return CANNOT_OPEN_FILE;
	}

	size_t load_factor = 0;

	fprintf (inf_file, "\\documentclass{article}\n"
						"\\usepackage{graphicx}\n"
						"\\usepackage{pgfplots}\n"
						"\\pgfplotsset{compat=1.9}\n\n"
						"\\author{Александр Старостин}\n\n"
						"\\begin{document}\n\n"
						"\\begin{center}\n"
    					"\t\\Large\n"
    					"\tInformation about results func\\_hash (%s)\n"
						"\\end{center}\n\n\n"
						"\\begin{tikzpicture}\n"
						"\\begin{axis}\n"
						"[\n"
						"\twidth=15cm,\n"
   						"\txlabel = {index\\_bucket},\n"
						"\tylabel = {count\\_elements},\n"	
						"]\n"
						"\\addplot table {\n"
						"\t    x      y\n", str_name_func_hash);

	for (size_t index_bucket = 0; index_bucket < COUNT_BUCKETS; index_bucket++)
	{
		size_t size_bucket = (hash_table[index_bucket]).size_of_list;
		load_factor       += size_bucket;

		fprintf (inf_file, "\t%5ld %5ld\n", index_bucket, size_bucket);
	}

	fprintf (inf_file, "};\n"
					   "\\end{axis}\n"
					   "\\end{tikzpicture}\n\n"
					   "load\\_factor = %lg\n\n"
					   "\\end{document}\n", ((double) load_factor) / COUNT_BUCKETS);

	if (fclose (inf_file) == EOF)
	{
		printf ("Error in %s:%d\n"
				"Cannot close file (%s)\n\n", __FILE__, __LINE__, name_inf_file);

		return CANNOT_CLOSE_FILE;
	}

	return NOT_ERROR;
}
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <immintrin.h>

#include "const_in_hash_table.h"
#include "func_hash.h"

static __u_int inversion_bits      (__u_int register_remainder);
static __u_int calculate_remainder (__u_int register_remainder, size_t max_index_byte, char* str);

//extern "C" size_t calculate_remainder_asm (char* str_for_crc32); 

//---------------------------------------------------------------------------------------------------------

size_t hash_sum_ascii (char* str)
{
	assert (str);

	size_t sum 	  = 0,
		   symbol = 0;
	
	while ((symbol = (size_t) (*str)) != '\0')
	{
		sum += symbol;
		str++;
	}

	return sum % COUNT_BUCKETS;
}

size_t hash_sum_of_squares (char* str)
{
	assert (str);

	size_t sum    = 0,
		   symbol = 0;

	while ((symbol = (size_t) (*str)) != '\0')
	{
		sum += symbol * symbol;
		str++;
	}

	return sum % COUNT_BUCKETS;
}

size_t hash_djb2 (char* str)
{
	assert (str);

	size_t hash   = BASE_NUMBER_DJB2,
		   symbol = 0;

	for (size_t byte = 0; (symbol = (size_t) str[byte]) != '\0'; byte++)
	{
		hash = ((hash << SHIFTING_IN_GFB2) + hash) ^ symbol;
	}

	return hash % COUNT_BUCKETS;
}

size_t hash_ascii_first_symbol (char* str)
{
	assert (str);

	return (size_t) ((*str) % COUNT_BUCKETS);
}

size_t hash_mul_all_ascii (char* str)
{
	assert (str);

	size_t mul    = 1,
		   symbol = 0;

	while ((symbol = (size_t) (*str)) != '\0')
	{
		mul *= (size_t) symbol;
		str++;
	}

	return mul % COUNT_BUCKETS;
}

size_t hash_average (char* str)
{
	assert (str);

	size_t sum           = 0,
	       count_symbols = 0,
		   symbol        = 0;

	while ((symbol = (size_t) (*str)) != '\0')
	{
		sum += symbol;

		count_symbols++;
		str++;
	}

	return (sum / count_symbols) % COUNT_BUCKETS;
}

//------------------------------------------------------------------------------------------------------------------------

// 0 - 0000
// 1 - 0001
// 2 - 0010
// 3 - 0011
// 4 - 0100
// 5 - 0101
// 6 - 0110
// 7 - 0111
// 8 - 1000
// 9 - 1001
// A - 1010
// B - 1011
// C - 1100
// D - 1101
// E - 1110
// F - 1111

size_t hash_crc32 (char* str)
{
	assert (str);

	#ifdef DEBUG_CRC32
	printf ("POLYNOMIAL ==               %32b\n\n", POLYNOMIAL);
	#endif

	__u_int register_remainder = 0;

	size_t count_bytes_in_str = strlen (str);
	size_t additional_bytes   = sizeof (__u_int);

	char str_null[MAX_LEN_WORD] = "\0";

	if (count_bytes_in_str >= sizeof (__u_int))
	{
		register_remainder = *((__u_int*) str);
	}

	else
	{
		for (size_t index_byte = 0; index_byte < count_bytes_in_str; index_byte++)
		{
			register_remainder += str[index_byte];
			register_remainder <<= BITS_IN_BYTE;
		}

		register_remainder = (__u_int) ((sizeof (__u_int) - count_bytes_in_str) * BITS_IN_BYTE);

		additional_bytes = count_bytes_in_str;
	}

	#ifdef DEBUG_CRC32
	printf ("register_remainder ==       %32b\n", register_remainder);
	#endif
	
	register_remainder = calculate_remainder (register_remainder, count_bytes_in_str, str);
	register_remainder = calculate_remainder (register_remainder, additional_bytes,   str_null);

	#ifdef DEBUG_CRC32
	printf ("----");
	getchar ();
	#endif

	return (size_t) (register_remainder % COUNT_BUCKETS);
}

static __u_int calculate_remainder (__u_int register_remainder, size_t max_index_byte, char* str)
{
	__u_int high_bit = 0;

	for (size_t index_byte = 0; index_byte < max_index_byte; index_byte++)
	{
		char byte = str[index_byte];

		char mask_next_bit = MASK_HIGH_BIT_BYTE;

		for (long number_bit = sizeof (char) - 1; number_bit >= 0; number_bit--)
		{
			high_bit = register_remainder & MASK_HIGH_BIT_IN_REGISTER;

			register_remainder <<= COUNT_SHIFTING_BITS;
			register_remainder += (byte & mask_next_bit) >> number_bit;
			mask_next_bit >>= COUNT_SHIFTING_BITS;

			#ifdef DEBUG_CRC32
			printf ("register_remainder ==       %32b\n", register_remainder);
			printf ("high_bit ==                 %32b\n", high_bit);
			#endif

			if (high_bit)
			{
				register_remainder = inversion_bits (register_remainder);
				#ifdef DEBUG_CRC32
				printf ("register_remainder ==       %32b\n\n", register_remainder);
				#endif
			}

			#ifdef DEBUG_CRC32
			printf ("----");
			getchar ();
			#endif
		}
	}

	return register_remainder;
}

static __u_int inversion_bits (__u_int register_remainder)
{
	__u_int mask_next_bit = MASK_FIRST_BIT;

	__u_int next_bit_in_polynomial   = 0;
	__u_int next_bit_in_old_register = 0;

	for (size_t iteration = 0; iteration < sizeof (__u_int); iteration++)
	{	
		next_bit_in_polynomial   = (POLYNOMIAL & mask_next_bit);
		#ifdef DEBUG_CRC32
		printf ("\nnext_bit_in_polynomial ==   %32b\n", next_bit_in_polynomial);
		#endif

		if (next_bit_in_polynomial == 0)
		{
			mask_next_bit <<= COUNT_SHIFTING_BITS;
			continue;
		}

		next_bit_in_old_register = register_remainder & mask_next_bit;

		#ifdef DEBUG_CRC32
		printf ("next_bit_in_old_register == %32b\n\n", next_bit_in_old_register);
		#endif

		if (next_bit_in_old_register == 0)
			register_remainder |= mask_next_bit;
		
		else
		{
			#ifdef DEBUG_CRC32
			printf ("mask_next_bit      == %32b\n", mask_next_bit);
			printf ("!mask_next_bit     == %32b\n", ~mask_next_bit);
			#endif

			register_remainder &= (~mask_next_bit);
		}

		#ifdef DEBUG_CRC32
		printf ("register_remainder ==       %32b\n", register_remainder);
		#endif

		mask_next_bit <<= COUNT_SHIFTING_BITS;

		#ifdef DEBUG_CRC32
		printf ("----");
		getchar ();
		#endif
	}

	return register_remainder;
}

//------------------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t print_inf_about_func_hash (list_t* hash_table, char* str_name_func_hash, char* name_inf_file)
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

	//printf ("load_factor = %lg\n", ((double) load_factor) / COUNT_BUCKETS);

	if (fclose (inf_file) == EOF)
	{
		printf ("Error in %s:%d\n"
				"Cannot close file (%s)\n\n", __FILE__, __LINE__, name_inf_file);

		return CANNOT_CLOSE_FILE;
	}

	return NOT_ERROR;
}

//----------------------------------------------------------------------------------------------------------------------------

// size_t hash_crc32_asm (char* str)
// {
// 	assert (str);

// 	size_t register_remainder = calculate_remainder_asm (str);

// 	return (size_t) (register_remainder % COUNT_BUCKETS);
// }

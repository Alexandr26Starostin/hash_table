#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <x86intrin.h>

#include "const_in_hash_table.h"
#include "list.h"
#include "func_hash.h"
#include "text_and_files.h"
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

#ifdef PRINT_WORDS_FOR_SEARCH
static errors_in_hash_table_t print_words_for_search   (char** words_for_search, size_t count_words);
#endif

#ifdef TEST_PROGRAM
	static errors_in_hash_table_t find_words_in_hash_table (int argc, char** argv, inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search);
#else
	static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search);
#endif

#ifdef PRINT_INF_ABOUT_HASH_FUNC
	static errors_in_hash_table_t get_inf_func_hash (int argc, char** argv, list_t* hash_table);
#endif

#ifdef PRINT_CPE_RESULT
	static errors_in_hash_table_t print_cpe_result   (inf_cpe_t cpe_result);
#endif

#ifdef TEST_PROGRAM
	static errors_in_hash_table_t analyze_cpe_result (int argc, char** argv, inf_cpe_t cpe_result);
	static errors_in_hash_table_t fill_file_plot     (FILE* file_plot, inf_cpe_t cpe_result);
#endif

static search_in_cash_t search_element_in_cash (cash_t cash_with_words, char* word, element_in_cash_t* ptr_word_from_cash);

//----------------------------------------------------------------------------------------------------------------

errors_in_hash_table_t create_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table)
{
	assert (ptr_inf_hash_table);

	errors_in_hash_table_t status = NOT_ERROR;

	list_t* hash_table = ptr_inf_hash_table -> hash_table;

	status = ctor_lists_in_hash_table (hash_table);
	if (status)	{return status;}

	FILE* file_words = fopen (argv[INDEX_FILE_WORDS_IN_ARGV], "r");
	if (file_words == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_words\n"
				"Error in fopen(); or you don't write name of file with words: "
				"./hash_table <file>\n\n", __FILE__, __LINE__);
			
		delete_hash_table (hash_table);

		return CANNOT_OPEN_FILE;
	}
	
	//--------------------------------------------------------------------------------------------------------------------

	status = fill_hash_table (argc, argv, ptr_inf_hash_table, file_words);
	if (status)
	{
		printf ("Error in %s:%d\n"
				"Failed fill_hash_table\n\n", __FILE__, __LINE__);

		CLOSE_FILE_(file_words, "file_words");

		delete_hash_table (hash_table);

		return FAILED_HASH_TABLE;
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

			delete_hash_table (hash_table);

			return FAILED_CTOR_LIST;
		}
	}

	return NOT_ERROR;
}

errors_in_hash_table_t delete_hash_table (list_t* hash_table)
{
	assert (hash_table);

	for (size_t index_bucket = 0; index_bucket < COUNT_BUCKETS; index_bucket++)
	{
		dtor_list (hash_table + index_bucket);
	}

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_hash_table (int argc, char** argv, inf_hash_table_t* ptr_inf_hash_table, FILE* file_words)
{
	assert (ptr_inf_hash_table);
	assert (ptr_inf_hash_table -> hash_table);
	assert (file_words);
	assert (argv);
	//ptr_inf_hash_table -> func_hash == NULL

	errors_in_hash_table_t status = NOT_ERROR;

	char word[MAX_BYTES_IN_WORD] = "\0";

	size_t (*func_hash) (char*) = NULL;
	list_t* hash_table          = ptr_inf_hash_table -> hash_table;

	status = find_name_func_hash (argc, argv, &func_hash);
	if (status) {return status;}
	
	//-------------------------------------------------------------------------------------

	while (! feof (file_words))      //while (not end of file)
	{
		get_word_from_file (file_words, word);   //return NOT_ERROR;
	
		//print_str_32_bytes (word);

		size_t index_bucket = func_hash (word);

		//printf ("%s\t%ld\n", word, index_bucket);

		status = add_element_in_list (hash_table + index_bucket, word);
		if (status)
		{
			printf ("Error in %s:%d\n"
					"Failed ctor_list of list by index == %ld\n\n", __FILE__, __LINE__, index_bucket);

			return status;
		}
	}

	//printf ("\n");

	#ifdef PRINT_INF_ABOUT_HASH_FUNC
		status = get_inf_func_hash (argc, argv, hash_table);
		if (status) {return status;}
	#endif

	ptr_inf_hash_table -> func_hash = func_hash;

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

	errors_in_hash_table_t status = get_words_for_search (argc, argv, &inf_for_search);
	if (status) {return status;}

	#ifdef PRINT_WORDS_FOR_SEARCH
	print_words_for_search (inf_for_search.words_for_search, inf_for_search.count_words);
	#endif

	#ifdef TEST_PROGRAM
	find_words_in_hash_table (argc, argv, *ptr_inf_hash_table, inf_for_search);   //return NOT_ERROR;
	#else
	find_words_in_hash_table (*ptr_inf_hash_table, inf_for_search);   //return NOT_ERROR;
	#endif

	delete_words_for_search (inf_for_search);

	return NOT_ERROR;
}

#ifdef PRINT_WORDS_FOR_SEARCH
static errors_in_hash_table_t print_words_for_search (char** words_for_search, size_t count_words)
{
	assert (words_for_search);

	printf ("------------------------------------------------------------------------\nwords_for search:\n\n");

	for (size_t index_word = 0; index_word < count_words; index_word++)
	{
		printf ("%s\n", words_for_search[index_word]);
	}

	printf ("------------------------------------------------------------------------\n\n");

	return NOT_ERROR;
}
#endif

#ifdef TEST_PROGRAM
static errors_in_hash_table_t find_words_in_hash_table (int argc, char** argv, inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
#else
static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
#endif
{
	assert (inf_hash_table.hash_table);
	assert (inf_hash_table.func_hash);
	assert (inf_for_search.words_for_search);

	#ifdef TEST_PROGRAM
	assert (argv);
	#endif

	size_t  count_words      = inf_for_search.count_words;
	size_t  index_bucket     = 0;
	char*   word             = NULL;
	char**  words_for_search = inf_for_search.words_for_search;
	list_t* hash_table       = inf_hash_table.hash_table;
	
	size_t (*func_hash) (char*) = inf_hash_table.func_hash;

	cash_t 			   cash_with_words  = {};
	//element_in_cash_t* elements_in_cash = cash_with_words.elements_in_cash;
	element_in_cash_t  word_from_cash   = {};

	//--------------------------------------------------------------------------

	size_t max_index_word = count_words;

	#ifdef TEST_PROGRAM

	size_t index_cpe = 0;

	inf_test_t* tests = (inf_test_t*) calloc (count_words * COUNT_REPEATING, sizeof (inf_test_t));
	if (tests == NULL)
	{
		printf ("Error in %s:%d\n"
				"Have not memory for ticks from calloc\n\n", __FILE__, __LINE__);

		return NOT_MEMORY_FROM_CALLOC;
	}

	inf_cpe_t cpe_result = {.max_iterations = max_index_word, 
							.tests          = tests};

	for (size_t index_repeating = 0; index_repeating < COUNT_REPEATING; index_repeating++)
	{
		for (size_t iterations = 1; iterations <= count_words; iterations++)
		{
			max_index_word = iterations;

			size_t ticks_before_cycle = __rdtsc();
	#endif

			for (size_t index_word = 0; index_word < max_index_word; index_word++)
			{
				word = words_for_search[index_word];

				//printf ("%s\n", word);

				//getchar ();

				search_in_cash_t status_cash = search_element_in_cash (cash_with_words, word, &word_from_cash);
				if (status_cash)
				{
					#ifndef TEST_PROGRAM
					printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n"
							"counter_word == %ld\n-----------------------------------------\n\n", 
							word, word_from_cash.index_bucket, word_from_cash.count_words_in_text);
					#endif

					continue;
				}

				index_bucket = func_hash (word);

				//printf ("%s\t%ld\n", word, index_bucket);

				#ifndef TEST_PROGRAM
				printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n", word, index_bucket);				
				#endif

				size_t counter_word = find_element_in_list (hash_table + index_bucket, word);

				#ifndef TEST_PROGRAM
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

	#ifdef TEST_PROGRAM
			size_t ticks_after_cycle = __rdtsc();

			size_t ticks = ticks_after_cycle - ticks_before_cycle;

			//printf ("iteration == %4ld\t\tticks == %ld\n", iterations, ticks);

			(tests[index_cpe]).ticks      = ticks;
			(tests[index_cpe]).iterations = iterations;
			index_cpe++;
		}
	}

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
#ifdef PRINT_CPE_RESULT

static errors_in_hash_table_t print_cpe_result (inf_cpe_t cpe_result)
{
	inf_test_t* tests = cpe_result.tests;
	assert (tests);

	size_t max_iterations = cpe_result.max_iterations;
	size_t max_index      = max_iterations * COUNT_REPEATING;

	printf ("-----------------------------------\ninf about cpe_result:\n"
			"max_iterations == %ld\nCOUNT_REPEATING == %ld\n\n"
			"iterations       ticks\n", max_iterations, COUNT_REPEATING);

	for (size_t index = 0; index < max_index; index++)
	{
		printf ("%10ld       %ld\n", (tests[index]).iterations, (tests[index]).ticks);
	}

	printf ("-----------------------------------\n\n");

	return NOT_ERROR;
}
#endif

#ifdef TEST_PROGRAM
static errors_in_hash_table_t analyze_cpe_result (int argc, char** argv, inf_cpe_t cpe_result)
{
	assert (argv);

	FIND_FLAG_("-plot", "Not find flag: -plot <file_for_with_code_for_print_plot.py>", NOT_FIND_FLAG_PLOT)

	FILE* file_plot = fopen (argv[index_argc], "w");
	if (file_plot == NULL)
	{
		printf ("Error in %s:%d\n"
				"Cannot open file_search\n"
				"Error in fopen(); or you don't <file_for_with_code_for_print_plot.py>:"
				"-plot <file_for_with_code_for_print_plot.py>\n\n", __FILE__, __LINE__);

		return CANNOT_OPEN_FILE;
	}

	errors_in_hash_table_t status = fill_file_plot (file_plot, cpe_result);
	if (status) 
	{
		CLOSE_FILE_(file_plot, "file_plot")

		return status;
	}

	CLOSE_FILE_(file_plot, "file_plot")

	return NOT_ERROR;
}

static errors_in_hash_table_t fill_file_plot (FILE* file_plot, inf_cpe_t cpe_result)
{
	assert (file_plot);

	inf_test_t* tests = cpe_result.tests;
	assert (tests);

	size_t max_iterations = cpe_result.max_iterations;
	size_t max_index      = max_iterations * COUNT_REPEATING;

	fprintf (file_plot, 

	"import numpy as np\n"
	"import matplotlib.pyplot as plt\n"
	"from scipy import stats\n\n"

	"def detect_outliers(x, y, threshold=2.5):\n"
		"\t\"\"\"\n"
		"\tОбнаруживает выбросы с помощью метода межквартильного размаха (IQR)\n"
		"\tВозвращает маску с True для выбросов\n"
		"\t\"\"\"\n"
		"\t# Сначала выполняем линейную регрессию\n"
		"\tslope, intercept, _, _, _ = stats.linregress(x, y)\n"
		"\tresiduals = y - (slope * x + intercept)\n\n"
		
		"\t# Вычисляем квартили и межквартильный размах\n"
		"\tQ1 = np.percentile(residuals, 25)\n"
		"\tQ3 = np.percentile(residuals, 75)\n"
		"\tIQR = Q3 - Q1\n\n"
		
		"\t# Определяем границы для выбросов\n"
		"\tlower_bound = Q1 - threshold * IQR\n"
		"\tupper_bound = Q3 + threshold * IQR\n\n"
		
		"\t# Возвращаем маску выбросов\n"
		"\treturn (residuals < lower_bound) | (residuals > upper_bound)\n\n"

	"def linear_regression_with_errors(x, y):\n"
		"\t\"\"\"\n"
		"\tВыполняет линейную регрессию и вычисляет погрешности параметров\n"
		"\tВозвращает:\n"
			"\t\tslope, intercept - параметры прямой\n"
			"\t\tslope_err, intercept_err - погрешности параметров\n"
			"\t\tr_squared - коэффициент детерминации\n"
		"\t\"\"\"\n"
		"\tn = len(x)\n\n"
		
		"\t# Вычисляем основные суммы\n"
		"\tsum_x = np.sum(x)\n"
		"\tsum_y = np.sum(y)\n"
		"\tsum_xy = np.sum(x * y)\n"
		"\tsum_x2 = np.sum(x ** 2)\n"
		"\tsum_y2 = np.sum(y ** 2)\n\n"
		
		"\t# Коэффициенты МНК\n"
		"\tslope = (n * sum_xy - sum_x * sum_y) / (n * sum_x2 - sum_x ** 2)\n"
		"\tintercept = (sum_y - slope * sum_x) / n\n\n"
		
		"\t# Предсказанные значения и остатки\n"
		"\ty_pred = slope * x + intercept\n"
		"\tresiduals = y - y_pred\n\n"
		
		"\t# Стандартное отклонение остатков\n"
		"\tsigma = np.sqrt(np.sum(residuals ** 2) / (n - 2))\n\n"
		
		"\t# Погрешности параметров\n"
		"\tx_mean = np.mean(x)\n"
		"\tS_xx = np.sum((x - x_mean) ** 2)\n\n"
		
		"\tslope_err = sigma / np.sqrt(S_xx)\n"
		"\tintercept_err = sigma * np.sqrt(1 / n + x_mean ** 2 / S_xx)\n\n"
		
		"\t# Коэффициент детерминации R²\n"
		"\tss_total = np.sum((y - np.mean(y)) ** 2)\n"
		"\tss_residual = np.sum(residuals ** 2)\n"
		"\tr_squared = 1 - (ss_residual / ss_total)\n\n"
		
		"\treturn slope, intercept, slope_err, intercept_err, r_squared\n\n"

	"def plot_regression_with_outliers(x, y, threshold=2.5):\n"
		"\t\"\"\"\n"
		"\tСтроит график с выделением выбросов и линией регрессии\n"
		"\t\"\"\"\n"
		"\t# Обнаруживаем выбросы\n"
		"\toutlier_mask = detect_outliers(x, y, threshold)\n\n"
		
		"\t# Выполняем регрессию без выбросов\n"
		"\tclean_x = x[~outlier_mask]\n"
		"\tclean_y = y[~outlier_mask]\n\n"
		
		"\tslope, intercept, slope_err, intercept_err, r_squared = linear_regression_with_errors(clean_x, clean_y)\n\n"
		
		"\t# Создаем график\n"
		"\tplt.figure(figsize=(10, 6))\n\n"
		
		"\t# Рисуем нормальные точки (синие)\n"
		"\tplt.scatter(clean_x, clean_y, color='blue', label='Нормальные точки')\n\n"
		
		"\t# Рисуем выбросы (красные)\n"
		"\tif np.any(outlier_mask):\n"
			"\t\tplt.scatter(x[outlier_mask], y[outlier_mask], color='red', label='Выбросы')\n\n"
		
		"\t# Рисуем линию регрессии\n"
		"\tx_fit = np.linspace(min(x), max(x), 100)\n"
		"\ty_fit = slope * x_fit + intercept\n"
		"\tplt.plot(x_fit, y_fit, color='green', label=f'МНК: y = ({slope:.3f}±{slope_err:.3f})x + ({intercept:.3f}±{intercept_err:.3f})')    #\\nR² = {r_squared:.3f}\n\n" 
		
		"\t# Настраиваем график\n"
		"\tplt.xlabel('итераций в цикле', fontsize=12)\n"
		"\tplt.ylabel('такты синхронизации на весь цикл', fontsize=12)\n"
		"\tplt.title('Линейная регрессия с выделением выбросов для измерения CPE', fontsize=14)\n"
		"\tplt.legend(fontsize=10)\n"
		"\tplt.grid(True, linestyle='--', alpha=0.6)\n\n"
		
		"\t# Выводим статистику\n"
		"\tprint(\"=== Результаты регрессии ===\")\n"
		"\tprint(f\"Угловой коэффициент: {slope:.5f} ± {slope_err:.5f}\")\n"
		"\tprint(f\"Смещение: {intercept:.5f} ± {intercept_err:.5f}\")\n"
		"\t#print(f\"Коэффициент детерминации R²: {r_squared:.5f}\")\n"
		"\tprint(f\"Обнаружено выбросов: {np.sum(outlier_mask)} из {len(x)} точек\")\n\n"
		
		"\tplt.tight_layout()\n"
		"\tplt.savefig('plot_cpe.png')\n\n"

	"# Пример использования\n"
	"if __name__ == \"__main__\":\n\n"
		
		"\tx = np.array([");


	for (size_t index = 0; index < max_index; index++)
	{
		fprintf (file_plot, "%7ld, ", (tests[index]).iterations);
	}

	fprintf (file_plot, "])\n"
						"\ty = np.array([");

	for (size_t index = 0; index < max_index; index++)
	{
		fprintf (file_plot, "%7ld, ", (tests[index]).ticks);
	}

	fprintf (file_plot, "])\n\n"
						"\tplot_regression_with_outliers(x, y, threshold=2.5)\n");

	return NOT_ERROR;
}

#endif
//---------------------------------------------------------------------------------------------------------------------------

static search_in_cash_t search_element_in_cash (cash_t cash_with_words, char* word, element_in_cash_t* ptr_word_from_cash)
{
	assert (word);
	assert (ptr_word_from_cash);

	search_in_cash_t status = NOT_FIND_IN_CASH;

	#ifndef INLINE_ASM

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

	return status;

	//---------------------------------------------------------------------------------------------

	#else

	bool (*ptr_compare_element) (char*, char*) = compare_element;

	asm volatile (
		".intel_syntax noprefix\n\t"  //rdi = cash_with_words 			 rsi = word  		rdx = ptr_word_from_cash		
		//"mov rdi, %[cash]\n\t"
		//"push r8\n\t"

		"mov rsi, %[word]\n\t"
		"mov rdx, %[ptr]\n\t"


		"mov rbx, %[cash]\n\t"           
		"add rbx, 8\n\t"                //rbx = elements_in_cash = cash_with_words.elements_in_cash
		"xor ecx, ecx\n\t"              //rcx = index_el = 0
		"mov r12, 4\n\t"                //r12 = SIZE_CASH_WITH_WORDS = 4

		// "sub rsp, 24d\n\t"
		// "mov r9, rsp\n\t"              //r9 = element 

		"check_next_cash:\n\t"

		"push rdx\n\t"
		"mov rax, rcx\n\t"
		"mov r15, 24\n\t"
		"mul r15\n\t"
		"mov r13, [rbx + rax]\n\t"    //r13 = elements_in_cash[index_el];
										  //r13 = word_el = element.word;
		"pop rdx\n\t"

		"test r13, r13\n\t"
		"jz word_is_null\n\t"
		//--------------------------------------------------------------------------------------------------------
      
		//"push rdi\n\t"
		"push rsi\n\t"
		"push rcx\n\t"
		"push rdx\n\t"

		"mov rdi, r13\n\t"
		//rsi not changed

		"call %[ptr_func]\n\t"

		"pop rdx\n\t"
		"pop rcx\n\t"
		"pop rsi\n\t"
		//"pop rdi\n\t"

		"test al, al\n\t"
		"jz not_elem_from_cash\n\t"

		//----------------------------------------------------------------------------------------------------------
  
		"push rdx\n\t"
		"mov rax, rcx\n\t"
		"mov r15, 24\n\t"
		"mul r15\n\t"
		"add r13, rax\n\t"  //r13 = rbx + rcx * 24
		"pop rdx\n\t"

		"mov r14, [r13]\n\t"
		"mov [rdx], r14\n\t"

		"mov r14, [r13 + 8]\n\t"
		"mov [rdx + 8], r14\n\t"

		"mov r14, [r13 + 16]\n\t"
		"mov [rdx + 16], r14\n\t"      //*ptr_word_from_cash = element;

		"mov %0, 1\n\t"
		"jmp end_asm\n\t"

		//-----------------------------------------------------------------------------------------------------------

		"not_elem_from_cash:\n\t"

		"inc rcx\n\t"
		"cmp rcx, r12\n\t"
		"jnz check_next_cash\n\t"

		"word_is_null:\n\t"

		"mov %0, 0\n\t"
		"end_asm:\n\t"

		//"pop r8\n\t"

		//-----------------------------------------------------------------------------------------------------
		:"=r" (status)
		:[cash] "r" (&cash_with_words), 
		 [word] "r" (word), 
		 [ptr]  "r" (ptr_word_from_cash),
		 [ptr_func] "r" (ptr_compare_element)
		:"rbx", "rcx", "rdi", "rsi", "rdx", "r12", "r13", "r14", "r15", "rax", "memory", "cc"
		);

	return status;

	#endif
}


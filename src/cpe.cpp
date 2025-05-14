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

//--------------------------------------------------------------------------------------------------

errors_in_hash_table_t print_cpe_result (inf_cpe_t cpe_result)
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

errors_in_hash_table_t analyze_cpe_result (int argc, char** argv, inf_cpe_t cpe_result)
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

errors_in_hash_table_t fill_file_plot (FILE* file_plot, inf_cpe_t cpe_result)
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

# hash_table

## Самое важное

Самое важное, что будет в этом проекте:

- знакомство с хэш-таблицей,
- написание hash функции crc32,
- анализ hash функций на коллизию,
- проведение измерения и анализа производительности программы,
- проведение оптимизаций программы.

## О чём этот проект

Хэш-таблица - это структура данных, которая позволяет быстро находить хранящиеся в ней данные. Каждому элементу в хэш-таблице соответствует свой ключ. Если нам нужно найти элемент в хэш-таблице, то достаточно узнать ключ и по нему найти элемент и информацию о нём в хэш-таблице. Другими словами,  хэш-таблица - структура данных, отображающая множество ключей в множество значений.

В данном проекте мы напишем свою хэш-таблицу и будем использовать её для поиска слов. После этого протестируем программу и оптимизируем неэффективные участки.

## На чём написан проект

Проект написан на arch-Linux, процессор Intel N95, используется 15.4 Гб оперативной памяти.

## Сборка и использование

Для использования данной программы необходимо:
1. Скопировать себе репозиторий:

```
git clone https://github.com/Alexandr26Starostin/my_pritf.git
```

2. Перейти в директорию с программой для подготовки слов для записи в хэш-таблицу:

```
./prepare_file_with_words
```

3. Скомпилировать программу с подготовкой слов:
```
make
```

4. Выполнить подготовку слов:
```
./prepare_file -w ../file_with_words.txt -r ../Five_Weeks_in_a_Balloon.txt
```
> -w ../file_with_words.txt - файл с подготовленными для записи словами

> -r ../Five_Weeks_in_a_Balloon.txt - исходный текст

Данная программа запишет слова из исходного текста в столбик, подготовив их к записи в хэш-таблицу.

5. Вернуться в директорию с хэщ-таблицей:
```
../
```

6. Скомпилировать программу с хэш-таблицей:
```
make MODE=RELEASE
```

В этой версии компиляции отключены все флаги контроля кода, отключён *sanitizer*, выставлен -DNDEBUG. Уровень оптимизаций по умолчанию стоит *-O0*. Его можно изменить вручную, если нужно. Этот режим используется в итоговой версии программы.

Программа с хэш-таблицей имеет несколько версий компиляции. Далее будут рассказаны и другие версии.

7. Вызвать программу:
```
./hash_table ./file_with_words.txt -hash hash_crc32 -find ./words_for_search.txt
```

> ./hash_table - вызов исполняемого файла (обяз.)

> ./file_with_words.txt - файл с словами для заполнения хэш-таблицы (всегда первый параметр) (обяз.)

> -hash hash_crc32 - выбор хэш-функции (обяз.)

>-find ./words_for_search.txt - файл со словами для поиска (слова должны быть записаны в столбик) (обяз.)

Программа с хэш-таблицей может иметь дополнительные параметры, о которых будет сказано далее.

Если программу с хэш-таблицей скомпилировать:
```
make MODE=DEBUG
```

то будут включены *assert*, все флаги проверки кода, будет включен *sanitizer*, уровень оптимизаций будет выставлен на *-O0*. Этот режим можно использовать при поиске ошибок и внесении изменений в код программы.

## Подробнее о хэш-таблицах

Как уже было сказано ранее, хэш-таблица - структура данных, отображающая множество ключей в множество значений. Каждому элементу соответствует свой ключ. Этот ключ можно получить при помощи какой-либо хэш-функции. Хэш-функция - это функция, которая для каждого элемента высчитывает определённым способом ключ. Однако хэш-функция может вызвать коллизию. Коллизия - это соответствие одному ключу нескольких данных. Те для некоторых элементов хэш-функция может вернуть одинаковые ключи. Очевидно, что коллизия нарушает однозначность соответствия ключ - элемент и ухудшает поиск элемента в хэш-таблице. Поэтому хэш-таблицы делятся на два вида по способу борьбы с коллизией: с цепочками и с открытой адресацией. Первая использует списки для хранения элементов (подробнее ниже). Вторая - массив из структур элемент-ключ и поиск свободной ячейки в этом массиве с помощью последовательности функций, принимающих ключ и возвращаемых значение адреса возможно свободной ячейки.

В нашем проекте используется хэш-таблица с цепочками. Сама хэш-таблица - массив указателей на бакеты: односвязные списки (о списках см. ниже). Ключ является индексом списка (бакета) в этом массиве. Когда мы хотим добавить элемент в хэш-таблица, хэш-функция для элемента вычисляет ключ, этот ключ становится индексом списка, в который нужно будет добавить элемент. При поиске элемента выполняется те же действия, но теперь ключ - индекс списка, в котром нужно искать наш элемент. Ниже представлена схема списка с цепочками (схема взята с [этого](https://neerc.ifmo.ru/wiki/index.php?title=%D0%A0%D0%B0%D0%B7%D1%80%D0%B5%D1%88%D0%B5%D0%BD%D0%B8%D0%B5_%D0%BA%D0%BE%D0%BB%D0%BB%D0%B8%D0%B7%D0%B8%D0%B9) источника):


<div style="text-align:center">
    <img src="./pictures/hash_table.png#center" alt="Схема хэш-таблицы с цепочками">
</div>

Списки же могут быть, например, односвязными, двусвязными, кольцевыми, friendly список и тд. В нашем проекте мы используем односвязные списки. В них каждый элемент списка указывает на следующий за ним. Последний элемент (*tail*) указывает на *null* . При добавлении слова в список будет осуществлён линейный поиск по списку, начиная с первого элемента в списке (*head*). Если добавляемый элемент будет найден, то произойдёт увеличения счётчика, равного количеству, сколько раз данное слово встретилось в тексте. Если же элемент не будет найден, то он будет добавлен в конец список, и счётчик будет равен *1*. При поиске будет возвращаться значение счётчика, если элемент найден в списке, или *0*, если нет. Ниже представлена схема односвязного списка (схема взята с [этого](https://prog-cpp.ru/data-ols-oop/) источника):

<div style="text-align:center">
    <img src="./pictures/list.png#center" alt="Схема хэш-таблицы с цепочками">
</div>

После заполнения хэш-таблицы из-за коллизии в разных бакетах будет сохранено разное количество элементов. Если коллизия низкая, то во всех бакетах будет приблизительно равное количество элементов, и оно должно стремиться к среднему. Среднее количество элементов в бакете называется *load_factor*. Чем больше бакетов в хэш_таблице, тем меньше *load_factor*. Тк линейный поиск внутри списка не эффективен, то идеальный *load_factor* для хэш-таблицы равен *1.5 - 2*. Но в нашем проекте мы возьмём *load_factor*, равный *10 - 15*. Этим мы искусственно создадим неэффективность в поиске элементов в хэш-таблице, которую мы позже сможем оптимизировать.

Теперь мы можем написать хэш-таблицу с цепочками на односвязных списках и после проанализировать различные хэш-функции на коллизию.

## Написание односвязного списка

Для всего проекта выполняется, что все структуры, типы и константы определены в файле [const_in_hash_table.h](https://github.com/Alexandr26Starostin/hash_table/blob/master/include/const_in_hash_table.h) 

В начале напишем свой односвязный список. Код с нашим списком можно найти в файлах [list.h](https://github.com/Alexandr26Starostin/hash_table/blob/master/include/list.h) и [list.сpp](https://github.com/Alexandr26Starostin/hash_table/blob/master/src/list.cpp). Принципы работы односвязного списка были разобраны раньше. Из самого важного отметим структуры, которые определяют строение списка:

<details>
<summary>Cтруктуры, определяющие строение списка</summary>

```C
typedef char* data_t;

struct node_t
{
	node_t* next_element;
	size_t  counter_element;
	data_t  data;
};

struct list_t 
{
	size_t  size_of_list;
	node_t* head;
	node_t* tail; 	
};

```
</details>

Элементами списков являются слова, которые копируются в динамическую память, указатели на которую лежат в элементах списков:

<details>
<summary>Фрагмент кода, выполняющий копирование элементов</summary>

```C
if (strncpy(element -> data, data, MAX_LEN_WORD) == NULL)
	{
		printf ("Error in %s:%d\n"
				"Failed strncpy();\n\n", __FILE__, __LINE__);

		return FAILED_STRNCPY;
	}
``` 
</details>

Особенно важной в этом файле является функция сравнения элементов, работающая как *strcmp*:

<details>
<summary>Функция сравнения элементов</summary>

```C
bool compare_element (data_t element_1, data_t element_2)
{
	//data_t == char*
	//data_t == char[32]
	
	assert (element_1);
	assert (element_2);

	size_t index = 0;

	char symbol_1 = '\0';
	char symbol_2 = '\0';

	while ((symbol_1 = element_1[index]) != '\0' and (symbol_2 = element_2[index]) != '\0')
	{
		if (symbol_1 - symbol_2)
		{
			return false;   //element_1 != element_2
		}

		index++;
	}

	if (element_1[index] == '\0' and element_2[index] == '\0')
		return true;     //element_1 == element_2

	return false;
}
``` 
</details>

## Написание хэш-функций

Теперь нам нужно подобрать подходящую хэш-функцию (обладающую наименьшей коллизией). Напишем небольшой набор различных хэш-функций, которые будут нами протестированы на коллизию. Хэш-функции можно найти в файлах [func_hash.h](https://github.com/Alexandr26Starostin/hash_table/blob/master/include/func_hash.h) и [func_hash.cpp](https://github.com/Alexandr26Starostin/hash_table/blob/master/src/func_hash.cpp). Вот краткое описание всех написанных нами хэш-функций:

- hash_sum_ascii - сумма ascii всех символов в слове,
- hash_sum_of_squares - сумма квадратов ascii всех символов в слове,
- hash_djb2 - считает по алгоритму djb2 (о djb2 можно прочитать [здесь](http://www.cse.yorku.ca/~oz/hash.html)),
- hash_ascii_first_symbol - ascii первой буквы в слове,
- hash_mul_all_ascii - произведение ascii всех символов в слове,
- hash_average - среднее значение ascii всех символов в слове,
- hash_crc32 - считает по алгоритму crc32 (о crc32 можно прочитать [здесь](https://soltau.ru/index.php/themes/dev/item/461-kak-poschitat-kontrolnuyu-summu-crc32-crc16-crc8)).

Все функции возвращают значение меньшее, чем количество бакетов в хэш_таблице (это значение равно индексу бакета).

Хэш-функцию, которая будет работать в программе, мы обязаны передать параметром командной строки с помощью: 

```
-hash <имя_хэш_функции>
```

Также в этом файле есть важная функция, анализирующая работу хэш-функции. Эта функция строит график количества элементов в бакете от индекса бакета (ключа), считает load_factor хэш-таблицы и создаёт файл.tex с результатами анализа.

<details>
<summary>Функция, анализирующая работу хэш-функции</summary>

```C

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

``` 
</details>

## Заполнение хэш-таблицы

Файлы, связанные с хэш_таблицей (заполнение и поиск элементов): [hash_table.h](https://github.com/Alexandr26Starostin/hash_table/blob/master/include/hash_table.h) и [hash_table.cpp](https://github.com/Alexandr26Starostin/hash_table/blob/master/src/hash_table.cpp).

Чтобы заполнить хэш-таблицу, мы будем вытаскивать слова из ранее подготовленного текста, вычислять для него с помощью хэш-функции индекс бакета и класть в него слово. После заполнения хэш-таблицы мы можем проанализировать хэш-функции. Чтобы это сделать, мы должны в [*Makefile*](https://github.com/Alexandr26Starostin/hash_table/blob/master/Makefile) для хэш-таблицы прописать:

```
DEFINES=-DPRINT_INF_ABOUT_HASH_FUNC
```

и при вызове передать в командной строке:

```
-INF_FUNC_HASH <строчка_с_названиям_хэш-функции_для_LaTex> <файл_для_сохранения_результатов_анализа_работы_хэш-функции.tex>
```

Например,
```
-INF_FUNC_HASH hash\\_crc32 ./inf_about_hash_funcs/hash_crc32.tex
```

Теперь мы можем проанализировать хэш-функции.

## Выбор хэш-функции

Приведём результаты анализа каждой хэш-функции

- hash_sum_ascii:

<div style="text-align:center">
    <img src="./pictures/hash sum ascii.png#center" alt="Результат анализа hash_sum_ascii">
</div>

- hash_sum_of_squares: 

<div style="text-align:center">
    <img src="./pictures/hash sum of squares.png#center" alt="Результат анализа hash_sum_of_squares">
</div>


- hash_djb2: 

<div style="text-align:center">
    <img src="./pictures/hash djb2.png#center" alt="Результат анализа hash djb2">
</div>

- hash_ascii_first_symbol:

<div style="text-align:center">
    <img src="./pictures/hash ascii first symbol.png#center" alt="Результат анализа hash ascii first symbol">
</div>

- hash_mul_all_ascii: 

<div style="text-align:center">
    <img src="./pictures/hash mul all ascii.png#center" alt="Результат анализа hash mul all ascii">
</div>

- hash_average:

<div style="text-align:center">
    <img src="./pictures/hash average.png#center" alt="Результат анализа hash average">
</div>

- hash_crc32:

<div style="text-align:center">
    <img src="./pictures/hash crc32.png#center" alt="Результат анализа hash crc32">
</div>

Чтобы определить наилучшую хэш-функцию, нужно для каждого распределения определить дисперсию. Функция с наименьшей дисперсией будет наилучшей для данных слов. Мы же оценим хэш-функции только качественно: распределение должно быть без резких скачков. Тогда наилучшей функцией является *hash_djb2*. И, вообще говоря, мы должны выбрать её. Но мы специально выберем *hash_crc32*, тк в x86-64 есть команда *crc32*, а значит, мы сможем произвести оптимизацию данной функции при помощи *asm* (об оптимизациях смотреть ниже). Это решение принимается специально в качестве упражнения, как и специально увеличенный *load_factor*. Мы осознаём, что в прикладных задач такие решения принимать нельзя.

<details>
<summary>Хэш-функция crc32</summary>

```C

const __u_int POLYNOMIAL   		         = 0x04C11DB7;
const __u_int MASK_HIGH_BIT_IN_REGISTER  = 0x80000000;
const __u_int MASK_FIRST_BIT             = 0x00000001;
const char    MASK_HIGH_BIT_BYTE         = (char) 0x80;
const size_t  COUNT_SHIFTING_BITS        = 1;
const size_t  BITS_IN_BYTE               = 8;

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

```
</details>

## Поиск элементов: отличие RELEASE от TEST; методика измерения производительности

Итак, мы выбрали хэш-функцию и заполнили хэш-таблицу. Следующий этап поиск элементов. Для поиска нужно передать параметром командной строки файл, который будет содержать столбик со словами для поиска. Для передачи файла со словами для поиска в командной строке необходимо ввести:
```
-find <файл_со_словами_для_поиска>
```

Полученные слова для поиска копируются в память и потом сравниваются с элементами в хэш-таблице, как было описано выше.

Если программу скомпилировать:
```
make MODE=RELEASE
```
то функция поиска будет осуществляться в один проход, пок не произойдёт перебор всех слов для поиска.

<details>
<summary>Функция поиска элементов при MODE=RELEASE</summary>

```C

const size_t COUNT_BUCKETS        = 1024; 
const size_t SIZE_CASH_WITH_WORDS = 4;

struct inf_hash_table_t
{
	size_t (*func_hash) (char*);
	list_t hash_table[COUNT_BUCKETS];
};

struct inf_for_search_t
{
	char** words_for_search;
	size_t count_words;
};

static errors_in_hash_table_t find_words_in_hash_table (inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
{
	assert (inf_hash_table.hash_table);
	assert (inf_hash_table.func_hash);
	assert (inf_for_search.words_for_search);

	size_t  count_words      = inf_for_search.count_words;
	size_t  index_bucket     = 0;
	char*   word             = NULL;
	char**  words_for_search = inf_for_search.words_for_search;
	list_t* hash_table       = inf_hash_table.hash_table;
	
	size_t (*func_hash) (char*) = inf_hash_table.func_hash;

	cash_t 			   cash_with_words  = {};
	element_in_cash_t  word_from_cash   = {};

	//--------------------------------------------------------------------------

	size_t max_index_word = count_words;

	for (size_t index_word = 0; index_word < max_index_word; index_word++)
	{
		word = words_for_search[index_word];

		search_in_cash_t status_cash = search_element_in_cash (cash_with_words, word, &word_from_cash);
		if (status_cash)
		{
			
			printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n"
					"counter_word == %ld\n-----------------------------------------\n\n", 
					word, word_from_cash.index_bucket, word_from_cash.count_words_in_text);

			continue;
		}

		index_bucket = func_hash (word);

		printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n", 
				word, index_bucket);				

		size_t counter_word = find_element_in_list (hash_table + index_bucket, word);

		printf ("counter_word == %ld\n-----------------------------------------\n\n", counter_word);

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

	return NOT_ERROR;
}

```
</details>

Особенным место в нашем поиске является небольшего кэша, в котором хранится информация и последних *SIZE_CASH_WITH_WORDS* словах, обработанных поиском. Кэш позволяет при попадании не вычислять ключ для слова и не искать это слове в хэш-таблице. Сама проверка кэша осуществляется линейным поиском. Если произойдет промах, то результаты поиска будут записаны в кэш на место самого старого элемента в нём. 

<details>
<summary>Функция проверки кэша</summary>

```C

enum search_in_cash_t
{
	NOT_FIND_IN_CASH = 0,
	FIND_IN_CASH     = 1
};

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


```
</details>

Если программу скомпилировать:
```
make MODE=TEST
```
то запустится измерение производительности поиска элементов в хэш_таблице. В этом режиме отключены все *printf* , выводящую информацию с результатами поиска слова, отключены все флаги проверки кода и *sanitizer*, выставлен -DNDEBUG, уровень оптимизаций компилятора выставлен на *-O0*. Его нужно будет поменять на *-O3* единственный раз при измерении производительности программы с *-O3*. Также выставлен *-fno-inline*, который запрещает компилятору производить *inline* функций. Позже это поможет профилировщику измерять долю времени работы только самих функций во время выполнения программы.

<details>
<summary>Функция поиска элементов при MODE=TEST</summary>

```C

const size_t COUNT_BUCKETS        = 1024;    
const size_t COUNT_REPEATING      = 15;
const size_t SIZE_CASH_WITH_WORDS = 4;

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

struct inf_hash_table_t
{
	size_t (*func_hash) (char*);
	list_t hash_table[COUNT_BUCKETS];
};

struct inf_for_search_t
{
	char** words_for_search;
	size_t count_words;
};

static errors_in_hash_table_t find_words_in_hash_table (int argc, char** argv, inf_hash_table_t inf_hash_table, inf_for_search_t inf_for_search)
{
	assert (inf_hash_table.hash_table);
	assert (inf_hash_table.func_hash);
	assert (inf_for_search.words_for_search);

	assert (argv);

	size_t  count_words      = inf_for_search.count_words;
	size_t  index_bucket     = 0;
	char*   word             = NULL;
	char**  words_for_search = inf_for_search.words_for_search;
	list_t* hash_table       = inf_hash_table.hash_table;
	
	size_t (*func_hash) (char*) = inf_hash_table.func_hash;

	cash_t 			   cash_with_words  = {};
	element_in_cash_t  word_from_cash   = {};

	//--------------------------------------------------------------------------

	size_t max_index_word = count_words;

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

			for (size_t index_word = 0; index_word < max_index_word; index_word++)
			{
				word = words_for_search[index_word];

				search_in_cash_t status_cash = search_element_in_cash (cash_with_words, word, &word_from_cash);
				if (status_cash)
					continue;
				
				index_bucket = func_hash (word);

				size_t counter_word = find_element_in_list (hash_table + index_bucket, word);

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

			size_t ticks_after_cycle = __rdtsc();

			size_t ticks = ticks_after_cycle - ticks_before_cycle;

			(tests[index_cpe]).ticks      = ticks;
			(tests[index_cpe]).iterations = iterations;
			index_cpe++;
		}
	}

	#ifdef PRINT_CPE_RESULT
	print_cpe_result   (cpe_result);
	#endif

	analyze_cpe_result (argc, argv, cpe_result);
	
	free (tests);

	return NOT_ERROR;
}

```
</details>

Измерение производительности поиска элементов в хэш-таблице будем проводить при помощи измерения CPE (Cycles Per Element) - количество тактов синхронизации процессора необходимых на обработку одной итерации цикла (поиск одного слова). Телом цикла будет поиск элемента в хэш-таблица. Чем больше слов мы будем искать в цикле, тем больше тактов синхронизации процессора потребуется на выполнение всего цикла. Причём эта зависимость будет близка к линейной. Поэтому можно измерить количество тактов синхронизации процессора необходимых для поиска n слов (n = 1, 2, 3, ..., количество_слов_для_поиска), записать данные: количество тактов синхронизации и количество слов для поиска, нанести эти данные на график такты от количества слов, по МНК провести наилучшую прямую и по её угловому коэффициенту определить количество тактов синхронизации процессора необходимых на обработку одной итерации цикла (поиск одного слова), те определить CPE. Чем меньше будет CPE, тем выше будет производительность поиска элементов в кэш-таблице. При построение нужно будет учесть выбросы, которые будут возникать из-за инициализации и завершения цикла.

Сначала будет искаться первое слово. Потом первое и второе. Далее первое, второе и третье. И тд. В конце будет поиск всех слов. Эти действия будут произведены *COUNT_REPEATING* раз, чтобы каждому поиску n-ого количества слов соответствовали несколько значений синхронизации процессора, необходимых на выполнение поиска этих n слов. Все измерения фиксируются в *tests* , а также в *cpe_result*. После завершения измерений функция *analyze_cpe_result* (её можно в файлах [cpe.h](https://github.com/Alexandr26Starostin/hash_table/blob/master/include/cpe.h) и [cpe.cpp](https://github.com/Alexandr26Starostin/hash_table/blob/master/src/cpe.cppc))сгенерирует файл на python, при запуске которого мы получим график и значение CPE со случайной погрешностью от МНК.

Программе через командную строку нужно передать файл, в которой будет записан код на python:

```
-plot plot.py
```

Для построения графика и вычисления CPE нужно выполнить:
```
python plot.py
```

Чтобы было проще понять метод измерения производительности поиска слов, ниже изображен краткий код с основными действиями при измерении:

```C
for (size_t index_repeating = 0; index_repeating < COUNT_REPEATING; index_repeating++)
{  
	for (size_t iterations = 1; iterations <= count_words; iterations++)
	{
		size_t max_index_word = iterations;

		size_t ticks_before_cycle = __rdtsc();

		for (size_t index_word = 0; index_word < max_index_word; index_word++)
		{
			<Поиск слова в хэш-таблице>
		}

		size_t ticks_after_cycle = __rdtsc();

		size_t ticks = ticks_after_cycle - ticks_before_cycle;

		(tests[index_cpe]).ticks      = ticks;
		(tests[index_cpe]).iterations = iterations;
		index_cpe++;
	}
}	
```

Измерение тактов синхронизации процессора выполняется при помощи *__rdtsc*. Эта функция возвращает количество тактов сихронизации с момента последнего сброса процессора. Во время выполнения программы процессор не сбрасывается, тк нештатные ситуации не происходят. Чтобы использовать эту функцию, нужно подключить библиотеку:

```C
#include <x86intrin.h>
```

Функция *__rdtsc* имеет свою приборную погрешность, которую нам нужно определить.

Кроме того, во время выполнения измерений нужно, чтобы программа выполнялась на одном ядре и была привязана к этому ядру. Этого можно добиться так:
```
taskset -c 0 <команда> [аргументы]
```

Например:
```
taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32  -find ./words_for_search.txt -plot plot.py
```

Так же все измерения должны приходить в одинаковых условиях: уровень зарядки, температура окружающего воздуха, другие активные процессы, положение компьютера, подключенные устройства, уровень яркости и громкости и тд. Заметим, что тротлинг при выполнении программы не будет возникать, тк ядро не будет перегрето за время работы программы.

## Измерение приборной погрешности *__rdtsc*

Напишем программу, которая позволит оценить приборную погрешность *__rdtsc*:

```C
#include <stdio.h>
#include <x86intrin.h>

const size_t MAX_ITERATION = 20;

enum error_in_rdtsc_error_t
{
	NOT_ERROR = 0
};

int main ()
{
	size_t ticks_before = 0;
	size_t ticks_after  = 0;

	printf ("iteration    rdtsc_error\n");

	for (size_t iteration = 1; iteration <= MAX_ITERATION; iteration++)
	{
		ticks_before = __rdtsc ();
		ticks_after  = __rdtsc ();

		printf ("%9ld    %ld\n", iteration, ticks_after - ticks_before);
	}

	return NOT_ERROR;
}
```

Компилируем и запускаем (на одном ядре через *taskset -c 0*). Получаем список данных:
```
iteration    rdtsc_error
        1    46
        2    40
        3    36
        4    38
        5    36
        6    36
        7    36
        8    38
        9    38
       10    40
       11    36
       12    38
       13    40
       14    36
       15    40
       16    38
       17    36
       18    40
       19    40
       20    38
```

По правой колонке мы сможем определить приборную погрешность:

Среднее значение:

$$\overline{x} = \frac{1}{N} \sum_{i = 1}^N x_i$$
$$\overline{x} = 38$$

Стандартное отклонение:

$$\sigma = \sqrt{\frac{1}{N} \sum_{i = 1}^N (x_i - \overline{x})^2}$$
$$\sigma = 2.53$$

Стандартная ошибка среднего:

$$SE = \frac{\sigma}{\sqrt{N}}$$
$$SE = 0.57$$

Получаем:

$$\text{rdtsc\_error} = 38.0 \pm 0.6$$

Значит приборная погрешность *__rdtsc* составляет 38 тактов сихронизации процессора.

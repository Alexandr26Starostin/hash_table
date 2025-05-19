# hash_table

## Самое важное

Самое важное, что будет в этом проекте:

- знакомство с хэш-таблицей,
- написание hash функции crc32,
- анализ hash функций на коллизию,
- проведение измерения и анализа производительности программы,
- использование профилировщика для выявления неэффективных мест в программе,
- проведение оптимизаций программы.

## О чём этот проект

Хэш-таблица - это структура данных, которая позволяет быстро находить хранящиеся в ней данные. Каждому элементу в хэш-таблице соответствует свой хэш. Ключом для хэш-таблицы является элемент, который мы хотим в ней найти. Если нам нужно найти ключ в хэш-таблице, то достаточно узнать хэш ключа и по нему найти элемент и информацию о нём в хэш-таблице. Другими словами, хэш-таблица - структура данных, отображающая множество ключей в множество значений.

В данном проекте мы напишем свою хэш-таблицу и будем использовать её для поиска слов. После этого протестируем программу и оптимизируем неэффективные участки.

Проект написан на arch-Linux, процессор Intel N95, используется 15.4 Гб оперативной памяти.

## Использование программы

### Сборка и использование программы в режиме RELEASE

Для использования данной программы необходимо:
1. Скопировать себе репозиторий:

```bash
git clone https://github.com/Alexandr26Starostin/my_pritf.git
```

2. Перейти в директорию с программой для подготовки слов для записи в хэш-таблицу:

```bash
./prepare_file_with_words
```

3. Скомпилировать программу с подготовкой слов:
```bash
make
```

4. Выполнить подготовку слов:
```bash
./prepare_file -w ../file_with_words.txt -r ../Five_Weeks_in_a_Balloon.txt
```
> -w ../file_with_words.txt - файл с подготовленными для записи словами

> -r ../Five_Weeks_in_a_Balloon.txt - исходный текст

Данная программа запишет слова из исходного текста в столбик, подготовив их к записи в хэш-таблицу.

5. Вернуться в директорию с хэш-таблицей:
```bash
../
```

6. Скомпилировать программу с хэш-таблицей:
```bash
make MODE=RELEASE
```

В этой версии компиляции отключены все флаги контроля кода, отключён *sanitizer*, выставлен -DNDEBUG. Уровень оптимизаций по умолчанию стоит *-O3*. Его можно изменить вручную, если нужно. Этот режим используется в итоговой версии программы, и именного его мы будем использовать при профилировании программы.  

Программа с хэш-таблицей имеет несколько версий компиляции. Они будут рассказаны далее.

7. Вызвать программу:
```bash
./hash_table ./file_with_words.txt -hash hash_crc32 -find ./words_for_search.txt
```

> ./hash_table - вызов исполняемого файла (обяз.)

> ./file_with_words.txt - файл со словами для заполнения хэш-таблицы (всегда первый параметр) (обяз.)

> -hash hash_crc32 - выбор хэш-функции (обяз.)

>-find ./words_for_search.txt - файл со словами для поиска (слова должны быть записаны в столбик) (обяз.)

Программа с хэш-таблицей может иметь дополнительные параметры, о которых будет сказано далее.

### Режим DEBUG

Если программу с хэш-таблицей скомпилировать:
```bash
make MODE=DEBUG
```

то будут включены *assert*, все флаги проверки кода, все *printf*, связанные с результатами работы программы (результаты поисков слов), будет включен *sanitizer*, уровень оптимизаций будет выставлен на *-O0*. Этот режим можно использовать при поиске ошибок и внесении изменений в код программы.

### Дополнительные флаги компиляции

Ниже приведён список дополнительных флагов компиляции, которые можно прописать в *Makefile* в разделе *DEFINES=*:

- **-DPRINT_TEXT_FROM_FILE** - печатать тексты, которые получит программа, со словами для заполнения и поиска,
- **-DPRINT_INF_ABOUT_HASH_FUNC** - собрать информацию о распределении хэш-функции (будет описано ниже),
- **-DINTRINSICS** - включает опитимизацию на Intrinsics (будет описано ниже),
- **-DINLINE_ASM** - включает опитимизацию с помощью inline asm в C (будет описано ниже),
- **-DTEST_PROGRAM** - Запусакает измерение производительности программы (используется в режиме TEST, будет описано ниже), 
- **-DDEBUG** - печатает результаты поиска слов (используется в режиме DEBUG).

### Коды возврата ошибок

При выполнении программа возвращает код, описывающий результат произошедшей работы. Ниже приведён список таких кодов.

- **NOT_ERROR                         = 0** - программа завершилась без ошибок и волнила все свои процессы,
- **HAVE_NOT_MEMORY_FOR_FIRST_ELEMENT = 1** - ошибка выделения динамической памяти для первого элемента в списке при его инициализации,
- **NOT_MEMORY_FOR_ELEMENT            = 2** - ошибка выделения динамической памяти для элемента при его добавлении в список,
- **TRY_TO_GET_VALUE_OUTSIDE_LIST     = 3** - провал попытки обратиться к элменту из списка по его индексу из-за выхода за границы списка,
- **CANNOT_OPEN_FILE                  = 4** - ошибка при открытии файла,
- **CANNOT_CLOSE_FILE                 = 5** - ошибка при закрытии файла,
- **FAILED_CTOR_LIST                  = 6** - ошибка при инициализации списка,
- **FAILED_STRNCPY                    = 7** - ошибка при вызову strncpy, 
- **NOT_FIND_FLAG_HASH                = 8** - пользователь не указал хэш-функцию, которую должна использовать программа, 
- **NOT_DEFINED_NAME_FUNC_HASH        = 9** - пользователь указал имя хэш-функции, неизвестное программе (список известных хэш-функций будет приведён ниже), 
- **NOT_FIND_FLAG_INF_FUNC_HASH       = 10** - не указаны параметры, нужные для сбора информации о работн хэш-функции (только при флаге -DPRINT_INF_ABOUT_HASH_FUNC),
- **NOT_FIND_FLAG_FIND                = 11** - не указан файл со словами для поиска,
- **NOT_MEMORY_FROM_CALLOC            = 12** - ошибка выделения динамической памяти,
- **ERROR_IN_FREAD                    = 13** - ошибка при вызове fread,
- **ERROR_IN_REALLOC                  = 14** - ошибка при вызове realloc,
- **NOT_FIND_FLAG_PLOT                = 15** - не указан файл, в которой нужно записать код для построения графика (В режиме TEST, будет описано ниже).

Все ошибки совпровождаются соббщениями, почему и где произошла ошибка.

## Подробнее о хэш-таблицах

Как уже было сказано ранее, хэш-таблица - структура данных, отображающая множество ключей в множество значений. Каждому элементу, как и ключу, соответствует свой хэш. Этот хэш можно получить при помощи какой-либо хэш-функции. Хэш-функция - это функция, которая для каждого элемента высчитывает определённым способом число, которое называют хэшем. Однако хэш-функция может вызвать коллизию. Коллизия - это соответствие нескольким различным элементам одинаковых хэшей. Коллизия нарушает однозначность соответствия хэш - элемент и ухудшает поиск ключа в хэш-таблице. Поэтому хэш-таблицы бывают разными в зависимости от способа борьбы с коллизией. Например, бывают хэш-таблицы с цепочками и с открытой адресацией. Первая использует списки для хранения элементов (подробнее ниже). Вторая - массив из структур элемент-хэш и поиск свободной ячейки в этом массиве с помощью последовательности функций, принимающих хэш и возвращаемых значение адреса возможно свободной ячейки.

В нашем проекте используется хэш-таблица с цепочками. Сама хэш-таблица - массив указателей на бакеты: односвязные списки (о списках см. ниже). Хэш является индексом списка (бакета) в этом массиве. Когда мы хотим добавить элемент в хэш-таблица, хэш-функция для элемента вычисляет хэш, этот хэш становится индексом списка, в который нужно будет добавить элемент. При поиске ключа выполняется те же действия, но теперь хэш - индекс списка, в котором нужно искать наш ключ. Ниже представлена схема списка с цепочками (схема взята с [этого](https://neerc.ifmo.ru/wiki/index.php?title=%D0%A0%D0%B0%D0%B7%D1%80%D0%B5%D1%88%D0%B5%D0%BD%D0%B8%D0%B5_%D0%BA%D0%BE%D0%BB%D0%BB%D0%B8%D0%B7%D0%B8%D0%B9) источника):


<div style="text-align:center">
    <img src="./pictures/hash_table.png#center" alt="Схема хэш-таблицы с цепочками">
	<div align="center">
	<p>Схема хэш-таблицы с цепочками</p>
	</div>
</div>

<br>

Списки же могут быть, например, односвязными, двусвязными, кольцевыми, friendly список и тд. В нашем проекте мы используем односвязные списки. В них каждый элемент списка указывает на следующий за ним. Последний элемент (*tail*) указывает на *null* . При добавлении слова в список будет осуществлён линейный поиск по списку, начиная с первого элемента в списке (*head*). Если добавляемый элемент будет найден, то произойдёт увеличения счётчика, равного количеству, сколько раз данное слово встретилось в тексте. Если же элемент не будет найден, то он будет добавлен в конец список, и счётчик будет равен *1*. При поиске будет возвращаться значение счётчика, если элемент найден в списке, или *0*, если нет. Ниже представлена схема односвязного списка (схема взята с [этого](https://prog-cpp.ru/data-ols-oop/) источника):

<div style="text-align:center">
    <img src="./pictures/list.png#center" alt="Схема односвязного списка">
	<p>Схема односвязного списка</p>
</div>

<br>

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

<br>

Элементами списков являются указатели на слова, лежащие в динамической памяти:

<details>
<summary>Фрагмент кода, выполняющий добавление элемента в список</summary>

```C
node_t* element = (node_t*) calloc (COUNT_NODES_IN_CALLOC, sizeof (node_t));
if (element == NULL)
{
	printf ("Error in %s:%d\nHave not memory for element in try to add element (%s) in list\n\n", __FILE__, __LINE__, data);
	return NOT_MEMORY_FOR_ELEMENT;
}

element -> next_element    = NULL;
element -> counter_element = MIN_COUNTER_ELEMENT;
element -> data            = data;

(ptr_list -> tail) -> next_element = element;

ptr_list -> tail = element;

(ptr_list -> size_of_list)++;
``` 
</details>

<br>

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

- hash_ascii_first_symbol - ascii первой буквы в слове,
- hash_sum_ascii - сумма ascii всех символов в слове,
- hash_sum_of_squares - сумма квадратов ascii всех символов в слове,
- hash_mul_all_ascii - произведение ascii всех символов в слове,
- hash_average - среднее значение ascii всех символов в слове,
- hash_djb2 - считает по алгоритму djb2 (о djb2 можно прочитать [здесь](http://www.cse.yorku.ca/~oz/hash.html)),
- hash_crc32 - считает по алгоритму crc32 (о crc32 можно прочитать [здесь](https://soltau.ru/index.php/themes/dev/item/461-kak-poschitat-kontrolnuyu-summu-crc32-crc16-crc8)).

Все функции возвращают значение меньшее, чем количество бакетов в хэш_таблице (это значение равно индексу бакета).

Хэш-функцию, которая будет работать в программе, мы обязаны передать параметром командной строки с помощью: 

```bash
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

Чтобы заполнить хэш-таблицу, мы последовательно положим все слова из ранее подготовленного текста в память так, чтобы размер всех слов был одинаковым (в нашем случае длина каждого слова равна 32 байта с учётом последнего '\0'), вычислим для каждого с помощью хэш-функции индекс бакета и положим в определённый по индексу бакет слово (копированием указателя на слово). После заполнения хэш-таблицы мы можем проанализировать хэш-функции. Чтобы это сделать, мы должны в [*Makefile*](https://github.com/Alexandr26Starostin/hash_table/blob/master/Makefile) для хэш-таблицы прописать:

```bash
DEFINES=-DPRINT_INF_ABOUT_HASH_FUNC
```

и при вызове передать в командной строке:

```bash
-INF_FUNC_HASH <строчка_с_названиям_хэш-функции_для_LaTex> <файл_для_сохранения_результатов_анализа_работы_хэш-функции.tex>
```

Например,
```bash
-INF_FUNC_HASH hash\\_crc32 ./inf_about_hash_funcs/hash_crc32.tex
```

Теперь мы можем проанализировать хэш-функции.

## Выбор хэш-функции

Приведём код и результаты анализа каждой хэш-функции

<details>
<summary>hash_ascii_first_symbol:</summary>

```C
size_t hash_ascii_first_symbol (char* str)
{
	assert (str);

	return (size_t) ((*str) % COUNT_BUCKETS);
}


```
</details>

<div style="text-align:center">
    <img src="./pictures/hash_ascii_first_symbol.png#center" alt="Результат анализа hash_ascii_first_symbol при load_factor = 10 - 15">
	<p>Результат анализа hash_ascii_first_symbol при   load_factor = 10 - 15</p>
</div>
<br>
<div style="text-align:center">
    <img src="./pictures/hash_ascii_first_symbol_red.png#center" alt="Результат анализа hash_ascii_first_symbol при load_factor = 1.5 - 2">
	<p>Результат анализа hash_ascii_first_symbol при   load_factor = 1.5 - 2</p>
</div>

<br>

<details>
<summary>hash_sum_ascii:</summary>

```C
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


```
</details>

<div style="text-align:center">
    <img src="./pictures/hash_sum_ascii.png#center" alt="Результат анализа hash_sum_ascii при load_factor = 10 - 15">
	<p>Результат анализа hash_sum_ascii при   load_factor = 10 - 15</p>S
</div>

<br>

<div style="text-align:center">
    <img src="./pictures/hash_sum_ascii_red.png#center" alt="Результат анализа hash_sum_ascii при load_factor = 1.5 - 2">
	<p>Результат анализа hash_sum_ascii при   load_factor = 1.5 - 2</p>S
</div>

<br>

<details>
<summary>hash_sum_of_squares:</summary>

```C
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


```
</details>

<div style="text-align:center">
    <img src="./pictures/hash_sum_of_squares.png#center" alt="Результат анализа hash_sum_of_squares при load_factor = 10 - 15">
	<p>Результат анализа hash_sum_of_squares при   load_factor = 10 - 15</p>
</div>

<br>

<div style="text-align:center">
    <img src="./pictures/hash_sum_of_squares_red.png#center" alt="Результат анализа hash_sum_of_squares при load_factor = 1.5 - 2">
	<p>Результат анализа hash_sum_of_squares при   load_factor = 1.5 - 2</p>
</div>

<br>

<details>
<summary>hash_mul_all_ascii:</summary>

```C
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


```
</details>

<div style="text-align:center">
    <img src="./pictures/hash_mul_all_ascii.png#center" alt="Результат анализа hash_mul_all_ascii при load_factor = 10 - 15">
	<p>Результат анализа hash_mul_all_ascii при   load_factor = 10 - 15</p>
</div>

<br>

<div style="text-align:center">
    <img src="./pictures/hash_mul_all_ascii_red.png#center" alt="Результат анализа hash_mul_all_ascii при load_factor = 1.5 - 2">
	<p>Результат анализа hash_mul_all_ascii при   load_factor = 1.5 - 2</p>
</div>

<br>

<details>
<summary>hash_average:</summary>

```C
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


```
</details>


<div style="text-align:center">
    <img src="./pictures/hash_average.png#center" alt="Результат анализа hash_average при load_factor = 10 - 15">
	<p>Результат анализа hash_average при   load_factor = 10 - 15</p>
</div>


<br>

<div style="text-align:center">
    <img src="./pictures/hash_average_red.png#center" alt="Результат анализа hash_average при load_factor = 1.5 - 2">
	<p>Результат анализа hash_average при   load_factor = 1.5 - 2</p>
</div>


<br>


<details>
<summary>hash_djb2:</summary>

```C
size_t hash_djb2 (char* str)
{
	assert (str);

	// printf ("%s\n", str);
	// print_str_32_bytes (str);
	// getchar ();

	size_t hash   = BASE_NUMBER_DJB2,
		   symbol = 0;

	for (size_t byte = 0; (symbol = (size_t) str[byte]) != '\0'; byte++)
	{
		hash = ((hash << SHIFTING_IN_GFB2) + hash) ^ symbol;
	}

	return hash % COUNT_BUCKETS;
}


```
</details>

<div style="text-align:center">
    <img src="./pictures/hash_djb2.png#center" alt="Результат анализа hash_djb2 при load_factor = 10 - 15">
	<p>Результат анализа hash_djb2 при   load_factor = 10 - 15</p>
</div>


<br>

<div style="text-align:center">
    <img src="./pictures/hash_djb2_red.png#center" alt="Результат анализа hash_djb2 при load_factor = 1.5 - 2">
	<p>Результат анализа hash_djb2 при   load_factor = 1.5 - 2</p>
</div>


<br>

<details>
<summary>hash_crc32:</summary>

```C
const size_t CRC32_POLYNOMIAL = 0xEDB88320;
const size_t CRC32_INIT_CRC   = 0xFFFFFFFF;
const size_t CRC32_TABLE_SIZE = UCHAR_MAX + 1;
const size_t MASK_FOR_2_BYTES = 0xFF;

size_t TABLE_FOR_CRC32 [CRC32_TABLE_SIZE] = {};

errors_in_hash_table_t hash_crc32_gen_table (size_t* table) 
{
	assert (table);

	size_t crc = 0;

	for (size_t byte_value = 0; byte_value < CRC32_TABLE_SIZE; byte_value++) 
	{
		crc = byte_value;

		for (size_t bit_value = 0; bit_value < CHAR_BIT; bit_value++) 
		{
			if (crc & 1)
				crc = (crc >> 1) ^ CRC32_POLYNOMIAL;
			else
				crc >>= 1;
		}
		table[byte_value] = crc;
	}

	return NOT_ERROR;
}

size_t hash_crc32 (char* bytes) 
{
	assert (bytes);

	size_t size_in_bytes = MAX_LEN_WORD;

	size_t crc = CRC32_INIT_CRC;
	const u_char* u_byte = (const u_char*) bytes;

	extern size_t TABLE_FOR_CRC32[CRC32_TABLE_SIZE];

	for (size_t i = 0; i < size_in_bytes; i++)
		crc = (crc >> 8) ^ TABLE_FOR_CRC32[(crc ^ *(const size_t*)(u_byte++)) & MASK_FOR_2_BYTES];

 	return crc % COUNT_BUCKETS;
}


```
</details>

<div style="text-align:center">
    <img src="./pictures/hash_crc32.png#center" alt="Результат анализа hash_crc32 при load_factor = 10 - 15">
	<p>Результат анализа hash_crc32 при   load_factor = 10 - 15</p>
</div>
<br>

<div style="text-align:center">
    <img src="./pictures/hash_crc32_red.png#center" alt="Результат анализа hash_crc32 при load_factor = 1.5 - 2">
	<p>Результат анализа hash_crc32 при   load_factor = 1.5 - 2</p>
</div>
<br>

Чтобы определить наилучшую хэш-функцию, нужно для каждого распределения определить дисперсию. Функция с наименьшей дисперсией будет наилучшей для данных слов. Мы же оценим хэш-функции только качественно: распределение должно быть без резких скачков (причём как на большом, так и на маленьком *load_factor*; Именно поэтому для каждой функции приведены два графика. Синий - для большого *load_factor*. Красный - для маленького). 

Тогда наилучшей функцией является *hash_djb2* или *hash_crc32*. И, вообще говоря, мы должны выбрать одну из них при помощи сравнения дисперсий их распределений. Но мы специально выберем *hash_crc32*, тк она лучше ляжет на аппаратуру: в x86-64 есть команда *crc32*, а значит, мы сможем произвести оптимизацию данной функции при помощи *asm* (об оптимизациях смотреть ниже).

## Поиск элементов: отличие RELEASE от TEST, методика измерения производительности

### Поиск элементов в режиме RELEASE

Итак, мы выбрали хэш-функцию и заполнили хэш-таблицу. Следующий этап поиск элементов. Для поиска нужно передать параметром командной строки файл, который будет содержать столбик со словами для поиска. Для передачи файла со словами для поиска в командной строке необходимо ввести:
```
-find <файл_со_словами_для_поиска>
```

Полученные слова для поиска последовательно копируются в динамическую память так, что длина каждого слова составляет 32 байта с учётом '\0' (как и слова для заполнения), и потом сравниваются с элементами в хэш-таблице, как было описано выше.

Если программу скомпилировать:
```
make MODE=RELEASE
```
то функция поиска будет осуществляться COUNT_REPEATING проходов. За каждый проход она будет делать перебор всех слов для поиска.

<details>
<summary>Функция поиска элементов при MODE=RELEASE</summary>

```C

const size_t COUNT_BUCKETS        = 1024; 
const size_t SIZE_CASH_WITH_WORDS = 4;
const size_t COUNT_REPEATING      = 70000;      

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

	size_t max_index_word = count_words * MAX_BYTES_IN_WORD;

	for (size_t index_repeating = 0; index_repeating < COUNT_REPEATING; index_repeating++)
	{
		for (size_t index_word = 0; index_word < max_index_word; index_word += MAX_BYTES_IN_WORD)
		{
			word = words_for_search + index_word;

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
	}

	return NOT_ERROR;
}

```
</details>
<br>
Особенным место в нашем поиске является наличие небольшего кэша, в котором хранится информация и последних *SIZE_CASH_WITH_WORDS* словах, обработанных поиском. Кэш позволяет при попадании не вычислять ключ для слова и не искать это слове в хэш-таблице. Сама проверка кэша осуществляется линейным поиском. Если произойдет промах, то результаты поиска будут записаны в кэш на место самого "старого" элемента в нём. 

<br>
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


### Поиск элементов в режиме TEST

Если программу скомпилировать:
```
make MODE=TEST
```
то запустится измерение производительности поиска элементов в хэш_таблице. В этом режиме отключены все *printf* , выводящую информацию с результатами поиска слова, отключены все флаги проверки кода и *sanitizer*, выставлен -DNDEBUG, уровень оптимизаций компилятора выставлен на *-O3*. Посмотрим, как происходит измерение производительности программы.

<details>
<summary>Функция поиска элементов при MODE=TEST</summary>

```C

const size_t COUNT_BUCKETS        = 1024;    
const size_t COUNT_REPEATING      = 20;
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

	size_t max_index_word = 0;

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
			max_index_word = iterations * MAX_BYTES_IN_WORD;

			size_t ticks_before_cycle = __rdtsc();

			for (size_t index_word = 0; index_word < max_index_word; index_word += MAX_BYTES_IN_WORD)
			{
				word = words_for_search + index_word;

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
<br>

Измерение производительности поиска элементов в хэш-таблице будем проводить при помощи измерения CPE (Cycles Per Element) - количество тактов синхронизации процессора необходимых на обработку одной итерации цикла (поиск одного слова). Телом цикла будет поиск элемента в хэш-таблица. Чем больше слов мы будем искать в цикле, тем больше тактов синхронизации процессора потребуется на выполнение всего цикла. Причём эта зависимость будет близка к линейной. Поэтому можно измерить количество тактов синхронизации процессора необходимых для поиска n слов (n = 1, 2, 3, ..., количество_слов_для_поиска), записать данные: количество тактов синхронизации и количество слов для поиска, нанести эти данные на график такты от количества слов, по МНК провести наилучшую прямую и по её угловому коэффициенту определить количество тактов синхронизации процессора необходимых на обработку одной итерации цикла (поиск одного слова), те определить CPE. Чем меньше будет CPE, тем выше будет производительность поиска элементов в кэш-таблице. При построение нужно будет учесть выбросы, которые будут возникать из-за инициализации и завершения цикла.

Сначала будет искаться первое слово. Потом первое и второе. Далее первое, второе и третье. И тд. В конце будет поиск всех слов. Эти действия будут произведены *COUNT_REPEATING* раз, чтобы каждому поиску n-ого количества слов соответствовали несколько значений синхронизации процессора, необходимых на выполнение поиска этих n слов. Все измерения фиксируются в *tests* , а также в *cpe_result*. После завершения измерений функция *analyze_cpe_result* (её можно в файлах [cpe.h](https://github.com/Alexandr26Starostin/hash_table/blob/master/include/cpe.h) и [cpe.cpp](https://github.com/Alexandr26Starostin/hash_table/blob/master/src/cpe.cppc))сгенерирует файл на python, при запуске которого мы получим график и значение CPE со случайной погрешностью от МНК.

Программе через командную строку нужно передать файл, в которой будет записан код на python:

```bash
-plot plot.py
```

Для построения графика и вычисления CPE нужно выполнить:
```bash
python plot.py
```

Чтобы было проще понять метод измерения производительности поиска слов, ниже изображен краткий код с основными действиями при измерении:

```C
for (size_t index_repeating = 0; index_repeating < COUNT_REPEATING; index_repeating++)
{  
	for (size_t iterations = 1; iterations <= count_words; iterations++)
	{
		size_t max_index_word = iterations * MAX_BYTES_IN_WORD;

		size_t ticks_before_cycle = __rdtsc();

		for (size_t index_word = 0; index_word < max_index_word; index_word += MAX_BYTES_IN_WORD)
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
```bash
taskset -c 0 <команда> [аргументы]
```

Например:
```bash
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

$$\text{rdtscerror} = 38.0 \pm 0.6$$

Значит приборная погрешность *__rdtsc* составляет 38 тактов сихронизации процессора.

## Формула для CPE и учёт погрешностей

Получив точки с результатами измерения количества тактов синхронизации процессора от количества итераций в цикле, мы можем построить график, по МНК провести наилучшую прямую *y = kx + b* и определить CPE = *k*. Основные формулы:

Коэффициент наклона *k* (N - количество точек):

$$k = \frac{N\sum_{i=1}^N (x_i y_i) - \sum_{i=1}^N x_i \sum_{i=1}^N y_i}{N \sum_{i=1}^N x_i^2 - (\sum_{i=1}^N x_i)^2}$$

Свободный коэффициент *b*:

$$b = \frac{\sum_{i=1}^N y_i - k \sum_{i=1}^N x_i}{N}$$

Случайная погрешность *k*:

$$\sigma_{k\_случ} = \sqrt{\frac{\sum_{i=1}^N (y_i - (k x_i + b))^2}{(N-2) \sum_{i=1}^N (x_i - \overline{x})^2}}$$

Если приборная погрешность *x* равна  $\sigma_x$, а погрешность *y* равна  $\sigma_y$, то приборная погрешность *k*:

$$\sigma_{k\_приб} = \frac{\sqrt{N \sigma_y^2 + N k^2 \sigma_x^2}}{\sum_{i=1}^N (x_i -\overline{x})^2}$$

Полная погрешность *k*:

$$\sigma_{k} = \sqrt{(\sigma_{k\_случ})^2 + (\sigma_{k\_приб})^2}$$

В нашем случае:

$$CPE = k$$
$$\sigma_{CPE} = \sigma_k$$
$$\sigma_x = 0$$
$$\sigma_y = 38$$

Значения $k$ и $\sigma_{k\_случ}$ вычисялет программа на *plot.py*. Нам же останется вычислить $\sigma_{k\_приб}$ и $\sigma_k$.

Поскольку в измерениях $N$ = COUNT_BUCKETS = 1024, то $\sigma_{k\_приб} = 1.4 * 10^{-5} \approx 0$.

Значит:

$$\sigma_{k} \approx \sigma_{k\_случ}$$

## Измерение базовой версии

Чтобы выполнить измерение производительности поиска слов в базовой версии, уберём флаги оптимизации и выполним следующие команды:

```bash
make MODE=TEST
taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32 -find ./words_for_search.txt -plot plot.py
python plot.py
```

Получаем график:

<div style="text-align:center">
    <img src="./pictures/base_-O0.png#center" alt="Измерение CPE для базовой версии">
	<p>Измерение CPE для базовой версии</p>
</div>
<br>
<br>

Получем, что для базовой версии: СPE = $177.68 \pm 0.04$

<!---

<br>
<p style="text-align:center;">
	<font size="3">
		Тогда СPE = $199.8 \pm 38.0$
	</font>
</p>

-->

## Измерение изначальной версии с *-O3*

Включим флаг оптимизаций *-O3*, выполним те же действия и получим график:

<div style="text-align:center">
    <img src="./pictures/base_-O3.png#center" alt="Измерение CPE изначальной версии с *-O3*">
	<p>Измерение CPE для изначальной версии с *-O3*</p>
</div>
<br>
<br>

Значит, что для изначальной версии с *-O3*: СPE = $115.28 \pm 0.02$ и что *-О3* улучшило производительность поиска слов в базовой версии в 1.54 раза.

Во всех последующих измерениях и выполнениях программы флаг *-O3* будет установлен по умолчанию.

## Виды оптимизаций программы

Оптимизации программы бывают алгоритмическими (связаны с логикой действий или структурами, хранящими обрабатываемые данные), аппаратные (связанные с особеностью апаратуры, размером кэша, возможностями процессора; проявляются в необычных синтаксичсеских и логических действиях, причём сама логика программы не менятся) и ассемблерными (вствака ассемблера, inline asm, Intrinsics). В нашем проекте мы делаем упор на ассемблерные оптимизации. Поэтому мы постраемся улучшить производительность программы при помощи трёх оптимизаций: вствака ассемблера, inline asm и Intrinsics.

## Первая оптимизация: выявление неэффективного места и переписывание *hash_crc32* на NASM

Для нахождения неэффективности будем ипользовать *perf* и графическое окружение для него *hotspot*. Выполним команды:
```bash
make MODE=RELEASE
perf record -F 101 -g -o ./perf.data taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32 -find ./words_for_search.txt
hotspot perf.data
```

Мы увидим следующее:

<div style="text-align:center">
    <img src="./pictures/perf_opt_1.png#center" alt="Выявление неэффективных участков изначальной версии с -O3">
	<p>Выявление неэффективных участков изначальной версии с -O3</p>
</div>
<br>
<br>

*Hotspot* показывает процент времени занимаемой функцией от времени выполнения всей программы. По *Include* мы видим процент времени всей функции, включая все вызываемые в ней другие функции. По *Self* мы видим процент времени, которое занимает только сама функция (без учёта вызываемых в ней функций). Поэтому выявление неэффекивных мест мы будем производить по *Self*.

Значит, самой трудоёмкой функцией является *hash_crc32*, и нужно оптимизировать именно её.

Перепишем *hash_crc32* на NASM. Функцию *hash_crc32* на NASM можно найти в файле [hash_crc32_asm.asm](https://github.com/Alexandr26Starostin/hash_table/blob/master/src/hash_crc32_asm.asm). Эта оптимизация должна увеличить производительность программы, тк в x86-64 есть ассемблерная команда crc32 и есть вероятность, что она лучше ляжет на аппаратуру, чем наша хэш-функция *hash_crc32*.

<details>
<summary>сrc32 на NASM</summary>

``` asm

;---------------------------------------------------------------------------------------
;program on Linux-nasm-64
;has code with hash_crc32_asm                  
;---------------------------------------------------------------------------------------
section .text   ;has text with program
global hash_crc32_asm  ;global func: other files can see this func (for ld) 

;---------------------------------------------------------------------------------------------------------
;                                       hash_crc32_asm 
;calculate hash crc32 for 'char[32] str' and return it
;
;entry: rdi = ptr on str with len 32 bytes
;
;exit:  rax = remainder (hash) crc32
;
;destr: rax = result
;		rsi = mask_for: rax % 1024
;	
;must save:    rbp, rbx, r12-15
;mustn't save: others registers
;---------------------------------------------------------------------------------------------------------
hash_crc32_asm:

	xor eax, eax    ;rax = 0 - result

	crc32 rax, qword [rdi]        ;rdx = hash for 1 part element 
	crc32 rax, qword [rdi + 8]    ;rdx = hash for 2 part element 
	crc32 rax, qword [rdi + 16]   ;rdx = hash for 3 part element 
	crc32 rax, qword [rdi + 24]   ;rdx = hash for 4 part element 

	mov rsi, 01111111111b   ;rsi = 1024 - 1 = mask 
	and rax, rsi           ;rax = rax_old % 1024WWWWWW

	ret  ;rax = hash from crc32
;----------------------------------------------------------------------------------------------------------

``` 
</details>

<br>

Сделать inline функции hash_crc32_asm нельзя, тк в программе она вызывается, как и другие хэш-функции, по указателю. Поэтому будем использовать её без inline. 

<details>
<summary>Объявление hash_crc32_asm в C</summary>

``` С
extern "C" size_t hash_crc32_asm (char* str_for_crc32); 
``` 
</details>

<br>

Чтобы программа увидела *hash_crc32_asm*, нужно добавить в [*Makefile*](https://github.com/Alexandr26Starostin/hash_table/blob/master/Makefile):
```bash
FILES=main.o hash_crc32_asm.o ...

...

hash_crc32_asm.o: src/hash_crc32_asm.asm
	@nasm -f elf64 -l src/hash_crc32_asm.lst src/hash_crc32_asm.asm -o build/hash_crc32_asm.o

...

```

Выполним команды:

```bash
make MODE=TEST
taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32_asm -find ./words_for_search.txt -plot plot.py
python plot.py
```

Получаем график:

<div style="text-align:center">
    <img src="./pictures/crc32_asm.png#center" alt="Измерение CPE для версии с hash_crc32_asm">
	<p>Измерение CPE для версии с hash_crc32_asm</p>
</div>
<br>
<br>

Значит, что для версии с *crc32* на NASM: СPE = $45.20 \pm 0.01$

Получаем, что наша оптимизация увиличела производительность прошлой версии программы в 2.55 раза.

Дополнительно проанализируем распределение *hash_crc32_asm*, как мы это делали до этого. Анализ этой функции будет производить только для *load_factor* = 10 - 15, тк она написана для частного случая с COUNT_BUCKETS = 1024.

График распределения *hash_crc32_asm*:

<div style="text-align:center">
    <img src="./pictures/hash_crc32 asm.png#center" alt="Результат анализа hash_crc32_asm при load_factor = 10 - 15">
	<p>Результат анализа hash_crc32_asm при   load_factor = 10 - 15</p>
</div>


<br>

Качественно видно, что у функции *hash_crc32_asm* хорошее распределение.


## Вторая оптимизация: переписывание *compare_element* на Intrinsics

Продолжим делать оптимизации, не убирая первую.

Повторим профилирование:
```bash
make MODE=RELEASE
perf record -F 101 -g -o ./perf.data taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32_asm -find ./words_for_search.txt
hotspot perf.data
```

Получим картинку:

<div style="text-align:center">
    <img src="./pictures/perf_opt_2.png#center" alt="Выявление неэффективных участков версии с hash_crc32_asm">
	<p>Выявление неэффективных участков версии с hash_crc32_asm</p>
</div>
<br>
<br>




Как мы видим, теперь *compare_element* является самой трудоёмкой. Поэтому будем оптимизировать её.


Большинство слов, которые мы используем, имеют длину не больше 31 символов. Если найдётся слово длиной больше 31, то программа обрежёт его. Все слова можно хранить в массивах длиной 32, где 31 символ будет под само слово и последний, 32-ой символ будет под '\0'.  Если слово будет занимать меньше 31 символа, то оставшиеся байты будут заполнены '\0'. Такое решение позволит нам использовать xmm-регистры и, следовательно, Intrinsics-функции. Тогда мы сможем написать более эффективный *compare_element*, тк он будет за одну команду обрабатывать больше символов чем при их последовательном переборе и он будет лучше ложиться на аппаратуру. 

Единственной проблемой останется то, что все слова должны будут быть выровнены по 32 байтам. Это можно решить 2 способами: c помощью конвентора или функции *aligned_alloc*. 

В первом случае, конвентор - программа, которая файл со словами в столбике для заполнения превратит в бинарный файл, в котором каждое слово будет дополнено до длины в 32 байта с помощью '\0'. Тогда можно будет работать со словами для заполнения как с массивом из 32 батных элементов. Чтобы адрес этого массива был выровнен по 32 байта, нужно в начале один раз вызвать *aligned_alloc* с первым параметром = 32. Аналочно, поступаем со словами для поиска (обрабатываем их в конвенторе).

Во втором случае, память для всех последовательно расположенных слов (и для заполнения, и для поиска) придётся выделять не при помощи *calloc*, а при помощи *aligned_alloc* с первым параметром = 32 и после инициализировать все байты в этих словах '\0' (это может сделать функция *memset*).

В нашем проекте реализован второй подход.

После решения проблемы с выравниваем получаем новую вурсию *compare_element* через Intrinsics. Также мы можем сделать эту функцию inline, что тоже увеличит производительность программы.

<details>
<summary>Новая версия compare_element через Intrinsics</summary>

```C

#include <immintrin.h>

const int MASK_IF_ELEM_EQUAL = -1;

[[gnu::always_inline]] inline bool compare_element (data_t element_1, data_t element_2);   //data_t == char*

[[gnu::always_inline]] inline bool compare_element (data_t element_1, data_t element_2)
{
	//data_t == char[32]
	
	assert (element_1);
	assert (element_2);

	__m256i result_cmp = _mm256_cmpeq_epi64 (*(__m256i*) element_1, *(__m256i*) element_2);

	if (_mm256_movemask_epi8 (result_cmp) == MASK_IF_ELEM_EQUAL)
		return true;

	return false;
}


``` 
</details>
<br>

Чтобы запустить программу с *compare_element* на Intrinsics, нужно: 

- Подключить библиотеку:
```C
#include <immintrin.h>
```

- Прописать в [*Makefile*](https://github.com/Alexandr26Starostin/hash_table/blob/master/Makefile):
```bash
INTRINSICS=-mavx2 -msse -msse2
DEFINES=-DINTRINSICS
```

- Выполнить команды:
```bash
make MODE=TEST
taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32  -find ./words_for_search.txt -plot plot.py
python plot.py
```

Получаем график:

<div style="text-align:center">
    <img src="./pictures/intrinsics.png#center" alt="Измерение CPE для версии с compare_element на Intrinsics">
	<p>Измерение CPE для версии с compare_element на Intrinsics</p>
</div>
<br>
<br>

Значит, что для версии с *compare_element* на Intrinsics: СPE = $22.801 \pm 0.004$.

Как мы видим, наша оптимизация увеличила производительность поиска в предыдущей версии в 1.98 раза. 


## Третья оптимизация: использование inline asm в C при переписывании 

Продолжим делать оптимизации, не убирая первую и вторую.

Повторим профилирование:
```bash
make MODE=RELEASE
perf record -F 101 -g -o ./perf.data taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32_asm -find ./words_for_search.txt
hotspot perf.data
```

Получим картинку:

<div style="text-align:center">
    <img src="./pictures/perf_opt_3.png#center" alt="Выявление неэффективных участков версии с hash_crc32_asm и compare_element через Intrinsics">
	<p>Выявление неэффективных участков версии с hash_crc32_asm и compare_element через Intrinsics</p>
</div>
<br>
<br>

Заметим, что профилировщик не отображает функцию *compare_element*, тк она inline.

Теперь самой неэффективной является функция *find_words_in_hash_table*. Попробуем оптимизировать самый внутренний цикл этой программы с помощью его разворачивания и вставки inline asm в C для экономии регистров и уменьшения количества обращений в память.



<details>
<summary>Внутренний цикл, который мы попытаемся оптимизировать</summary>

```C

for (size_t index_word = 0; index_word < max_index_word; index_word += MAX_BYTES_IN_WORD)
{
	word = words_for_search + index_word;

	search_in_cash_t status_cash = search_element_in_cash (cash_with_words, word, &word_from_cash);
	if (status_cash)
	{
		#ifdef DEBUG
		printf ("-----------------------------------------\ninf about search word in cash: %s\n\nindex_bucket == %ld\n"
				"counter_word == %ld\n-----------------------------------------\n\n", 
				word, word_from_cash.index_bucket, word_from_cash.count_words_in_text);
		#endif

		continue;
	}

	index_bucket = func_hash (word);

	#ifdef DEBUG
	printf ("-----------------------------------------\ninf about search word: %s\n\nindex_bucket == %ld\n", word, index_bucket);				
	#endif

	size_t counter_word = find_element_in_list (hash_table + index_bucket, word);

	#ifdef DEBUG
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

``` 
</details>
<br>

Упростим данный фрагмент кода для нашего случая: режим RELEASE или TEST, использование хэш-функции *hash_crc32_asm*, убирание поиска элементов в cash, тк для наша хэш-функция *hash_crc32_asm* малозатратная (этим мы уменьшим количество обращений в память).   

<details>
<summary>Упрощённый цикл</summary>

```C

for (size_t index_word = 0; index_word < max_index_word; index_word += MAX_BYTES_IN_WORD)
{
	word = words_for_search + index_word;

	index_bucket = hash_crc32_asm (word);

	size_t counter_word = find_element_in_list (hash_table + index_bucket, word);
}

``` 
</details>
<br>

Теперь напишем версию с разворачиванием цикла на С:

<details>
<summary>Цикл с разворачиванием</summary>

```C

size_t max_index_word = count_words - count_words % 2;

char* word_1         = words_for_search;
char* word_2         = words_for_search + MAX_BYTES_IN_WORD;   
size_t step_in_cycle = MAX_BYTES_IN_WORD * 2;           //== 64

for (size_t index_word = 0; index_word < max_index_word; index_word += 2)
{
	size_t index_bucket_1 = hash_crc32_asm (word_1);
	size_t index_bucket_2 = hash_crc32_asm (word_2);

	find_element_in_list (hash_table + index_bucket_1, word_1);
	find_element_in_list (hash_table + index_bucket_2, word_2);

	word_1 += step_in_cycle;
	word_2 += step_in_cycle;
}

``` 
</details>
<br>

На следующем этапе осуществим саму ассмеблерную вставку:
<details>
<summary>Цикл с inline asm</summary>

```C
char* word_1 = words_for_search;
char* word_2 = words_for_search + MAX_BYTES_IN_WORD;


asm volatile (
	".intel_syntax noprefix\n\t"

	"push r8\n\t"
	"push r9\n\t"
	"push r10\n\t"
	"push r11\n\t"  //save others registers

	//--------------------------------------------------------------------

	"mov rcx, %[max_index]\n\t"   //rcx = max_index_word
	"mov r13, %[word_1]\n\t"      //r13 = word_1
	"mov r14, %[word_2]\n\t"      //r14 = word_2
	"mov r15, %[hash_table]\n\t"  //r15 = hash_table

	"find_next_two_elem:\n\t"

	//---------------------------------------------------------
	"push rcx\n\t"

		"mov rdi, r13\n\t"
		"call hash_crc32_asm\n\t"    //size_t index_bucket_1 = hash_crc32_asm (word_1);

		"mov rdx, 24\n\t"
		"mul rdx\n\t"
		"mov rdi, r15\n\t"
		"add rdi, rax\n\t"
		"mov rsi, r13\n\t"
		"call find_element_in_list\n\t"   //find_element_in_list (hash_table + index_bucket_1, word_1);

		//-------------------------------------------

		"mov rdi, r14\n\t"
		"call hash_crc32_asm\n\t"   //size_t index_bucket_2 = hash_crc32_asm (word_2);

		"mov rdx, 24\n\t"
		"mul rdx\n\t"
		"mov rdi, r15\n\t"
		"add rdi, rax\n\t"
		"mov rsi, r14\n\t"
		"call find_element_in_list\n\t"   //find_element_in_list (hash_table + index_bucket_2, word_2);
	
	"add r13, 64\n\t"     //word_1 += step_in_cycle;
	"add r14, 64\n\t"     //word_2 += step_in_cycle;

	"pop rcx\n\t"
	//---------------------------------------------------------

	"dec rcx\n\t"
	"loop find_next_two_elem\n\t"   //for (size_t index_word = max_index_word; index_word != 0; index_word -= 2) {}
	
	//--------------------------------------------------------------------

	"pop r11\n\t"
	"pop r10\n\t"
	"pop r9\n\t"
	"pop r8\n\t"   //save others registers

	//-----------------------------------------------------------------------------------------------------
	:
	:[max_index] "r" (max_index_word), 
	[word_1] "r" (word_1), 
	[word_2]  "r" (word_2),
	[hash_table] "r" (hash_table)
	:"rcx", "r13", "r14", "r15", "rax", "rdi", "rsi", "rdx", "cc", "memory"
	);


``` 
</details>
<br>

Чтобы скомпилировать функцию с inline asm в C, нужно в [*Makefile*](https://github.com/Alexandr26Starostin/hash_table/blob/master/Makefile) прописать:
```bash
DEFINES= ... -DINLINE_ASM  ...

INLINE_ASM=-masm=intel
```

Выполним команды:
```bash
make MODE=TEST
taskset -c 0 ./hash_table ./file_with_words.txt -hash hash_crc32_asm -find ./words_for_search.txt -plot plot.py
python plot.py
```

Получаем график:

<div style="text-align:center">
    <img src="./pictures/inline_asm.png#center" alt="Измерение CPE для версии с inline asm в C">
	<p>Измерение CPE для версии с inline asm в C</p>
</div>
<br>
<br>

Значит, что для версии с *find_words_in_hash_table* написанной при помощи inline asm в C: СPE = $8.406 \pm 0.003$

Получаем, что наша оптимизация увиличела производительность предыдущей версии в 2.71 раза.

Такое резкое увеличение производительности связано с сильным упрощением внутреннего цикла.

## Вывод

### Таблица с итогами 

Подведём итоги проделанной работы. Мы произвели ассемблерные оптимизации поиска слов в хэш-таблицы и получили улучшение производительности программы. Ниже приведена таблица с результатами измерений.

| Версия программы       | CPE для поиска элементов | Улучшение относительно изначальной версии (в n раз) | Улучшение относительно предыдущей версии (в n раз)  |
| ---------------------- | ------------------------ | --------------------------------------------------  | --------------------------------------------------- |
| Базовая с -О0          |     $177.68 \pm 0.04$    |                        $1.00$                       |                        $1.00$                       |
| Базовая с -О3          |     $115.28 \pm 0.02$    |                        $1.54$                       |                        $1.54$                       |
| hash_crc32_asm         |     $45.20  \pm 0.01$    |                        $3.93$                       |                        $2.55$                       |
| compare на Intrinsics  |     $22.801 \pm 0.004$   |                        $7.79$                       |                        $1.98$                       |
| Развёртка и inline asm |     $8.406  \pm 0.003$   |             $21.14$ (сильное упрощение программы)   |                        $2.71$                       |

### КПД

Важно отметить, что любое использование ассемблерных оптимизаций ухудшает читаемость, надёжность, переносимость и оптимизацию кода. Поэтому важно помнить, что нужно избегать ассемблерных оптимизаций. В качестве напоминания оценим КПД наших оптимизаций.

$$\text{КПД} = \frac{\text{Общее улучшение производительности программы}}{\text{Количество строк, написанных на ассмеблере и интрисиках}} * 1000$$
$$\text{КПД} = \frac{21.14}{63} * 100 \approx 33.5$$

У нас получился хороший КПД. Но так может быть не всегда. Поэтому нужно нужно стараться не прибегать к ассмеблерным оптимизациям.
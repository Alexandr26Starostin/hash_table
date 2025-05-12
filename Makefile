CC=g++
#DEBUG RELEASE
#MODE=DEBUG
#MODE=RELEASE
FILES=main.o hash_crc32_asm.o list.o hash_table.o func_hash.o text_and_files.o 
TARGET=hash_table
INTRINSICS=-mavx2 -msse -msse2
INLINE_ASM=-masm=intel

ifeq ($(MODE),RELEASE)
	FLAGS=-Iinclude -O0 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=40000 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -D_EJC
	SANITIZERS=-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
	PRINT_ALL=
	TEST=
else ifeq ($(MODE),PRINT_ALL)
	FLAGS=-Iinclude -O3 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=40000 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -D_EJC
	SANITIZERS=-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
	PRINT_ALL=-DPRINT_ALL
	TEST=
else ifeq ($(MODE),PRINT_INF_ABOUT_HASH_FUNC)
	FLAGS=-Iinclude -O3 -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=40000 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -D_EJC
	SANITIZERS=-fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fno-sanitize=null -fno-sanitize=alignment
	PRINT_ALL=-DPRINT_INF_ABOUT_HASH_FUNC
	TEST=
else ifeq ($(MODE),TEST)
	FLAGS=-Iinclude -DNDEBUG -O0 
	SANITIZERS=
	PRINT_ALL=
	TEST=-DTEST_PROGRAM
else
	FLAGS=-Iinclude -DNDEBUG -O0 
	SANITIZERS=
	PRINT_ALL=
	TEST=
endif

#SFML_TARGETS=sfml-app -lsfml-graphics -lsfml-window -lsfml-network -lsfml-system #-lsfml-audio
#g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

# pathsub
# addprefix
# wildcard

#nasm -f elf64 -l my_printf.lst my_printf.s
#g++ -static main.cpp my_printf.o  -o my_printf

COMPILATION=$(FLAGS) $(SANITIZERS) $(PRINT_ALL) $(TEST) $(INTRINSICS) $(INLINE_ASM)

PATCHED_FILES = $(addprefix build/, $(FILES))

all: prepare $(TARGET)

prepare:
	mkdir -p build

$(TARGET): $(FILES)
	@echo "Linling..."
	@$(CC) $(COMPILATION) $(PATCHED_FILES) -o $(TARGET) 
#-o $(SFML_TARGETS) 

main.o: src/main.cpp
	@$(CC) -c $(COMPILATION) src/main.cpp -o build/main.o

hash_crc32_asm.o: src/hash_crc32_asm.asm
	@nasm -f elf64 -l src/hash_crc32_asm.lst src/hash_crc32_asm.asm -o build/hash_crc32_asm.o

list.o: src/list.cpp
	@$(CC) -c $(COMPILATION) src/list.cpp -o build/list.o

hash_table.o: src/hash_table.cpp
	@$(CC) -c $(COMPILATION) src/hash_table.cpp -o build/hash_table.o  

func_hash.o: src/func_hash.cpp
	@$(CC) -c $(COMPILATION) src/func_hash.cpp -o build/func_hash.o

text_and_files.o: src/text_and_files.cpp
	@$(CC) -c $(COMPILATION) src/text_and_files.cpp -o build/text_and_files.o

clean:
	rm -rf $(TARGET)
	cd build && rm -rf *.o
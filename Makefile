CC=g++
FLAGS=-Iinclude -Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wmissing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE -D_EJC
INTRINSICS=-mavx2 -msse
FILES=main.o list.o hash_table.o func_hash.o
TARGET=hash_table
#SFML_TARGETS=sfml-app -lsfml-graphics -lsfml-window -lsfml-network -lsfml-system #-lsfml-audio

# pathsub
# addprefix
# wildcard
#g++ main.o -o sfml-app -lsfml-graphics -lsfml-window -lsfml-audio -lsfml-network -lsfml-system

PATCHED_FILES = $(addprefix build/, $(FILES))

all: prepare $(TARGET)

prepare:
	mkdir -p build

$(TARGET): $(FILES)
	@echo "Linling..."
	@$(CC) $(FLAGS) $(PATCHED_FILES) -o $(TARGET) 
#-o $(SFML_TARGETS) 

main.o: src/main.cpp
	@$(CC) -c $(FLAGS) src/main.cpp -o build/main.o

list.o: src/list.cpp
	@$(CC) -c $(FLAGS) src/list.cpp -o build/list.o

hash_table.o: src/hash_table.cpp
	@$(CC) -c $(FLAGS) src/hash_table.cpp -o build/hash_table.o

func_hash.o: src/func_hash.cpp
	@$(CC) -c $(FLAGS) src/func_hash.cpp -o build/func_hash.o

clean:
	rm -rf $(TARGET)
	cd build && rm -rf *.o
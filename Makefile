CC=gcc
CFLAGS=-Wall -Werror -g -std=gnu99
SRC=src/block_process.o src/system.o src/tinymt32.o
LIBS=-lcunit -lpthread -lm
INCLUDE_HEADERS_DIRECTORY=-Iheaders

threads_run: thread.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f thread
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o thread
	@./thread input_binary -n 4 -f output.txt

%.o: %.c
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ -c $<

all: thread.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f thread
	@rm -f fec
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o all
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 main.c $(SRC) $(LIBS) -o fec

fec: main.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f fec
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 main.c $(SRC) $(LIBS) -o fec

fec_threads: thread.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f thread
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o thread

run: main.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f fec
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 main.c $(SRC) $(LIBS) -o fec
	@./fec input_binary/ -f output.txt -v

valgrind_run: main.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f fec
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 main.c $(SRC) $(LIBS) -o fec
	@valgrind --leak-check=full --show-leak-kinds=all ./fec input_binary/ -f output.txt

valgrind_threads: thread.c src/block_process.o src/system.o src/tinymt32.o
	@rm -f fec
	@$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o thread
	@valgrind --leak-check=full -s --show-leak-kinds=all ./thread input_binary/ -n 4 -f output.txt

tests: tests/
	@$(CC) -O3 tests/test_tinymt32.c src/system.c src/tinymt32.c -lcunit -o tests/test_tinymt32
	./tests/test_tinymt32
	@$(CC) -O3 tests/test_make_linear_system.c src/system.c src/tinymt32.c -lcunit -o tests/test_make_linear_system
	./tests/test_make_linear_system
	@$(CC) -O3 tests/test_gen_coeffs.c src/system.c src/tinymt32.c -lcunit -o tests/test_gen_coeffs
	./tests/test_gen_coeffs
	@$(CC) -O3 tests/test_block_to_string.c src/system.c src/tinymt32.c -lcunit -o tests/test_block_to_string
	./tests/test_block_to_string
	@$(CC) -O3 tests/test_make_block.c src/system.c src/tinymt32.c -lcunit -o tests/test_make_block
	./tests/test_make_block
	@$(CC) -O3 tests/test_find_lost_words.c src/system.c src/tinymt32.c -lcunit -o tests/test_find_lost_words
	./tests/test_find_lost_words
	@$(CC) -O3 tests/test_write_block.c src/system.c src/tinymt32.c -lcunit -o tests/test_write_block
	./tests/test_write_block input_binary/ -f
	@$(CC) -O3 tests/test_write_last_block.c src/system.c src/tinymt32.c -lcunit -o tests/test_write_last_block
	./tests/test_write_last_block input_binary/ -f
	@$(CC) -O3 tests/test_gaussian.c src/system.c src/tinymt32.c -lcunit -o tests/test_gaussian
	./tests/test_gaussian
	@$(CC) -O3 tests/test_get_file_info.c src/system.c src/tinymt32.c -lcunit -o tests/test_get_file_info
	./tests/test_get_file_info
	@$(CC) -O3 tests/test_process_block.c src/system.c src/tinymt32.c -lcunit -o tests/test_process_block
	./tests/test_process_block

clean:
	@rm -f *.o
	@rm -f src/*.o
	@rm -f all
	@rm -f fec
	@rm -f thread
	@rm -f *.txt
	@rm -f tests/test_tinymt32
	@rm -f tests/test_make_linear_system
	@rm -f tests/test_block_to_string
	@rm -f tests/test_find_lost_words
	@rm -f tests/test_gaussian
	@rm -f tests/test_gen_coeffs
	@rm -f tests/test_get_file_info
	@rm -f tests/test_make_block
	@rm -f tests/test_process_block
	@rm -f tests/test_write_last_block
	@rm -f tests/test_write_block
	@rm -f tests/*.txt

help:
	@echo \> 'make help_cmd' : to see all the available commands
	@echo if you wish to run compiled code yourself :
	@echo \> './[name of executable] input_binary/' : is the base command and will print your output straight to terminal
	@echo here are the flags available to use after the base command :
	@echo '-v' \	\	\	\: to activate verbose mode and show steps of the program in your terminal
	@echo '-f [name of output].txt' \: to get your output in a .txt file
	@echo '-n [number of threads]' \	: to specify the number of threads you want to run the "thread" executable with

help_cmd:
	@echo \> 'make fec' \	\	\: compiles main.c with the name fec
	@echo \> 'make fec_threads' \	\: compiles thread.c with the name thread
	@echo \> 'make help'\	\	\: commands and flags to run compiled code yourself
	@echo \> 'make run' \	\	: compiles and runs main.c with the program output in output.txt and verbose mode activated
	@echo \> 'make threads_run' \	\: compiles and runs thread.c with the program output in output.txt and verbose mode activated
	@echo \> 'make valgrind_run' \	: compiles and runs main.c with Valgrind to check memory leaks
	@echo \> 'make valgrind_threads' \: compiles and runs thread.c with Valgrind to check memory leaks
	@echo \> 'make tests' \	\	: compiles and runs all tests with Cunit
	@echo \> 'make clean' \	\	: deletes all compiled files in the project


# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: clean tests

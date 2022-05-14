CC=gcc
CFLAGS=-Wall -Werror -g
SRC=src/block_process.c src/system.c src/tinymt32.c
LIBS=-lcunit -lpthread -lm
INCLUDE_HEADERS_DIRECTORY=-Iheaders

fec: main.c
	@rm -f fec
	@$(CC) -O3 main.c $(SRC) $(LIBS) -o fec

fec_threads: thread.c
	@rm -f thread
	@$(CC) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o thread

run: main.c
	@rm -f fec
	@$(CC) $(CFLAGS) -O3 main.c $(SRC) $(LIBS) -o fec
	@./fec input_binary/ -f output.txt -v

threads_run: thread.c
	@rm -f thread
	@$(CC) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o thread
	@./thread input_binary -n 4 -f output.txt

valgrind_run: main.c
	@rm -f fec
	@$(CC) $(CFLAGS) -O3 main.c $(SRC) $(LIBS) -o fec
	@valgrind --leak-check=full --show-leak-kinds=all ./fec input_binary/ -f output.txt

valgrind_threads: thread.c
	@rm -f fec
	@$(CC) $(CFLAGS) -O3 thread.c $(SRC) $(LIBS) -o thread
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
	@rm -f src/*.o
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

# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: clean tests

CC=gcc
CFLAGS=-Wall -Werror -g
LIBS=-lcunit -lpthread -lm
INCLUDE_HEADERS_DIRECTORY=-Iheaders

fec: main.c
	@$(CC) $(CFLAGS) -o fec main.c src/block_process.c src/system.c src/tinymt32.c $(LIBS)

run: main.c
	@$(CC) $(CFLAGS) -o fec main.c src/block_process.c src/system.c src/tinymt32.c $(LIBS)
	@./fec input_binary/ -f output.txt -v

valgrind: main.c
	@$(CC) $(CFLAGS) -o fec main.c src/block_process.c src/system.c src/tinymt32.c $(LIBS)
	@valgrind --leak-check=full ./fec input_binary/ -f output.txt

tests: tests/
	@$(CC) -o tests/test_tinymt32 tests/test_tinymt32.c src/system.c src/tinymt32.c -lcunit
	./tests/test_tinymt32
	@$(CC) -o tests/test_make_linear_system tests/test_make_linear_system.c src/system.c src/tinymt32.c -lcunit
	./tests/test_make_linear_system
	@$(CC) -o tests/test_gen_coeffs tests/test_gen_coeffs.c src/system.c src/tinymt32.c -lcunit
	./tests/test_gen_coeffs
	@$(CC) -o tests/test_block_to_string tests/test_block_to_string.c src/system.c src/tinymt32.c -lcunit
	./tests/test_block_to_string
	@$(CC) -o tests/test_make_block tests/test_make_block.c src/system.c src/tinymt32.c -lcunit
	./tests/test_make_block
	@$(CC) -o tests/test_find_lost_words tests/test_find_lost_words.c src/system.c src/tinymt32.c -lcunit
	./tests/test_find_lost_words
	@$(CC) -o tests/test_write_block tests/test_write_block.c src/system.c src/tinymt32.c -lcunit
	./tests/test_write_block input_binary/ -f
	@$(CC) -o tests/test_write_last_block tests/test_write_last_block.c src/system.c src/tinymt32.c -lcunit
	./tests/test_write_last_block input_binary/ -f
	@$(CC) -o tests/test_gaussian tests/test_gaussian.c src/system.c src/tinymt32.c -lcunit
	./tests/test_gaussian
	@$(CC) -o tests/test_get_file_info tests/test_get_file_info.c src/system.c src/tinymt32.c -lcunit
	./tests/test_get_file_info
	@$(CC) -o tests/test_process_block tests/test_process_block.c src/system.c src/tinymt32.c -lcunit
	./tests/test_process_block

clean:
	@rm -f src/*.o
	@rm -f fec
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

CC=gcc
CFLAGS=-Wall -Werror -g
LIBS=-lcunit -lpthread
INCLUDE_HEADERS_DIRECTORY=-Iheaders

fec: main.c     # add your other object files needed to compile your program here. !! The ordering is important !! if file_a.o depends on file_b.o, file_a.o must be placed BEFORE file_b.o in the list !
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ $^ $(LIBS)    # this will run the following command: gcc -Wall -Werror -g -o kmeans src/distance.o other_object_filespresent_above.o ... -lcunit -lpthread

%.o: %.c                  # if for example you want to compute example.c this will create an object file called example.o in the same directory as example.c. Don't forget to clean it in your "make clean"
	$(CC) $(INCLUDE_HEADERS_DIRECTORY) $(CFLAGS) -o $@ -c $<

clean:
	rm -f src/*.o
	rm -f fec
	rm -f test_tinymt32
	rm -f testmls
	rm -f main
	rm -f output.txt


tests: tests/
	$(CC) -o test_tinymt32 tests/test_tinymt32.c system.c tinymt32.c -lcunit
	$(CC) -o main main.c system.c tinymt32.c -lm
	$(CC) -o testmls tests/test_make_linear_system.c system.c tinymt32.c -lcunit
	$(CC) -o testgencoeffs tests/test_gen_coeffs.c system.c tinymt32.c -lcunit
	$(CC) -o testbts tests/test_block_to_string.c system.c tinymt32.c -lcunit
	$(CC) -o testmakeblock tests/test_make_block.c system.c tinymt32.c -lcunit
	$(CC) -o testflw tests/test_find_lost_words.C system.c tinymt32.c -lcunit
	valgrind ./testgencoeffs
	valgrind ./testbts
	valgrind ./testmls
	valgrind ./testmakeblock
	valgrind ./testflw
	valgrind ./test_tinymt32


testsMLS: tests/test_make_linear_system.c
	$(CC) -o testmls tests/test_make_linear_system.c system.c tinymt32.c -lcunit
	valgrind ./testmls

testsGenCoeffs: tests/test_gen_coeffs.c
	$(CC) -o testgencoeffs tests/test_gen_coeffs.c system.c tinymt32.c -lcunit
	valgrind ./testgencoeffs

testsMakeBlock: tests/test_make_block.c
	$(CC) -o testmakeblock tests/test_make_block.c system.c tinymt32.c -lcunit
	valgrind ./testmakeblock

testsBTS: tests/test_block_to_string.c
	$(CC) -o testbts tests/test_block_to_string.c system.c tinymt32.c -lcunit
	valgrind ./testbts

testsFLW: tests/test_find_lost_words.c
	$(CC) -o testflw tests/test_find_lost_words.c system.c tinymt32.c -lcunit
	valgrind ./testflw

testsWriteBlock: tests/test_write_block.c
	$(CC) -o testwriteblock tests/test_write_block.c system.c tinymt32.c -lcunit
	./testwriteblock input_binary/ -f test.txt

tests_short: tests/test_tinymt32.c
	$(CC) -Wall -Werror -o test_tinymt32 tests/test_tinymt32.c system.c tinymt32.c -lcunit -lm
	$(CC) -Wall -Werror -o main main.c system.c tinymt32.c
	./test_tinymt32

main: main.c
	$(CC) -Werror -o main main.c system.c tinymt32.c -lm
	./main input_binary/ -f output.txt  -v

# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: clean tests

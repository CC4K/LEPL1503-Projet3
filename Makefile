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
	./test_tinymt32
	$(CC) -o main main.c system.c tinymt32.c -lm
	$(CC) -o testmls tests/test_make_linear_system.c system.c tinymt32.c -lcunit
	./testmls
	$(CC) -o testgencoeffs tests/test_gen_coeffs.c system.c tinymt32.c -lcunit
	./testgencoeffs
	$(CC) -o testbts tests/test_block_to_string.c system.c tinymt32.c -lcunit
	./testbts
	$(CC) -o testmakeblock tests/test_make_block.c system.c tinymt32.c -lcunit
	./testmakeblock
	$(CC) -o testflw tests/test_find_lost_words.c system.c tinymt32.c -lcunit
	./testflw
	$(CC) -o testwriteblock tests/test_write_block.c system.c tinymt32.c -lcunit
	./testwriteblock input_binary/ -f
	$(CC) -o testwlb tests/test_write_last_block.c system.c tinymt32.c -lcunit
	./testwlb input_binary/ -f
	$(CC) -o testgaussian tests/test_gaussian.c system.c tinymt32.c -lcunit
	./testgaussian
	$(CC) -o testgfi tests/test_get_file_info.c system.c tinymt32.c -lcunit
	./testgfi
	$(CC) -o testprocessblock tests/test_process_block.c system.c tinymt32.c -lcunit
	./testprocessblock


testsMLS: tests/test_make_linear_system.c
	$(CC) -o testmls tests/test_make_linear_system.c system.c tinymt32.c -lcunit
	./testmls

testsGenCoeffs: tests/test_gen_coeffs.c
	$(CC) -o testgencoeffs tests/test_gen_coeffs.c system.c tinymt32.c -lcunit
	./testgencoeffs

testsMakeBlock: tests/test_make_block.c
	$(CC) -o testmakeblock tests/test_make_block.c system.c tinymt32.c -lcunit
	./testmakeblock

testsBTS: tests/test_block_to_string.c
	$(CC) -o testbts tests/test_block_to_string.c system.c tinymt32.c -lcunit
	./testbts

testsFLW: tests/test_find_lost_words.c
	$(CC) -o testflw tests/test_find_lost_words.c system.c tinymt32.c -lcunit
	./testflw

testsWriteBlock: tests/test_write_block.c
	$(CC) -o testwriteblock tests/test_write_block.c system.c tinymt32.c -lcunit
	./testwriteblock input_binary/ -f

testsWLB: tests/test_write_last_block.c
	$(CC) -o testwlb tests/test_write_last_block.c system.c tinymt32.c -lcunit
	./testwlb input_binary/ -f

testsGaussian: tests/test_gaussian.c
	$(CC) -o testgaussian tests/test_gaussian.c system.c tinymt32.c -lcunit
	./testgaussian

testsGFI: tests/test_get_file_info.c
	$(CC) -o testgfi tests/test_get_file_info.c system.c tinymt32.c -lcunit
	./testgfi

testsProcessBlock: tests/test_process_block.c
	$(CC) -o testprocessblock tests/test_process_block.c system.c tinymt32.c -lcunit
	./testprocessblock

tests_short: tests/test_tinymt32.c
	$(CC) -Wall -Werror -o test_tinymt32 tests/test_tinymt32.c system.c tinymt32.c -lcunit -lm
	$(CC) -Wall -Werror -o main main.c system.c tinymt32.c
	./test_tinymt32

main: main.c
	$(CC) -Werror -o main main.c system.c tinymt32.c -lm
	./main input_binary/ -f output.txt  -v

# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: clean tests

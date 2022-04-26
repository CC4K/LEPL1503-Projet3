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
	rm -f main

tests: tests/test_tinymt32.c
	$(CC) -o test_tinymt32 tests/test_tinymt32.c system.c tinymt32.c -lcunit
	$(CC) -o main main.c system.c tinymt32.c
	valgrind ./test_tinymt32

testsMLS: tests/test_make_linear_system.c
	$(CC) -o testmls tests/test_make_linear_system.c system.c tinymt32.c -lcunit
	valgrind --track-origins=yes ./testmls

tests_short: tests/test_tinymt32.c
	$(CC) -Wall -Werror -o test_tinymt32 tests/test_tinymt32.c system.c tinymt32.c -lcunit
	$(CC) -Wall -Werror -o main main.c system.c tinymt32.c
	./test_tinymt32

main: main.c
	$(CC) -Werror -o main main.c system.c tinymt32.c
	./main input_binary/ -v

# a .PHONY target forces make to execute the command even if the target already exists
.PHONY: clean tests

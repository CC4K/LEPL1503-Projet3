# FEC Project for the LEPL1503 course

### P3-2022-Group Y2

---
---
## Project goals

This project is a **Forward Erasure Correction** (FEC) algorithm capable of fixing packet loss in network transmissions.
Given the said algorithm in Python, we were tasked to translate it in C and use threads to improve it.
## How does this program work ?

The program takes binary files in input.

The 24 first bytes of the file give us :
- **seed** : number used to generate random numbers needed for file correction, encrypted on a 4 bytes unsigned integer
- **block_size** : size of the source symbols in the file, encrypted on a 4 bytes unsigned integer
- **word_size** : size of a word in the file, encrypted on a 4 bytes unsigned integer
- **redundancy** : number of redundancy coefficients for each block, encrypted on a 4 bytes unsigned integer
- **message_size** : size of the file (not including the 24 initial bytes), encrypted on an 8 bytes unsigned integer

Given the "size" of the file, the program can now find the number of **full blocks** in the file and if a **non-full block** will have to be corrected separately.

For each block, we calculate the missing value for the full blocks, translate it back to string & write it in the output file.

Then the same is done with the very last block (if there is one).

Finally, the program can close the binary file.
## How to run the program ?

First you must have your binary files in the `input_binary/` directory.
You can then compile the code using these commands.

The project includes a Makefile to avoid overcomplicated commands :
- `make fec` : compiles sequential code for execution
- `make fec_threads` : compiles threaded code for execution
- `make run` : compiles and runs sequential code (default output is output.txt and verbose is activated)
- `make threads_run` : compiles and runs threaded code (default output is output.txt, verbose is activated and 4 threads are running)
- `make valgrind_run` : compiles and runs sequential code with Valgrind
- `make valgrind_threads` : compiles and runs threaded code with Valgrind
- `make tests` : compiles and runs the tests (as well as the tested files)
- `make clean` : clears all compiled files

To run the executable "fec", use `./fec input_binary/` which will print your output straight to your terminal.
Same goes for the "thread" executable.

Here are some of the parameters you can use alongside the base command :
- `-v` : to set verbose mode to 'true' and show steps of the program in your terminal
- `-f output.txt` : to get your output in a .txt file (here named "output.txt")
- `-n 4` : to specify the number of threads you want to run the "thread" executable with (here 4 threads)
## For Mac users

If the program does not run because of warnings on MacOS, you need to remove the "-Werror" flag in CFLAGS at the beginning of the Makefile
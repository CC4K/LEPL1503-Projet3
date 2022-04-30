# FEC Project for the LEPL1503 course

### P3-2022-Group Y2

---
---
## Project goals

This project is an **Forward Erasure Correction** (FEC) algorithm capable of fixing packet loss in network transmissions.
Given the said algorithm in Python, we were tasked to translate it in C and use threads to improve it.
## How does this program works ?

The program takes binary files in input.

The 24 first bytes of the file gives us :
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
- `make fec` : compiles code for execution
- `make tests` : compiles and runs the tests (as well as the tested files)
- `make clean` : clears all compiled files

To run the executable 'main', use `./fec input_binary/` which will give your output to your terminal.

Here are some of the parameters you can use with the base command :
- `-f output.txt` : to get your output in a .txt file name "output"
- `-v` : to set verbose mode to 'true' and show steps of the program in your terminal
- `-n 4` : to specify the number of threads you want to run the program with (here 4 threads)
## Threads

TODO : TODO
## Performance results

TODO : TODO : TODO

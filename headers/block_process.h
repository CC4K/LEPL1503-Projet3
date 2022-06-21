#ifndef GROUPEY2_BLOCK_PROCESS_H
#define GROUPEY2_BLOCK_PROCESS_H

#include <stdint.h>
#include <stdbool.h>


/**
 * Structure to store missing words emplacements
 */
typedef struct {
    uint8_t* unknown_map;
    uint8_t unknowns_amount;
} unknowns_t;

/**
 * Structure to store linear system matrices
 */
typedef struct {
    uint8_t** A;
    uint8_t** B;
} linear_system_t;

/**
 * Help function to print n x m matrices in verbose mode
 * @param matrix: the matrix to print
 * @param n: number of lines
 * @param m: number of columns
 */
void printf_matrix(uint8_t** matrix, uint8_t n, uint8_t m);

/**
 * Help function to print the linear systems A x B in verbose mode
 * @param A: first matrix
 * @param B: second matrix
 * @param nb_unk: size of A
 */
void printf_linear_system(uint8_t** A, uint8_t** B, uint8_t nb_unk);

/**
 * Build the block based on the data and the size of a block
 * @param data: the block data in binary form. If the input file is given, it will be cut
 *              'size' symbols of size 'word_size' bytes, followed by 'redundancy' symbols of size 'word_size'
 * @param size: number of source symbols in a block
 * @return block: the built block in the form of a matrix (one line = one symbol)
 */
uint8_t** make_block(uint8_t* data, uint8_t size);

/**
 * Based on a block, find the lost source symbols et index them in 'unknown_indexes'
 * A symbol is considered as lost in the block if the symbol only contains 0's
 * @param block: the said block
 * @param size: the size of the block
 * @return unknown_indexes: table of size 'size' mapping with source symbols
 *                          The input 'i' is 'true' if the source symbol 'i' is lost
 * @return unknwowns: the amount of lost source symbols
 */
unknowns_t* find_lost_words(uint8_t** block, uint8_t size);

/**
 * Build a linear system Ax=b from the blocks given in args
 * @param unknown_indexes: index of the lost source symbols of a block. The input 'i' is 'true' if the symbol 'i' is lost
 * @pixearam nb_unk: the amount of unknowns in the system - the size of the system
 * @param current_block: the block of symbols to solve
 * @param block_size: the amount of source symbols in the block
 * @return A: the coefficients matrix
 * @return B: the independents terms vector. Each element of B is the same size as a data vector (packet)
 */
linear_system_t* make_linear_system(uint8_t* unknown_indexes, uint8_t nb_unk, uint8_t** current_block, uint8_t block_size);

/**
 * Based on a block, find the unknowns (e.g., lost source symbols) and build the corresponding linear system.
 * This simple version considers there are always as many unknowns as there are redundancy symbols, that is to say
 * we will always build a system with as many equations as there are redundancy symbols
 * @param block: the said block
 * @param size: the size of the block
 * @return block: the solved block
 */
uint8_t** process_block(uint8_t** block, uint8_t size);

#endif //GROUPEY2_BLOCK_PROCESS_H
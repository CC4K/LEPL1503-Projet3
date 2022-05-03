#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>

/**
 *
 * Add two vectors in a Galois Field 256
 * @param symbol_1: the first symbol to add
 * @param symbol_2: the second symbol to add
 * @param symbol_size: size of the two symbols (of the same size!)
 * @return: a new vector of `symbol_size` byte containing the result of symbol_1 + symbol_2 in GF(256)
 */
uint8_t *gf_256_full_add_vector(uint8_t *symbol_1, uint8_t *symbol_2, uint32_t symbol_size);

/**
 *
 * Multiply a vector by a coefficient in a Galois Field 256
 * @param symbol: the symbol to multiply
 * @param coef: the coefficient of the scaling
 * @param symbol_size: size of the symbol
 * @return: a new vector of `symbol_size` byte containing the result of symbol * coef in GF(256)
 */
uint8_t *gf_256_mul_vector(uint8_t *symbol, uint8_t coef, uint32_t symbol_size);

/**
 *
 * Divide a vector in a Galois Field 256 by a coefficient
 * @param symbol: the symbol to add
 * @param coef: the dividing coefficient
 * @param symbol_size: size of the two symbols (of the same size!)
 * @return: a new vector of `symbol_size` byte containing the result of symbol / coef in GF(256)
 */
uint8_t *gf_256_inv_vector(uint8_t *symbol, uint8_t coef, uint32_t symbol_size);

/**
 *
 * Resolve the linear system Ax=b in a Galois Field 256. The result is stored in the independent terms after the resolution
 * @param A: matrix of coefficients
 * @param b: independent terms
 * @param symbol_size: size of the independent terms
 * @param system_size: the size of the system (i.e., number of rows/columns)
 */
void gf_256_gaussian_elimination(uint8_t **A, uint8_t **b, uint32_t symbol_size, uint32_t system_size);

/**
 *
 * Generate all coefficients for a block
 * @param seed: the seed to generate the coefficients
 * @param nss: number of source symbols in a block
 * @param nrs: number of repair symbols in a block
 * @return: a nss * nrs array of coefficients
 */
uint8_t **gen_coefs(uint32_t seed, uint32_t nss, uint32_t nrs);

#endif /* SYSTEM_H */

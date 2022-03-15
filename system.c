//
// Created by jacques on 15/03/22.
//

//Includes

#include "headers/system.h"
#include "headers/gf256_tables.h"

/**
 *
 * Add two vectors in a Galois Field 256
 * @param symbol_1: the first symbol to add
 * @param symbol_2: the second symbol to add
 * @param symbol_size: size of the two symbols (of the same size!)
 * @return: a new vector of `symbol_size` byte containing the result of symbol_1 + symbol_2 in GF(256)
 */
uint8_t *gf_256_full_add_vector(uint8_t *symbol_1, uint8_t *symbol_2, uint32_t symbol_size){
    //Fait par Jacques le 15/03/22
    //TODO: Verifier si le code est bon
    uint8_t *output = symbol_1;
    for (int i = 0; i < symbol_size/ sizeof(int); ++i) {
        output[i] = (symbol_1[i] ^ symbol_2[i]);
    }
    return output;
}

/**
 *
 * Divide a vector in a Galois Field 256 by a coefficient
 * @param symbol: the symbol to add
 * @param coef: the dividing coefficient
 * @param symbol_size: size of the two symbols (of the same size!)
 * @return: a new vector of `symbol_size` byte containing the result of symbol / coef in GF(256)
 */
uint8_t *gf_256_inv_vector(uint8_t *symbol, uint8_t coef, uint32_t symbol_size) {
    //Fait par Pierre le 15/03/22
    //TODO: Verifier si le code est bon
    uint8_t *output = symbol;
    for (int i = 0; i < symbol_size / sizeof(int); ++i) {
        output[i] = gf256_mul_table[(int) symbol[i]][gf256_inv_table[coef]];
    }
    return output;
}

/**
 *
 * Multiply a vector by a coefficient in a Galois Field 256
 * @param symbol: the symbol to multiply
 * @param coef: the coefficient of the scaling
 * @param symbol_size: size of the symbol
 * @return: a new vector of `symbol_size` byte containing the result of symbol * coef in GF(256)
 */
uint8_t *gf_256_mul_vector(uint8_t *symbol, uint8_t coef, uint32_t symbol_size){
    uint8_t *output = symbol;
    for (int i = 0; i < symbol_size/ sizeof(int); i++) {
        output[i] = gf256_mul_table[(int) symbol[i]][coef];
    }
    return output;
}
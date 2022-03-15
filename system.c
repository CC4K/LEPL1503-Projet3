//
// Created by jacques on 15/03/22.
//

//Includes

#include "headers/system.h"

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
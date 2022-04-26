//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Jacques, Romain, Cédric & Pierre on 15/03/22.  //
//===========================================================//

//================= Libraries and imports ===================//
#include <malloc.h>
#include <string.h>
#include "headers/system.h"
#include "headers/gf256_tables.h"
#include "headers/tinymt32.h"

//====================== Functions ==========================//

/**
 * Add two vectors in a Galois Field 256
 * @param symbol_1: the first symbol to add
 * @param symbol_2: the second symbol to add
 * @param symbol_size: size of the two symbols (of the same size!)
 * @return a new vector of `symbol_size` byte containing the result of symbol_1 + symbol_2 in GF(256)
 */
uint8_t* gf_256_full_add_vector(uint8_t* symbol_1, uint8_t* symbol_2, uint32_t symbol_size) {
    uint8_t* output = malloc(sizeof(uint8_t) * symbol_size);
    for (int i = 0; i < symbol_size; i++) {
        output[i] = (symbol_1[i] ^ symbol_2[i]);
    }
    return output;
}

/**
 * Multiply a vector by a coefficient in a Galois Field 256
 * @param symbol: the symbol to multiply
 * @param coef: the coefficient of the scaling
 * @param symbol_size: size of the symbol
 * @return a new vector of `symbol_size` byte containing the result of symbol * coef in GF(256)
 */
uint8_t* gf_256_mul_vector(uint8_t* symbol, uint8_t coef, uint32_t symbol_size) {
    uint8_t* output = malloc(sizeof(uint8_t) * symbol_size);
    for (int i = 0; i < symbol_size; i++) {
        output[i] = gf256_mul_table[(int) symbol[i]][coef];
    }
    return output;
}

/**
 * Divide a vector in a Galois Field 256 by a coefficient
 * @param symbol: the symbol to add
 * @param coef: the dividing coefficient
 * @param symbol_size: size of the two symbols (of the same size!)
 * @return a new vector of `symbol_size` byte containing the result of symbol / coef in GF(256)
 */
uint8_t* gf_256_inv_vector(uint8_t* symbol, uint8_t coef, uint32_t symbol_size) {
    uint8_t* output = malloc(sizeof(uint8_t) * symbol_size);
    for (int i = 0; i < symbol_size; i++) {
        output[i] = gf256_mul_table[(int) symbol[i]][gf256_inv_table[coef]];
    }
    return output;
}

/**
 * Resolve the linear system Ax=b in a Galois Field 256. The result is stored in the independent terms after the resolution
 * @param A: matrix of coefficients
 * @param b: independent terms
 * @param symbol_size: size of the independent terms
 * @param system_size: the size of the system (i.e., number of rows/columns)
 */
void gf_256_gaussian_elimination(uint8_t** A, uint8_t** b, uint32_t symbol_size, uint32_t system_size) {
    // Forward elimination
    for (int i = 0; i < system_size; i++) {
        for (int j = i+1; j < system_size; j++) {
            uint8_t factor = gf256_mul_table[A[j][i]][gf256_inv_table[A[i][i]]];
            for (int k = 0; k < system_size; k++) {
                A[j][k] = A[j][k] ^ gf256_mul_table[A[i][k]][factor];
            }
            b[j] = gf_256_full_add_vector(b[j], gf_256_mul_vector(b[i], factor, symbol_size), symbol_size);
        }
    }
    // Backward elimination
    uint8_t* factor_tab = malloc(sizeof(uint8_t) * symbol_size);
    for (int i = system_size - 1; i > -1 ; i--) {
        for(int n = 0; n < symbol_size; n++) {
            factor_tab[n] = 0;
        }
        for (int j = i+1; j < system_size; j++) {
            factor_tab = gf_256_full_add_vector(factor_tab, gf_256_mul_vector(b[j], A[i][j], symbol_size), symbol_size);
        }
        b[i] = gf_256_inv_vector(gf_256_full_add_vector(b[i], factor_tab, symbol_size), A[i][i], symbol_size);
    }
}

/**
 * Generate all coefficients for a block
 * @param seed: the seed to generate the coefficients
 * @param nss: number of source symbols in a block = redudancy
 * @param nrs: number of repair symbols in a block = block_size
 * @return a nss * nrs array of coefficients
 */
uint8_t** gen_coefs(uint32_t seed, uint32_t nss, uint32_t nrs) {

    // Malloc for matrix
    uint8_t** coefs = malloc(sizeof(uint8_t*) * nss);
    if(coefs == NULL) return NULL;
    // malloc vectors in matrix
    for (int i = 0; i < nss ; i++) {
        coefs[i] = malloc(sizeof(uint8_t) * nrs);
        if (coefs[i] == NULL) return NULL;
    }

    // TINYMT32(seed) ~= rnd
    tinymt32_t prng;
    memset(&prng, 0, sizeof(tinymt32_t));
    // Do not modify these values!
    prng.mat1 = 0x8f7011ee;
    prng.mat2 = 0xfc78ff1f;
    prng.tmat = 0x3793fdff;
    tinymt32_init(&prng, seed);

    // Generate all coefficients
    for (int i = 0; i < nss; i++) {
        for (int j = 0; j < nrs; j++) {
            coefs[i][j] = (uint8_t) tinymt32_generate_uint32(&prng);
            if (coefs[i][j] == 0) {
                coefs[i][j] = 1;
            }
        }
    }
    return coefs;
}

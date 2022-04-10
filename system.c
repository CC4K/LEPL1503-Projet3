//
// Created by jacques on 15/03/22.
//

//Includes

#include "headers/system.h"
#include "headers/gf256_tables.h"
// A enlever, juste utilisé pour debug
#include "stdio.h"
#include <inttypes.h>
#include "headers/tinymt32.h"
#include <malloc.h>
#include <stdio.h>
#include <string.h>

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
    for (int i = 0; i < symbol_size/sizeof(int); ++i) {
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
/**
 *
 * Resolve the linear system Ax=b in a Galois Field 256. The result is stored in the independent terms after the resolution
 * @param A: matrix of coefficients
 * @param b: independent terms
 * @param symbol_size: size of the independent terms
 * @param system_size: the size of the system (i.e., number of rows/columns)
 */
void gf_256_gaussian_elimination(uint8_t **A, uint8_t **b, uint32_t symbol_size, uint32_t system_size) {
    // Code de Romain
    for (int k = 0; k < system_size; ++k) {
        for (int i = k+1; i < system_size; ++i) {
            uint8_t factor = gf256_mul_table[A[i][k]][gf256_inv_table[A[k][k]]];
            for (int j = 0; j < system_size; ++j) {
                A[i][j] = A[i][j] ^ gf256_mul_table[A[k][j]][factor];
            }
            for (int j = 0; j < system_size; ++j) {
            }
            printf("\n");
            b[i] = gf_256_full_add_vector(b[i], gf_256_mul_vector(b[k], factor, symbol_size), symbol_size);
        }
    }

    uint8_t* factor_tab = malloc(sizeof(uint8_t)*symbol_size);
    for (int i = system_size - 1; i > -1 ; --i) {
        for(int a = 0; a < symbol_size; a++){
            factor_tab[a] = 0;
        }
        for (int j = i+1; j < system_size; ++j) {
            factor_tab = gf_256_full_add_vector(factor_tab, gf_256_mul_vector(b[j], A[i][j], symbol_size), symbol_size);
        }
        b[i] = gf_256_inv_vector(gf_256_full_add_vector(b[i], factor_tab, symbol_size), A[i][i], symbol_size);
    }

    // Code de Cédric (retravaillé)
    // Forward
    // for (int k = 0; k < symbol_size; k++) {
    //  uint8_t i_max = k;
    //  uint8_t v_max = A[i_max][k];
    //  for (int i = k+1; i < symbol_size; i++) {
    //      if (A[i][k] > v_max || -(A[i][k]) > v_max){
    //          v_max = A[i][k];
    //          i_max = i;
    //      }
    //  }
    //  if (i_max != k){
    //      for (int h = 0; h < symbol_size; h++) {
    //          double temp = A[k][h];
    //          A[k][h] = A[i_max][h];
    //          A[i_max][h] = temp;
    //      }
    //  }
    //  for (int i = k+1; i < symbol_size; i++) {
    //      uint8_t divider = gf256_mul_table[A[i][k]][gf256_inv_table[A[k][k]]];
    //      for (int j = k+1; j < symbol_size; j++) {
    //          A[i][j] = A[i][j] ^ gf256_mul_table[A[k][j]][divider];
    //      }
    //      A[i][k] = 0;
    //  }
    //}

    // Backward
//    uint8_t* factor_tab;
//    for (int m = symbol_size-1; m >= 0; m--) {
//        factor_tab[m] = A[m][symbol_size];
//        for (int n = m+1; n < symbol_size; n++) {
//            factor_tab = gf_256_full_add_vector(factor_tab, gf_256_mul_vector(b[n], A[m][n], symbol_size), symbol_size);
//        }
//        b[m] = gf_256_inv_vector(gf_256_full_add_vector(b[m], factor_tab, symbol_size), A[m][m], symbol_size);
//    }
    // TODO
    // le forward semble fonctionner
    // le backward fait n'importe quoi HELP !!!
}


/**
 *
 * Generate all coefficients for a block
 * @param seed: the seed to generate the coefficients
 * @param nss: number of source symbols in a block
 * @param nrs: number of repair symbols in a block
 * @return: a nss * nrs array of coefficients
 */
uint8_t **gen_coefs(uint32_t seed, uint32_t nss, uint32_t nrs){
    //fait pas Jacques le 29/03/22
    //Normalement bon car vérifié par Emma

    //cree une matrice de malloc
    uint8_t **coefs = malloc(sizeof(uint8_t*) * nss);
    if(coefs == NULL){
        return NULL;
    }
    for (int i = 0; i < nss ; ++i) {
        coefs[i] = malloc(sizeof(uint8_t) * nrs);
        if (coefs[i] == NULL){
            return NULL;
        }
    }

    //crée le *random
    tinymt32_t prng;
    memset(&prng, 0, sizeof(tinymt32_t));
    // Do not modify these values!
    prng.mat1 = 0x8f7011ee;
    prng.mat2 = 0xfc78ff1f;
    prng.tmat = 0x3793fdff;
    tinymt32_init(&prng, seed);

    //generate tous les coefs
    for (int i = 0; i < nss; ++i) {
        for (int j = 0; j < nrs; ++j) {
            coefs[i][j] = (uint8_t) tinymt32_generate_uint32(&prng);
            if (coefs[i][j] == 0){
                coefs[i][j] = 1;
            }
        }
    }
    return coefs;
}

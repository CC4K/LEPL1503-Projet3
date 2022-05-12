//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Jacques, Romain, Cédric & Pierre on 15/03/22.  //
//===========================================================//

//================= Libraries and headers ===================//
#include <string.h>
#include <stdlib.h>
#include "../headers/system.h"
#include "../headers/gf256_tables.h"
#include "../headers/tinymt32.h"

//====================== Functions ==========================//
uint8_t* gf_256_full_add_vector(uint8_t* symbol_1, uint8_t* symbol_2, uint32_t symbol_size) {
    // Allocate memory for output
    uint8_t* output = malloc(sizeof(uint8_t) * symbol_size);
    if (output == NULL) exit(EXIT_FAILURE);

    for (int32_t i = 0; i < symbol_size; i++) {
        output[i] = (symbol_1[i] ^ symbol_2[i]);
    }

    return output;
}

uint8_t* gf_256_mul_vector(uint8_t* symbol, uint8_t coef, uint32_t symbol_size) {
    // Allocate memory for output
    uint8_t* output = malloc(sizeof(uint8_t) * symbol_size);
    if (output == NULL) exit(EXIT_FAILURE);

    for (int32_t i = 0; i < symbol_size; i++) {
        output[i] = gf256_mul_table[(int) symbol[i]][coef];
    }

    return output;
}

uint8_t* gf_256_inv_vector(uint8_t* symbol, uint8_t coef, uint32_t symbol_size) {
    // Allocate memory for output
    uint8_t* output = malloc(sizeof(uint8_t) * symbol_size);
    if (output == NULL) exit(EXIT_FAILURE);

    for (int32_t i = 0; i < symbol_size; i++) {
        output[i] = gf256_mul_table[(int) symbol[i]][gf256_inv_table[coef]];
    }

    return output;
}

void gf_256_gaussian_elimination(uint8_t** A, uint8_t** b, uint32_t symbol_size, uint32_t system_size) {
    // Forward elimination
    for (int32_t i = 0; i < system_size; i++) {
        for (int32_t j = i+1; j < system_size; j++) {
            uint8_t factor = gf256_mul_table[A[j][i]][gf256_inv_table[A[i][i]]];
            for (int32_t k = 0; k < system_size; k++) {
                A[j][k] = A[j][k] ^ gf256_mul_table[A[i][k]][factor];
            }
            uint8_t* mul_vector = gf_256_mul_vector(b[i], factor, symbol_size);
            uint8_t* free_add_vector = b[j];
            b[j] = gf_256_full_add_vector(b[j], mul_vector, symbol_size);
            // Free temporary values
            free(mul_vector);
            free(free_add_vector);
        }
    }
    // Backward elimination
    uint8_t* factor_tab = malloc(sizeof(uint8_t) * symbol_size);
    if (factor_tab == NULL) exit(EXIT_FAILURE);
    for (int32_t i = system_size - 1; i > -1 ; i--) {
        for(int32_t n = 0; n < symbol_size; n++) {
            factor_tab[n] = 0;
        }
        for (int32_t j = i+1; j < system_size; j++) {
            uint8_t* mul_vector = gf_256_mul_vector(b[j], A[i][j], symbol_size);
            uint8_t* free_add_vector_2 = factor_tab;
            factor_tab = gf_256_full_add_vector(factor_tab, mul_vector, symbol_size);
            // Free temporary values
            free(mul_vector);
            free(free_add_vector_2);
        }
        uint8_t* add_vector = gf_256_full_add_vector(b[i], factor_tab, symbol_size);
        uint8_t* free_b = b[i];
        b[i] = gf_256_inv_vector(add_vector, A[i][i], symbol_size);
        // Free temporary values
        free(add_vector);
        free(free_b);
    }
    // Free temporary value
    free(factor_tab);
}

uint8_t** gen_coefs(uint32_t seed, uint32_t nss, uint32_t nrs) {
    // Allocate memory for coefs matrix
    uint8_t** coeffs = malloc(sizeof(uint8_t*) * nss);
    if (coeffs == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < nss; i++) {
        coeffs[i] = malloc(sizeof(uint8_t) * nrs);
        if (coeffs[i] == NULL) exit(EXIT_FAILURE);
    }

    // TINYMT32(seed) ~= rnd
    tinymt32_t prng;
    memset(&prng, 0, sizeof(tinymt32_t));
    prng.mat1 = 0x8f7011ee;
    prng.mat2 = 0xfc78ff1f;
    prng.tmat = 0x3793fdff;
    tinymt32_init(&prng, seed);

    // Generate all coefficients
    for (int32_t i = 0; i < nss; i++) {
        for (int32_t j = 0; j < nrs; j++) {
            coeffs[i][j] = (uint8_t) tinymt32_generate_uint32(&prng);
            if (coeffs[i][j] == 0) {
                coeffs[i][j] = 1;
            }
        }
    }

    return coeffs;
}

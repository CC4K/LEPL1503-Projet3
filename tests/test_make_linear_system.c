//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain on 25/04/22.                            //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdint.h>
#include <string.h>
#include <CUnit/Basic.h>
#include "../headers/system.h"

// Setup structure for function to test
typedef struct {
    uint8_t** A;
    uint8_t** B;
} linear_system_t;

// Setup global variables
uint8_t word_size = 3;
uint8_t** coeffs = NULL;

// Function to test
linear_system_t* make_linear_system(uint8_t* unknown_indexes, uint8_t nb_unk, uint8_t** current_block, uint8_t block_size) {
    // Crée par Romain le 15/04/22

    // Allocate memory for the two matrices
    uint8_t** A = malloc(sizeof(uint8_t*) * nb_unk);
    if (A == NULL) return NULL;
    for (size_t i = 0; i < nb_unk; ++i) {
        A[i] = malloc(sizeof(uint8_t) * nb_unk);
        if (A[i] == NULL) return NULL;
    }
    uint8_t** B = malloc(sizeof(uint8_t*) * nb_unk);
    if (B == NULL) return NULL;
    for (size_t i = 0; i < nb_unk; ++i) {
        B[i] = malloc(sizeof(uint8_t) * word_size);
        if (B[i] == NULL) return NULL;
    }

    for (int i = 0; i < nb_unk; i++) {
        B[i] = current_block[block_size + i];
    }

    for (int i = 0; i < nb_unk; i++) {
        int temp = 0;
        for (int j = 0; j < block_size; j++) {
            if (unknown_indexes[j] == 1) {
                A[i][temp] = coeffs[i][j];
                temp += 1;
            }
            else {
                uint8_t* vec_mul = gf_256_mul_vector(current_block[j], coeffs[i][j], word_size);
                B[i] = gf_256_full_add_vector(B[i], vec_mul,word_size);
            }
        }
    }


    // Allocate memory to store the results in a struct and return it
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) return NULL;
    output->A = A;
    output->B = B;
    return output;
}

void test_MLS() {
    coeffs = malloc(sizeof(uint8_t * ) * 4);
    for (int i = 0; i < 4; ++i) {
        coeffs[i] = malloc(sizeof(uint8_t)*3);
    }
    coeffs[0][0] = 171;
    coeffs[0][1] = 165;
    coeffs[0][2] = 55;
    coeffs[1][0] = 61;
    coeffs[1][1] = 69;
    coeffs[1][2] = 143;
    coeffs[2][0] = 152;
    coeffs[2][1] = 158;
    coeffs[2][2] = 168;
    coeffs[3][0] = 64;
    coeffs[3][1] = 5;
    coeffs[3][2] = 91;
    uint8_t nb_unk = 1;
    uint8_t block_size = 2;
    uint8_t **current_block = malloc(sizeof(uint8_t*)*6);
    for (int i = 0; i < 6; ++i) {
        current_block[i] = malloc(sizeof(uint8_t)*3);
    }

    uint8_t *unknown_indexes = malloc(sizeof(uint8_t)*2);
    current_block[0][0] = 110;
    current_block[0][1] = 103;
    current_block[0][2] = 32;
    current_block[1][0] = 0;
    current_block[1][1] = 0;
    current_block[1][2] = 0;
    current_block[2][0] = 48;
    current_block[2][1] = 218;
    current_block[2][2] = 196;
    current_block[3][0] = 135;
    current_block[3][1] = 164;
    current_block[3][2] = 243;
    current_block[4][0] = 122;
    current_block[4][1] = 252;
    current_block[4][2] = 234;
    current_block[5][0] = 80;
    current_block[5][1] = 117;
    current_block[5][2] = 232;
    unknown_indexes[0] = 0;
    unknown_indexes[1] = 1;
    linear_system_t* test = make_linear_system(unknown_indexes, nb_unk, current_block, block_size);
    printf("A :\n");
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 1; ++j) {
            printf("%"
            PRId8
            "\t", (test->A)[i][j]);
        }
        printf("\n");
    }
    printf("B :\n");
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%"
            PRId8
            "\t", (test->B)[i][j]);
        }
        printf("\n");
    }
    uint8_t** correct_A = malloc(sizeof(uint8_t*)*1);
    correct_A[0] = malloc(sizeof(uint8_t)*1);
    correct_A[0][0] = 165;
    uint8_t** correct_B = malloc(sizeof(uint8_t*));
    correct_B[0] = malloc(sizeof(uint8_t)*3);
    correct_B[0][0] = 141;
    correct_B[0][1] = 253;
    correct_B[0][2] = 0;
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 1; ++j) {
             CU_ASSERT_EQUAL(test->A[i][j], correct_A[i][j]);
        }
        printf("\n");
    }
    for (int i = 0; i < 1; ++i) {
        for (int j = 0; j < 3; ++j) {
            CU_ASSERT_EQUAL(test->B[i][j], correct_B[i][j]);
        }
        printf("\n");
    }
    free(test);
    free(current_block);
    free(unknown_indexes);
    free(coeffs);
    free(correct_B);
    free(correct_A);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("makelinearsystem", 0, 0);
    CU_add_test(suite, "correct_MLS", test_MLS);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

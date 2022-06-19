//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain on 30/04/22.                            //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../headers/system.h"
#include "../headers/global.h"

// Setup structures for other functions
typedef struct {
    uint8_t* unknown_map;
    uint8_t unknowns_amount;
} unknowns_t;

typedef struct {
    uint8_t** A;
    uint8_t** B;
} linear_system_t;
bool verbose = false;
uint64_t word_size = 3;
uint8_t** coeffs = NULL;


// Helping function
void printf_linear_system(uint8_t** A, uint8_t** B, uint8_t nb_unk) {
    printf(">> linear_system :\n");
    for (int32_t i = 0; i < nb_unk; i++) {
        printf("[ ");
        for (int32_t j = 0; j < nb_unk; j++) {
            printf("%d ", A[i][j]);
        }
        printf("]\t[ ");
        for (int32_t j = 0; j < word_size; j++) {
            printf("%d ", B[i][j]);
        }
        printf("]\n");
    }
}

// Function needed for process_block
linear_system_t* make_linear_system(uint8_t* unknown_indexes, uint8_t nb_unk, uint8_t** current_block, uint8_t block_size) {
    // Crée par Romain le 15/04/22

    // Allocate memory for the two matrices
    uint8_t** A = malloc(sizeof(uint8_t*) * nb_unk);
    if (A == NULL) exit(EXIT_FAILURE);
    for (size_t i = 0; i < nb_unk; ++i) {
        A[i] = malloc(sizeof(uint8_t) * nb_unk);
        if (A[i] == NULL) exit(EXIT_FAILURE);
    }
    uint8_t** B = malloc(sizeof(uint8_t*) * nb_unk);
    if (B == NULL) exit(EXIT_FAILURE);

    for (int32_t i = 0; i < nb_unk; i++) {
        B[i] = current_block[block_size + i];
    }
    for (int32_t i = 0; i < nb_unk; i++) {
        int32_t temp = 0;
        for (int32_t j = 0; j < block_size; j++) {
            if (unknown_indexes[j] == 1) {
                A[i][temp] = coeffs[i][j];
                temp++;
            }
            else {
                uint8_t* vec_mul = gf_256_mul_vector(current_block[j], coeffs[i][j], word_size);
                B[i] = gf_256_full_add_vector(B[i], vec_mul,word_size);
                free(vec_mul);
            }
        }
    }

    // Allocate memory to store the results in a struct and return it
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) exit(EXIT_FAILURE);
    output->A = A;
    output->B = B;
    return output;
}

unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Initialize an array of boolean of size 'size' to false & the unknowns to 0
    uint8_t* unknown_indexes = malloc(sizeof(uint8_t) * size);
    if (unknown_indexes == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < size; i++) {
        unknown_indexes[i] = 0;
    }
    uint8_t unknowns = 0;

    // Mapping the locations with lost values and counting the unknowns
    for (int32_t i = 0; i < size; i++) {
        uint8_t count = 0;
        for (int32_t j = 0; j < word_size; j++) {
            count = count + block[i][j];
        }
        // A symbol with only 0's is considered as lost
        if (count == 0) {
            unknown_indexes[i] = 1;
            unknowns = unknowns + 1;
        }
    }

    // Allocate memory to store the results in a struct and return it
    unknowns_t* output = malloc(sizeof(unknowns_t));
    if (output == NULL) exit(EXIT_FAILURE);
    output->unknown_map = unknown_indexes;
    output->unknowns_amount = unknowns;
    return output;
}

// Function to test
uint8_t** process_block(uint8_t** block, uint8_t size) {
    // Crée par Cédric le 13/04/22

    // Import the data from the other functions
    unknowns_t* input_unknowns = find_lost_words(block, size);

    // If there are no unknowns return block
    if (input_unknowns->unknowns_amount > 0) {
        linear_system_t* input_linear_system = make_linear_system(input_unknowns->unknown_map, input_unknowns->unknowns_amount, block, size);

        // Gaussian elimination 'in place'
        gf_256_gaussian_elimination(input_linear_system->A, input_linear_system->B, word_size, input_unknowns->unknowns_amount);

        // Verbose
        if (verbose) printf_linear_system(input_linear_system->A, input_linear_system->B, input_unknowns->unknowns_amount);

        // For each index marked as 'true', replace the data
        int32_t temp = 0;
        for (int32_t i = 0; i < size; i++) {
            if (input_unknowns->unknown_map[i] == 1) {
                memcpy(block[i], input_linear_system->B[temp], sizeof(uint8_t) * word_size);
                for (int32_t j = 0; j < word_size; j++) {
                    block[i][j] = input_linear_system->B[temp][j];
                }
                temp++;
            }
        }

        // Free used values
        for (int32_t i = 0; i < input_unknowns->unknowns_amount; i++) {
            free(input_linear_system->A[i]);
        }
        free(input_linear_system->A);
        for (int32_t i = 0; i < input_unknowns->unknowns_amount; i++) {
            free(input_linear_system->B[i]);
        }
        free(input_linear_system->B);
        free(input_linear_system);
    }
    free(input_unknowns->unknown_map);
    free(input_unknowns);

    return block;
}

void test_process_block() {
    coeffs = malloc(sizeof(uint8_t*) * 4);
    if (coeffs == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < 4; i++) {
        coeffs[i] = malloc(sizeof(uint8_t)*3);
        if (coeffs[i] == NULL) exit(EXIT_FAILURE);
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

    uint8_t** input_block = malloc(sizeof(uint8_t*) * 7);
    if (input_block == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < 7; i++) {
        input_block[i] = malloc(sizeof(uint8_t) * 3);
        if (input_block[i] == NULL) exit(EXIT_FAILURE);
    }
    input_block[0][0] = 0;
    input_block[0][1] = 0;
    input_block[0][2] = 0;
    input_block[1][0] = 103;
    input_block[1][1] = 114;
    input_block[1][2] = 97;
    input_block[2][0] = 0;
    input_block[2][1] = 0;
    input_block[2][2] = 0;
    input_block[3][0] = 229;
    input_block[3][1] = 39;
    input_block[3][2] = 229;
    input_block[4][0] = 52;
    input_block[4][1] = 38;
    input_block[4][2] = 0;
    input_block[5][0] = 63;
    input_block[5][1] = 219;
    input_block[5][2] = 232;
    input_block[6][0] = 147;
    input_block[6][1] = 82;
    input_block[6][2] = 111;

    uint8_t** output_block = malloc(sizeof(uint8_t*) * 7);
    if (output_block == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < 7; i++) {
        output_block[i] = malloc(sizeof(uint8_t) * 3);
        if (output_block[i] == NULL) exit(EXIT_FAILURE);
    }
    output_block[0][0] = 112;
    output_block[0][1] = 114;
    output_block[0][2] = 111;
    output_block[1][0] = 103;
    output_block[1][1] = 114;
    output_block[1][2] = 97;
    output_block[2][0] = 109;
    output_block[2][1] = 109;
    output_block[2][2] = 105;
    output_block[3][0] = 229;
    output_block[3][1] = 39;
    output_block[3][2] = 229;
    output_block[4][0] = 52;
    output_block[4][1] = 38;
    output_block[4][2] = 0;
    output_block[5][0] = 63;
    output_block[5][1] = 219;
    output_block[5][2] = 232;
    output_block[6][0] = 147;
    output_block[6][1] = 82;
    output_block[6][2] = 111;

    uint8_t** processed = process_block(input_block,3);
    for (int32_t i = 0; i < 7; i++) {
        for (int32_t j = 0; j < 3; j++) {
            CU_ASSERT_EQUAL(processed[i][j],output_block[i][j]);
        }
    }
    free(input_block);
    free(output_block);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("processblock", 0, 0);
    CU_add_test(suite, "correct_processblock", test_process_block);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

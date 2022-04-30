//
// Created by romain on 30/04/22.
//
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <CUnit/Basic.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../headers/gf256_tables.h"
#include "../headers/system.h"
#include "../headers/tinymt32.h"

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

void printf_linear_system(uint8_t** A, uint8_t** B, uint8_t nb_unk) {
    printf(">> linear_system :\n");
    for (int i = 0; i < nb_unk; i++) {
        printf("[ ");
        for (int j = 0; j < nb_unk; j++) {
            printf("%d ", A[i][j]);
        }
        printf("]\t[ ");
        for (int j = 0; j < word_size; j++) {
            printf("%d ", B[i][j]);
        }
        printf("]\n");
    }
}

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

    // Verbose
    if (verbose) {
        printf_linear_system(A, B, nb_unk);
        printf(">> size : %d\n", nb_unk);
    }

    // Allocate memory to store the results in a struct and return it
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) return NULL;
    output->A = A;
    output->B = B;
    return output;
}

unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Initialize an array of boolean of size 'size' to false & the unknowns to 0
    uint8_t* unknown_indexes = malloc(sizeof(uint8_t) * size);
    if (unknown_indexes == NULL) return NULL;
    for (int i = 0; i < size; i++) {
        unknown_indexes[i] = 0;
    }
    uint8_t unknowns = 0;

    // Mapping the locations with lost values and counting the unknowns
    for (int i = 0; i < size; i++) {
        uint8_t count = 0;
        for (int j = 0; j < word_size; j++) {
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
    if (output == NULL) return NULL;
    output->unknown_map = unknown_indexes;
    output->unknowns_amount = unknowns;

    // Verbose
    if (verbose) {
        printf(">> unknown_indexes : [");
        for (int i = 0; i < size; i++) {
            if (unknown_indexes[i] == 0) printf(" false");
            else printf(" true");
        }
        printf(" ]\n");
    }

    return output;
}

uint8_t** process_block(uint8_t** block, uint8_t size) {
    // Crée par Cédric le 13/04/22

    // Import the data from the other functions
    unknowns_t* input_unknowns = find_lost_words(block, size);
    uint8_t* unknown_indexes = input_unknowns->unknown_map;
    uint8_t unknowns = input_unknowns->unknowns_amount;
    linear_system_t* input_linear_system = make_linear_system(unknown_indexes, unknowns, block, size);
    uint8_t** A = input_linear_system->A;
    uint8_t** B = input_linear_system->B;

    // Gaussian elimination 'in place'
    gf_256_gaussian_elimination(A, B, word_size, unknowns);

    // Verbose
    if (verbose) printf_linear_system(A, B, unknowns);

    // For each index marked as 'true', replace the data
    uint8_t temp = 0;
    for (int i = 0; i < size; i++) {
        if (unknown_indexes[i] == 1) {
            block[i] = B[temp];
            temp += 1;
        }
    }

    // Return the solved block
    return block;
}

void test_process_block() {

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

    uint8_t** input_block = malloc(sizeof(uint8_t*)*7);

    for (int i = 0; i < 7; ++i) {
        input_block[i] = malloc(sizeof(uint8_t)*3);
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

    uint8_t** output_block = malloc(sizeof(uint8_t*)*7);

    for (int i = 0; i < 7; ++i) {
        output_block[i] = malloc(sizeof(uint8_t)*3);
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
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 3; ++j) {
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
}

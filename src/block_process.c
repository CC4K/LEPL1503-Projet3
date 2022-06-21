//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Cédric on 03/04/22.                            //
//===========================================================//

//================= Libraries and headers ===================//
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../headers/block_process.h"
#include "../headers/system.h"

uint8_t** coeffs;
uint64_t word_size;
uint32_t redundancy;
bool verbose;

//======================= Functions =========================//
void printf_matrix(uint8_t** matrix, uint8_t n, uint8_t m) {
    // Made by Cédric

    printf("[");
    for (int32_t i = 0; i < n; i++) {
        if (i != 0) printf(" [");
        else printf("[");
        for (int32_t j = 0; j < m; j++) {
            if (j != m-1) printf("%d ", matrix[i][j]);
            else printf("%d", matrix[i][j]);
        }
        if (i != n-1) printf("]\n");
        else printf("]");
    }
    printf("]\n");
}

void printf_linear_system(uint8_t** A, uint8_t** B, uint8_t nb_unk) {
    // Made by Cédric

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

uint8_t** make_block(uint8_t* data, uint8_t size) {
    // Made by Jacques

    // Allocates memory for the returned block
    uint8_t** block = malloc(sizeof(uint8_t*) * (size + redundancy));
    if (block == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < (size + redundancy); i++) {
        block[i] = malloc(sizeof(uint8_t) * word_size);
        if (block[i] == NULL) exit(EXIT_FAILURE);
    }

    for (int32_t i = 0; i < (size + redundancy) ; i++) {
        for (int32_t j = 0; j < word_size; j++) {
            block[i][j] = data[i * word_size + j];
        }
    }

    return block;
}

unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Made by Cédric

    // Initializes an array of boolean of size 'size' to false & the amount of unknowns to 0
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

    // Allocates memory to store the results in a struct and return it
    unknowns_t* output = malloc(sizeof(unknowns_t));
    if (output == NULL) exit(EXIT_FAILURE);
    output->unknown_map = unknown_indexes;
    output->unknowns_amount = unknowns;

    // Verbose
    if (verbose) {
        printf(">> unknown_indexes : [");
        for (int32_t i = 0; i < size; i++) {
            if (unknown_indexes[i] == 0) printf(" false");
            else printf(" true");
        }
        printf(" ]\n");
    }

    return output;
}

linear_system_t* make_linear_system(uint8_t* unknown_indexes, uint8_t nb_unk, uint8_t** current_block, uint8_t block_size) {
    // Made by Romain

    // Allocates memory for the two matrices A and B
    uint8_t** A = malloc(sizeof(uint8_t*) * nb_unk);
    if (A == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < nb_unk; i++) {
        A[i] = malloc(sizeof(uint8_t) * nb_unk);
        if (A[i] == NULL) exit(EXIT_FAILURE);
    }
    uint8_t** B = malloc(sizeof(uint8_t*) * nb_unk);
    if (B == NULL) exit(EXIT_FAILURE);

    // Linear system
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
                B[i] = gf_256_full_add_vector(B[i], vec_mul, word_size);
                free(vec_mul);
            }
        }
    }

    // Verbose
    if (verbose) {
        printf_linear_system(A, B, nb_unk);
        printf(">> size : %d\n", nb_unk);
    }

    // Allocates memory to the structure that stores the results and return it
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) exit(EXIT_FAILURE);
    output->A = A;
    output->B = B;
    return output;
}

uint8_t** process_block(uint8_t** block, uint8_t size) {
    // Made by Cédric

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

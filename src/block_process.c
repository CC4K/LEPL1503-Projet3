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

//================== Global Variables =======================//
uint8_t** coeffs = NULL;
uint64_t word_size = 0;
uint32_t redundancy = 0;
bool verbose = false;

//======================= Functions =========================//
void printf_matrix(uint8_t** matrix, uint8_t n, uint8_t m) {
    // Made by Cédric

    printf("[");
    for (int i = 0; i < n; i++) {
        if (i != 0) printf(" [");
        else printf("[");
        for (int j = 0; j < m; j++) {
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

uint8_t** make_block(uint8_t* data, uint8_t size) {
    // Made by Jacques

    // Allocate memory for the returned block
    uint8_t** block = malloc(sizeof(uint8_t*) * (size + redundancy));
    if(block == NULL) return NULL;
    for (int i = 0; i < (size + redundancy); i++) {
        block[i] = malloc(sizeof(uint8_t) * word_size);
        if (block[i] == NULL) return NULL;
    }

    for (int i = 0; i < (size + redundancy) ; i++) {
        for (int j = 0; j < word_size; j++) {
            block[i][j] = data[i * word_size + j];
        }
    }

    return block;
}

unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Made by Cédric

    // Initialize an array of boolean of size 'size' to false & the amount of unknowns to 0
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

linear_system_t* make_linear_system(uint8_t* unknown_indexes, uint8_t nb_unk, uint8_t** current_block, uint8_t block_size) {
    // Made by Romain

    // Allocate memory for the two matrices A and B
    uint8_t** A = malloc(sizeof(uint8_t*) * nb_unk);
    if (A == NULL) return NULL;
    for (size_t i = 0; i < nb_unk; i++) {
        A[i] = malloc(sizeof(uint8_t) * nb_unk);
        if (A[i] == NULL) return NULL;
    }
    uint8_t** B = malloc(sizeof(uint8_t*) * nb_unk);
    if (B == NULL) return NULL;

    for (int i = 0; i < nb_unk; i++) {
        B[i] = current_block[block_size + i];
    }

    for (int i = 0; i < nb_unk; i++) {
        int temp = 0;
        for (int j = 0; j < block_size; j++) {
            if (unknown_indexes[j] == 1) {
                A[i][temp] = coeffs[i][j];
                temp ++;
            }
            else {
                uint8_t* vec_mul = gf_256_mul_vector(current_block[j], coeffs[i][j], word_size);
                B[i] = gf_256_full_add_vector(B[i], vec_mul,word_size);
                // TODO: free vec_mul
                free(vec_mul);
            }
        }
    }

    // Verbose
    if (verbose) {
        printf_linear_system(A, B, nb_unk);
        printf(">> size : %d\n", nb_unk);
    }

    // Allocate memory to the structure that stores the results and return it
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) return NULL;
    output->A = A;
    output->B = B;
    return output;
}

uint8_t** process_block(uint8_t** block, uint8_t size) {
    // Made by Cédric

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
    int temp = 0;
    for (int i = 0; i < size; i++) {
        if (unknown_indexes[i] == 1) {
            // TODO: free lines of block
            free(block[i]);
            block[i] = B[temp];
            temp++;
        }
    }

//    printf("\n\nB:\n");
//    printf_matrix(B, unknowns, word_size);
//    printf("\nblock:\n");
//    printf_matrix(block, size + redundancy, word_size);

    // TODO: free input_linear_system, input_unknowns, matrix B, matrix A, unknown indexes
    for (int i = 0; i < unknowns; i++) {
        free(input_linear_system->A[i]);
    }
    free(input_linear_system->A);
//    for (int i = 0; i < unknowns; i++) {
//        free(B[i]);
//    }
    free(input_linear_system->B);
    free(input_linear_system);
    free(unknown_indexes);
    free(input_unknowns);

    return block;
}

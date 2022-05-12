//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain on 29/04/22.                            //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include <string.h>

// Setup global variables
uint32_t block_size = 3;
uint64_t word_size = 3;

// Struct used by function to test
typedef struct {
    uint8_t* unknown_map;
    uint8_t unknowns_amount;
} unknowns_t;

// Function to test
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

void test_FLW() {
    uint8_t** block = malloc(sizeof(uint8_t*) * 7);
    for (int32_t i = 0; i < 7; ++i) {
        block[i] = malloc(sizeof(uint8_t) * 3);
    }
    block[0][0] = 0;
    block[0][1] = 0;
    block[0][2] = 0;
    block[1][0] = 111;
    block[1][1] = 118;
    block[1][2] = 101;
    block[2][0] = 0;
    block[2][1] = 0;
    block[2][2] = 0;
    block[3][0] = 151;
    block[3][1] = 140;
    block[3][2] = 120;
    block[4][0] = 15;
    block[4][1] = 96;
    block[4][2] = 173;
    block[5][0] = 70;
    block[5][1] = 82;
    block[5][2] = 203;
    block[6][0] = 214;
    block[6][1] = 245;
    block[6][2] = 65;
    uint8_t* correct_map = malloc(sizeof(uint8_t)*3);
    correct_map[0] = 1;
    correct_map[1] = 0;
    correct_map[2] = 1;
    uint8_t correct_amount = 2;
    unknowns_t* input_unknows = find_lost_words(block,block_size);
    for (int32_t i = 0; i < block_size; ++i) {
        CU_ASSERT_EQUAL(input_unknows->unknown_map[i],correct_map[i]);
    }
    CU_ASSERT_EQUAL(input_unknows->unknowns_amount,correct_amount);
    free(block);
    free(correct_map);
    free(input_unknows);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("findlostwords", 0, 0);
    CU_add_test(suite, "correct_findlostwords", test_FLW);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

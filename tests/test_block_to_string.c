//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain on 28/04/22.                            //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include <string.h>
#include "../headers/global.h"


// Function to test
char* block_to_string(uint8_t** block, uint32_t size) {
    // Allocate memory for the returned string
    char* str = malloc(sizeof(char) * ((size * word_size)+1));
    if (str == NULL) exit(EXIT_FAILURE);

    // Record block elements in the string array
    int index = 0;
    for (int32_t i = 0; i < size; i++) {
        for (int32_t j = 0; j < word_size; j++) {
            // Stop at the first 0 we meet
            if (block[i][j] == 0) {
                // Add end of string and return
                str[index] = '\0';
                return str;
            }
            str[index] = (char) block[i][j];
            index++;
        }
    }

    // Add end of string and return
    str[index] = '\0';
    return str;
}

void test_BTS() {
    uint32_t size = 3;
    uint8_t** current_block = malloc(sizeof(uint8_t*) * 7);
    for (int32_t i = 0; i < 7; ++i) {
        current_block[i] = malloc(sizeof(uint8_t) * 3);
    }
    current_block[0][0] = 112;
    current_block[0][1] = 114;
    current_block[0][2] = 111;
    current_block[1][0] = 103;
    current_block[1][1] = 114;
    current_block[1][2] = 97;
    current_block[2][0] = 109;
    current_block[2][1] = 109;
    current_block[2][2] = 105;
    current_block[3][0] = 229;
    current_block[3][1] = 39;
    current_block[3][2] = 229;
    current_block[4][0] = 52;
    current_block[4][1] = 38;
    current_block[4][2] = 0;
    current_block[5][0] = 63;
    current_block[5][1] = 219;
    current_block[5][2] = 232;
    current_block[6][0] = 147;
    current_block[6][1] = 82;
    current_block[6][2] = 111;
    char* correct_str = "programmi";
    char* str = block_to_string(current_block,size);
    int size_of_str = sizeof(correct_str) / sizeof(correct_str[0]);
    for (int32_t i = 0; i < size_of_str; ++i) {
        CU_ASSERT_EQUAL(str[i],correct_str[i]);
    }
    free(current_block);
    free(str);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("blocktostring", 0, 0);
    CU_add_test(suite, "correct_blocktostring", test_BTS);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

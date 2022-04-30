//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain                                         //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include <stdint.h>

// Import function to test
#include "../headers/system.h"

void test_gen_coefs(){
    uint32_t seed = 12345;
    uint32_t block_size = 10;
    uint32_t redudancy = 2;
    uint8_t** coeffs = gen_coefs(seed,redudancy, block_size);

    uint8_t** correct_coeffs = gen_coefs(seed,redudancy, block_size);
    for (int i = 0; i < redudancy; ++i) {
        correct_coeffs[i] = malloc(sizeof(uint8_t)*block_size);
    }
    correct_coeffs[0][0] = 126;
    correct_coeffs[0][1] = 101;
    correct_coeffs[0][2] = 239;
    correct_coeffs[0][3] = 226;
    correct_coeffs[0][4] = 104;
    correct_coeffs[0][5] = 130;
    correct_coeffs[0][6] = 6;
    correct_coeffs[0][7] = 188;
    correct_coeffs[0][8] = 113;
    correct_coeffs[0][9] = 120;
    correct_coeffs[1][0] = 70;
    correct_coeffs[1][1] = 70;
    correct_coeffs[1][2] = 225;
    correct_coeffs[1][3] = 94;
    correct_coeffs[1][4] = 208;
    correct_coeffs[1][5] = 238;
    correct_coeffs[1][6] = 40;
    correct_coeffs[1][7] = 144;
    correct_coeffs[1][8] = 130;
    correct_coeffs[1][9] = 153;
    for (int i = 0; i < redudancy; ++i) {
        for (int j = 0; j < block_size; ++j) {
            CU_ASSERT_EQUAL(coeffs[i][j], correct_coeffs[i][j]);

        }
        printf("\n");
    }
    free(correct_coeffs);
    free(coeffs);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("gencoeffs", 0, 0);
    CU_add_test(suite, "correct_gencoeffs", test_gen_coefs);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
}

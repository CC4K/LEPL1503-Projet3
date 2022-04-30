//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Pierre                                         //
//===========================================================//

// Libraries
#include <CUnit/Basic.h>
#include <stdlib.h>

// Import function to test
#include "../headers/system.h"

void test_gaussian() {
    uint8_t** A = malloc(sizeof(uint8_t *));
    A[0] = malloc(sizeof(uint8_t));
    uint8_t** b = malloc(sizeof(uint8_t *));
    b[0] = malloc(sizeof(uint8_t)*3);

    uint8_t **expected_res = malloc(sizeof(uint8_t*));
    expected_res[0] = malloc(sizeof(uint8_t)*3);

    uint32_t symbol_size = 3;
    uint32_t system_size = 1;

    A[0][0] = 165;

    b[0][0] = 141;
    b[0][1] = 253;
    b[0][2] = 0;

    expected_res[0][0] = 58;
    expected_res[0][1] = 41;
    expected_res[0][2] = 0;
    gf_256_gaussian_elimination(A,b,symbol_size,system_size);

    for (int i = 0; i < 3; ++i){
            CU_ASSERT_EQUAL(b[0][i], expected_res[0][i]);
    }
    free(A);
    free(b);
    free(expected_res);
}

int main(){
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("gaussian", 0, 0);
    CU_add_test(suite, "correct_gaussian", test_gaussian);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

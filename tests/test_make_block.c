//
// Created by romain on 27/04/22.
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

uint8_t** make_block(uint8_t* data, uint8_t size) {
    // Fait par Jacques le 12/04/22

    // Allocate memory for the returned block
    printf("size %d\n",size);
    uint8_t** block = malloc(sizeof(uint8_t*) * (size + 4));
    if(block == NULL) return NULL;
    for (int i = 0; i < (size + 4); i++) {
        block[i] = malloc(sizeof(uint8_t) * 3);
        if (block[i] == NULL) return NULL;
    }

    for (int i = 0; i < (size + 4) ; i++) {
        for (int j = 0; j < 3; j++) {
            block[i][j] = data[i * 3 + j];
        }
    }
    for (int i = 0; i < (size + 4); ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("block: %d\n",block[i][j]);
        }
        printf("\n");
    }

    return block;
}


void test_make_block() {
    uint8_t* temps_buf = malloc(sizeof(uint8_t)*21);
    temps_buf[0] = 0;
    temps_buf[1] = 0;
    temps_buf[2] = 0;
    temps_buf[3] = 103;
    temps_buf[4] = 114;
    temps_buf[5] = 97;
    temps_buf[6] = 0;
    temps_buf[7] = 0;
    temps_buf[8] = 0;
    temps_buf[9] = 229;
    temps_buf[10] = 39;
    temps_buf[11] = 229;
    temps_buf[12] = 52;
    temps_buf[13] = 38;
    temps_buf[14] = 0;
    temps_buf[15] = 63;
    temps_buf[16] = 219;
    temps_buf[17] = 232;
    temps_buf[18] = 147;
    temps_buf[19] = 82;
    temps_buf[20] = 111;
    uint8_t size = 3;
    uint8_t** correct_block = malloc(sizeof(uint8_t*)*7);
    for (int i = 0; i < 7; ++i) {
        correct_block[i] = malloc(sizeof(uint8_t)*3);
    }
    correct_block[0][0] = 0;
    correct_block[0][1] = 0;
    correct_block[0][2] = 0;
    correct_block[1][0] = 103;
    correct_block[1][1] = 114;
    correct_block[1][2] = 97;
    correct_block[2][0] = 0;
    correct_block[2][1] = 0;
    correct_block[2][2] = 0;
    correct_block[3][0] = 229;
    correct_block[3][1] = 39;
    correct_block[3][2] = 229;
    correct_block[4][0] = 52;
    correct_block[4][1] = 38;
    correct_block[4][2] = 0;
    correct_block[5][0] = 63;
    correct_block[5][1] = 219;
    correct_block[5][2] = 232;
    correct_block[6][0] = 147;
    correct_block[6][1] = 82;
    correct_block[6][2] = 111;

    uint8_t** current_block = make_block(temps_buf,size);
    for (int i = 0; i < 7; ++i) {
        for (int j = 0; j < 3; ++j) {
            CU_ASSERT_EQUAL(current_block[i][j],correct_block[i][j]);
        }
    }
    free(current_block);
    free(correct_block);
    free(temps_buf);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("makeblock", 0, 0);
    CU_add_test(suite, "correct_makeblock", test_make_block);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
}


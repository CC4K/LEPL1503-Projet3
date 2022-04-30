//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain on 29/04/22.                            //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include <string.h>

// Global variables
#define SIZE 2048
uint32_t block_size = 3;
uint64_t word_size = 3;

// Function to test
void write_last_block(FILE* output_file, uint8_t** block, uint8_t size, uint8_t word_size, uint8_t last_word_size) {
    for (int i = 0; i < size-1; i++) {
        for (int j = 0; j < word_size; j++) {
            if ((output_file == stdout) || (output_file == stderr)) {
                printf("%c", (char) block[i][j]);
            }
            else {
                fprintf(output_file, "%c",  block[i][j]);
            }
        }
    }

    for (int i = 0; i < last_word_size; i++) {
        if ((output_file == stdout) || (output_file == stderr)) {
            printf("%c", (char) block[size - 1][i]);
        }
        else {
            fprintf(output_file, "%c", (char) block[size - 1][i]);
        }
    }
}

void test_WLB() {
    FILE* output = fopen("tests/test_write_last_block.txt", "w+");
    uint8_t** current_block = malloc(sizeof(uint8_t*)*6);
    for (int i = 0; i < 6; ++i) {
        current_block[i] = malloc(sizeof(uint8_t)*3);
    }
    current_block[0][0] = 110;
    current_block[0][1] = 103;
    current_block[0][2] = 32;
    current_block[1][0] = 58;
    current_block[1][1] = 41;
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

    write_last_block(output,current_block,2,word_size,2);

    char response[SIZE];
    rewind(output);
    fgets(response,SIZE,output);
    CU_ASSERT_STRING_EQUAL(response,"ng :)");
    free(current_block);
    fclose(output);

}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("writelastblock", 0, 0);
    CU_add_test(suite, "correct_writelastblock", test_WLB);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
}

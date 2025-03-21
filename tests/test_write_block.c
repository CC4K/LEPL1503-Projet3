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
void write_block(FILE* output_file, uint8_t** block, uint32_t size, uint64_t word_size) {
    for (int32_t i = 0; i < size; i++) {
        for (int32_t j = 0; j < word_size; j++) {
            if ((output_file == stdout) || (output_file == stderr)) {
                printf("%c", (char) block[i][j]);
            }
            else {
                fprintf(output_file, "%c", (char) (block[i][j]));
            }
        }
    }
}

void test_write_block() {
    FILE* output = fopen("tests/test_write_block.txt", "w+");
    uint8_t** current_block = malloc(sizeof(uint8_t*) * 7);
    if (current_block == NULL) exit(EXIT_FAILURE);
    for (int32_t i = 0; i < 7; i++) {
        current_block[i] = malloc(sizeof(uint8_t) * 3);
        if (current_block[i] == NULL) exit(EXIT_FAILURE);
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

    write_block(output,current_block,block_size,word_size);
    char response[SIZE];
    rewind(output);
    fgets(response,SIZE,output);
    CU_ASSERT_STRING_EQUAL(response,"programmi");
    free(current_block);
    fclose(output);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("writeblock", 0, 0);
    CU_add_test(suite, "correct_writeblock", test_write_block);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

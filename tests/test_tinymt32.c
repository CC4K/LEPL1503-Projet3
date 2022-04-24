#include <CUnit/Basic.h>
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include "../headers/gf256_tables.h"
#include "../headers/system.h"
#include "../headers/tinymt32.h"

//=============================================================//
void printf_matrix(uint8_t** matrix, uint8_t n, uint8_t m) {
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


// test_tinymt32
/*
void test_tinymt32_gen_42()
{
    tinymt32_t prng;
    memset(&prng, 0, sizeof(tinymt32_t));
    prng.mat1 = 0x8f7011ee;
    prng.mat2 = 0xfc78ff1f;
    prng.tmat = 0x3793fdff;
    tinymt32_init(&prng, 42);

    uint8_t expected_res[] = {
        171, 165, 55, 61, 69, 143, 152, 158, 168, 64, 5, 91, 215, 134, 85, 17, 29, 41, 122, 169, 75, 0, 84, 85, 231, 156, 156, 201, 100, 17, 96, 137, 207, 72, 15, 182, 183, 154, 222, 92, 143, 153, 105, 58, 56, 239, 37, 75, 40, 133, 106, 199, 200, 144, 136, 175, 219, 87, 100, 59, 62, 26, 208, 19, 194, 227, 31, 187, 176, 225, 189, 120, 189, 78, 13, 112, 12, 47, 80, 39, 149, 170, 184, 23, 227, 247, 99, 218, 163, 140, 58, 248, 171, 12, 99, 5, 78, 178, 155, 127, 138, 37, 12, 54, 142, 80, 96, 206, 24, 23, 127, 242, 106, 86, 135, 185, 255, 157, 1, 91, 213, 135, 75, 87, 216, 233, 11, 7, 235, 168, 31, 229, 231, 29, 51, 105, 163, 139, 156, 104, 119, 247, 158, 210, 158, 15, 120, 27, 237, 28, 31, 23, 212, 240, 44, 95, 201, 157, 168, 14, 20, 10, 222, 208, 34, 137, 217, 221, 111, 155, 4, 97, 119, 31, 132, 108, 62, 135, 76, 148, 18, 249, 169, 95, 162, 18, 68, 109, 50, 102, 216, 62, 204, 16, 160, 5, 205, 223, 75, 187, 239, 64, 64, 135, 1, 160, 141, 0, 132, 64, 59, 35, 24, 245, 245, 132, 126, 134, 41, 236, 88, 155, 108, 12, 100, 214, 87, 67, 172, 46, 71, 217, 97, 36, 37, 26, 87, 204, 17, 159, 91, 98, 64, 78, 87, 134, 18, 165, 233, 187, 71, 90, 71, 52, 203, 208, 145, 248, 42, 228, 214, 100, 243, 155, 183, 250, 126, 133, 243, 75, 236, 91, 177, 213, 20, 166, 198, 41, 112, 255, 152, 84, 32, 27, 76, 182, 47, 39, 226, 11, 226, 87, 162, 62, 51, 237, 107, 147, 230, 127, 149, 222, 94, 29, 194, 250, 60, 19, 215, 38, 187, 6, 254, 186, 227, 57, 158, 134, 230, 239, 185, 211, 91, 118, 187, 251, 221, 194, 54, 15, 90, 181, 219, 232, 180, 182, 94, 86, 43, 114, 132, 195, 222, 144, 118, 90, 135, 118, 204, 98, 116, 235, 198, 181, 229, 37, 208, 204, 55, 82, 123, 172, 185, 228, 57, 248, 248, 144, 154, 1, 172, 228, 233, 203, 137, 17, 4, 226, 23, 229, 119, 168, 223, 145, 203, 71, 54, 252, 34, 26, 36, 89, 35, 246, 2, 175, 191, 162, 60, 49, 171, 0, 2, 46, 248, 193, 114, 96, 174, 188, 240, 100, 238, 73, 142, 31, 14, 42, 161, 58, 193, 142, 116, 147, 9, 130, 60, 95, 155, 133, 200, 245, 250, 179, 29, 55, 208, 79, 110, 152, 214, 50, 115, 115, 224, 184, 74, 73, 209, 151, 127, 167, 22, 111, 127, 1, 93, 95, 196, 49, 17, 211, 71, 141, 45, 127, 2, 151, 190, 205, 1, 90, 76, 59, 176, 146, 33, 237, 71, 27, 227, 97, 70, 24, 87, 209, 237, 105, 8, 93, 81, 144, 171, 189, 16, 9, 34, 196, 201, 205, 179, 224, 34, 122, 118, 117, 199, 169, 109, 65, 180, 219, 186, 39, 221, 87, 151, 246, 243, 29, 172, 162, 233, 226, 96, 253, 140, 203, 181, 167, 49, 200, 72, 23, 232, 58, 45, 60, 14, 128, 78, 228, 204, 152, 80, 101, 105, 178, 44, 99, 218, 209, 97, 54, 5, 178, 246, 247, 232, 244, 130, 146, 67, 2, 219, 175, 142, 158, 174, 21, 26, 168, 203, 86, 242, 93, 239, 164, 26, 64, 150, 184, 71, 165, 152, 109, 163, 3, 42, 16, 213, 73, 160, 188, 74, 104, 219, 167, 197, 207, 227, 108, 3, 144, 150, 49, 174, 233, 119, 151, 161, 33, 242, 249, 94, 57, 101, 61, 241, 93, 86, 240, 245, 111, 87, 110, 230, 134, 205, 255, 37, 163, 156, 93, 150, 68, 131, 11, 8, 24, 46, 196, 151, 88, 89, 93, 241, 28, 194, 15, 176, 91, 60, 92, 126, 238, 251, 95, 153, 174, 108, 251, 118, 202, 216, 96, 207, 176, 7, 174, 1, 68, 116, 0, 252, 225, 248, 0, 85, 247, 222, 93, 57, 61, 224, 53, 210, 1, 150, 47, 165, 152, 185, 253, 6, 213, 158, 90, 29, 210, 130, 39, 11, 125, 1, 75, 16, 145, 203, 247, 8, 140, 72, 168, 199, 40, 59, 222, 183, 133, 102, 65, 161, 156, 163, 8, 27, 136, 185, 146, 44, 128, 64, 8, 73, 124, 241, 51, 157, 250, 110, 114, 213, 36, 153, 83, 24, 59, 38, 33, 9, 46, 232, 221, 136, 181, 11, 234, 247, 177, 206, 34, 232, 136, 10, 73, 190, 50, 198, 140, 132, 107, 83, 71, 146, 210, 198, 4, 104, 118, 246, 200, 168, 179, 136, 116, 215, 60, 71, 250, 225, 56, 199, 96, 181, 110, 138, 57, 115, 121, 123, 103, 136, 82, 21, 212, 236, 49, 26, 88, 239, 204, 147, 213, 9, 4, 112, 82, 12, 230, 193, 43, 10, 250, 172, 29, 101, 26, 101, 122, 96, 113, 164, 29, 43, 32, 5, 255, 124, 10, 43, 190, 207, 19, 210, 61, 143, 106, 229, 73, 63, 110, 38, 112, 243, 99, 121, 43, 148, 221, 141, 253, 90, 238, 58, 240, 172, 79, 8, 101, 139, 251, 216, 8, 67, 68, 164, 119, 160, 76, 54, 239, 1, 204, 208, 105, 93, 26, 199, 139, 231, 102, 183, 70, 168, 193, 179, 154, 83, 38, 11, 110, 174, 140, 14, 15, 35, 43, 217, 31, 174, 124, 237, 144, 185, 47, 48, 51, 170, 189, 156, 37, 234, 121, 10, 220, 17, 198, 81, 168, 93, 11, 240, 121, 97, 105, 188, 114, 133, 188, 20, 45, 130, 214, 241, 24, 100, 134, 112, 225, 203, 121, 141, 239, 31, 125, 13, 215, 251, 91, 165, 178, 160, 86, 152, 207, 185, 134, 21, 127, 101, 208, 217, 159, 134, 242, 35, 231, 123, 183, 172, 84, 111, 132, 74, 89, 1, 85, 217, 138, 206, 139, 222, 224, 126, 221, 96, 118, 138, 81
    };

    for (int i = 0; i < 1000; ++i)
    {
        uint8_t coef = (uint8_t)tinymt32_generate_uint32(&prng);
        CU_ASSERT_EQUAL(coef, expected_res[i]);
    }
}
*/

// gauss_elimin
/*
void test_gaussian() {
    uint8_t** A = malloc(sizeof(int*)*2);
    uint8_t** b = malloc(sizeof(int*)*2);
    uint8_t** expected_b = malloc(sizeof(int*)*2);
    uint8_t** expected_a = malloc(sizeof(int*)*2);
    uint32_t symbol_size = 3;
    uint32_t system_size = 3;
    for (int i = 0; i < 3; ++i) {
        A[i] = malloc(sizeof(int)*2);
        b[i] = malloc(sizeof(int));
        expected_a[i] = malloc(sizeof(int)*2);
        expected_b[i] = malloc(sizeof(int));
    }
    b[0][0] = 57;
    b[0][1] = 148;
    b[0][2] = 214;
    b[1][0] = 63;
    b[1][1] = 140;
    b[1][2] = 157;
    expected_a[0][0] = 171;
    expected_a[0][1] = 165;
    expected_a[1][0] = 0;
    expected_a[1][1] = 78;
    A[0][0] = 171;
    A[0][1] = 165;
    A[1][0] = 61;
    A[1][1] = 69;
    printf("b avant: \n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%" PRId8 "\t", b[i][j]);
        }
        printf("\n");
    }
    gf_256_gaussian_elimination(A,b,symbol_size,system_size);
    printf("A :\n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf("%" PRId8 "\t", A[i][j]);
        }
        printf("\n");
    }
    printf("expected A :\n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf("%" PRId8 "\t", expected_a[i][j]);
        }
        printf("\n");
    }
    printf("b :\n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%" PRId8 "\t", b[i][j]);
        }
        printf("\n");
    }



    printf("End of test\n");


}
*/

// make_linear_system
/*
void test_MLS() {
    uint8_t** coeffs = malloc(sizeof(uint8_t*)*4);
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
    coeffs[1][1] = 5;
    coeffs[1][2] = 91;
    uint8_t nb_unk = 1;
    uint8_t block_size = 2;
    uint8_t** current_block = malloc(sizeof(uint8_t*)*6);  // [[110,103,32],[0,0,0],[48,218,196],[135,164,243],[122,252,234],[80,117,232]];
    uint8_t* unknown_indexes = malloc(sizeof(uint8_t*));
    current_block[0][0] = 110;
    current_block[0][1] = 103;
    current_block[0][2] = 32;
    current_block[1][0] = 0;
    current_block[1][1] = 0;
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
    unknown_indexes[0] = 0;
    unknown_indexes[1] = 1;
    uint8_t** test = make_linear_system(unknown_indexes,nb_unk,current_block,block_size);
    printf("A :\n");
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 2; ++j) {
            printf("%" PRId8 "\t", test[i][j]);
        }
        printf("\n");
    }
}
*/

// gen_coeffs
/*
void test_gen_coefs(){
    uint32_t seed = 12345;
    uint32_t block_size = 10;
    uint32_t redudancy = 2;
    uint8_t** caca = gen_coefs(seed,redudancy, block_size);
    for (int i = 0; i < redudancy; ++i) {
        for (int j = 0; j < block_size; ++j) {
            printf("%d ", caca[i][j]);

        }
        printf("\n");
    }
}
*/

// find_lost_words
/*
typedef struct {
    bool* unknown_map;
    uint8_t unknowns_amount;
} unknowns_t;

uint8_t word_size;

unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Initialize an array of boolean of size 'size' to false & the unknowns to 0
    bool unknown_indexes[size];
    for (int i = 0; i < size; i++) {
        unknown_indexes[i] = false;
    }
    uint8_t unknowns = 0;

    // Mapping the locations with lost values and counting the unknowns
    for (int i = 0; i < size; i++) {
        uint8_t count = 0;
        for (int j = 0; j < word_size; j++) {
            count += block[i][j];
        }
        // A symbol with only 0's is considered as lost
        if (count == 0) {
            unknown_indexes[i] = true;
            unknowns += 1;
        }
    }

    // Allocate memory to store the results in a struct and return it
    unknowns_t* output = malloc(sizeof(unknowns_t));
    if (output == NULL) return NULL;
    output->unknown_map = unknown_indexes;
    output->unknowns_amount = unknowns;

    return output;
}

void test_find_lost_1(){
    uint8_t size_1 = 3;
    word_size = 3;
    uint8_t** block_1 = malloc(sizeof(uint8_t*) * 7);
    for (int i = 0; i < 7; i++) {
        block_1[i] = malloc(sizeof(uint8_t) * size_1);
    }
    block_1[0][0] = 0;
    block_1[0][1] = 0;
    block_1[0][2] = 0;
    block_1[1][0] = 111;
    block_1[1][1] = 118;
    block_1[1][2] = 101;
    block_1[2][0] = 0;
    block_1[2][1] = 0;
    block_1[2][2] = 0;
    block_1[3][0] = 151;
    block_1[3][1] = 140;
    block_1[3][2] = 120;
    block_1[4][0] = 15;
    block_1[4][1] = 96;
    block_1[4][2] = 173;
    block_1[5][0] = 70;
    block_1[5][1] = 82;
    block_1[5][2] = 203;
    block_1[6][0] = 214;
    block_1[6][1] = 245;
    block_1[6][2] = 65;

    unknowns_t* out_1 = find_lost_words(block_1, size_1);
    bool* mapping_1 = out_1->unknown_map;
    uint8_t amount_1 = out_1->unknowns_amount;

    printf("Mapping_1:\n[");
    for (int i = 0; i < size_1; i++) {
        if (mapping_1[i]) {
            printf("true");
        }
        else {
            printf("false");
        }
        if (i != size_1 - 1) {
            printf(" ");
        }
    }
    printf("]\n");
    printf("Amount_1: %d\n", amount_1);
}

void test_find_lost_2(){
    uint8_t size_2 = 10;
    word_size = 20;
    uint8_t** block_2 = malloc(sizeof(uint8_t*) * 12);
    for (int i = 0; i < 12; i++) {
        block_2[i] = malloc(sizeof(uint8_t) * size_2);
    }
    block_2[0][0] = 110;
    block_2[0][1] = 115;
    block_2[0][2] = 117;
    block_2[0][3] = 105;
    block_2[0][4] = 116;
    block_2[0][5] = 101;
    block_2[0][6] = 44;
    block_2[0][7] = 32;
    block_2[0][8] = 97;
    block_2[0][9] = 106;
    block_2[0][10] = 111;
    block_2[0][11] = 117;
    block_2[0][12] = 116;
    block_2[0][13] = 101;
    block_2[0][14] = 114;
    block_2[0][15] = 32;
    block_2[0][16] = 117;
    block_2[0][17] = 110;
    block_2[0][18] = 101;
    block_2[0][19] = 32;

    block_2[1][0] = 99;
    block_2[1][1] = 111;
    block_2[1][2] = 117;
    block_2[1][3] = 99;
    block_2[1][4] = 104;
    block_2[1][5] = 101;
    block_2[1][6] = 32;
    block_2[1][7] = 100;
    block_2[1][8] = 101;
    block_2[1][9] = 32;
    block_2[1][10] = 109;
    block_2[1][11] = 195;
    block_2[1][12] = 169;
    block_2[1][13] = 108;
    block_2[1][14] = 97;
    block_2[1][15] = 110;
    block_2[1][16] = 103;
    block_2[1][17] = 101;
    block_2[1][18] = 32;
    block_2[1][19] = 97;

    block_2[2][0] = 117;
    block_2[2][1] = 32;
    block_2[2][2] = 109;
    block_2[2][3] = 97;
    block_2[2][4] = 115;
    block_2[2][5] = 99;
    block_2[2][6] = 97;
    block_2[2][7] = 114;
    block_2[2][8] = 112;
    block_2[2][9] = 111;
    block_2[2][10] = 110;
    block_2[2][11] = 101;
    block_2[2][12] = 44;
    block_2[2][13] = 32;
    block_2[2][14] = 112;
    block_2[2][15] = 117;
    block_2[2][16] = 105;
    block_2[2][17] = 115;
    block_2[2][18] = 32;
    block_2[2][19] = 112;

    block_2[3][0] = 97;
    block_2[3][1] = 114;
    block_2[3][2] = 32;
    block_2[3][3] = 100;
    block_2[3][4] = 101;
    block_2[3][5] = 115;
    block_2[3][6] = 115;
    block_2[3][7] = 117;
    block_2[3][8] = 115;
    block_2[3][9] = 44;
    block_2[3][10] = 32;
    block_2[3][11] = 108;
    block_2[3][12] = 101;
    block_2[3][13] = 115;
    block_2[3][14] = 32;
    block_2[3][15] = 102;
    block_2[3][16] = 114;
    block_2[3][17] = 117;
    block_2[3][18] = 105;
    block_2[3][19] = 116;

    block_2[4][0] = 115;
    block_2[4][1] = 32;
    block_2[4][2] = 114;
    block_2[4][3] = 111;
    block_2[4][4] = 117;
    block_2[4][5] = 103;
    block_2[4][6] = 101;
    block_2[4][7] = 115;
    block_2[4][8] = 46;
    block_2[4][9] = 10;
    block_2[4][10] = 195;
    block_2[4][11] = 137;
    block_2[4][12] = 116;
    block_2[4][13] = 97;
    block_2[4][14] = 112;
    block_2[4][15] = 101;
    block_2[4][16] = 32;
    block_2[4][17] = 56;
    block_2[4][18] = 10;
    block_2[4][19] = 10;

    block_2[5][0] = 82;
    block_2[5][1] = 195;
    block_2[5][2] = 169;
    block_2[5][3] = 112;
    block_2[5][4] = 195;
    block_2[5][5] = 169;
    block_2[5][6] = 116;
    block_2[5][7] = 101;
    block_2[5][8] = 114;
    block_2[5][9] = 32;
    block_2[5][10] = 108;
    block_2[5][11] = 39;
    block_2[5][12] = 111;
    block_2[5][13] = 112;
    block_2[5][14] = 195;
    block_2[5][15] = 169;
    block_2[5][16] = 114;
    block_2[5][17] = 97;
    block_2[5][18] = 116;
    block_2[5][19] = 105;

    block_2[6][0] = 0;
    block_2[6][1] = 0;
    block_2[6][2] = 0;
    block_2[6][3] = 0;
    block_2[6][4] = 0;
    block_2[6][5] = 0;
    block_2[6][6] = 0;
    block_2[6][7] = 0;
    block_2[6][8] = 0;
    block_2[6][9] = 0;
    block_2[6][10] = 0;
    block_2[6][11] = 0;
    block_2[6][12] = 0;
    block_2[6][13] = 0;
    block_2[6][14] = 0;
    block_2[6][15] = 0;
    block_2[6][16] = 0;
    block_2[6][17] = 0;
    block_2[6][18] = 0;
    block_2[6][19] = 0;

    block_2[7][0] = 118;
    block_2[7][1] = 111;
    block_2[7][2] = 116;
    block_2[7][3] = 114;
    block_2[7][4] = 101;
    block_2[7][5] = 32;
    block_2[7][6] = 112;
    block_2[7][7] = 108;
    block_2[7][8] = 97;
    block_2[7][9] = 116;
    block_2[7][10] = 44;
    block_2[7][11] = 32;
    block_2[7][12] = 101;
    block_2[7][13] = 116;
    block_2[7][14] = 32;
    block_2[7][15] = 102;
    block_2[7][16] = 105;
    block_2[7][17] = 110;
    block_2[7][18] = 105;
    block_2[7][19] = 114;

    block_2[8][0] = 32;
    block_2[8][1] = 112;
    block_2[8][2] = 97;
    block_2[8][3] = 114;
    block_2[8][4] = 32;
    block_2[8][5] = 108;
    block_2[8][6] = 101;
    block_2[8][7] = 115;
    block_2[8][8] = 32;
    block_2[8][9] = 102;
    block_2[8][10] = 114;
    block_2[8][11] = 117;
    block_2[8][12] = 105;
    block_2[8][13] = 116;
    block_2[8][14] = 115;
    block_2[8][15] = 32;
    block_2[8][16] = 114;
    block_2[8][17] = 111;
    block_2[8][18] = 117;
    block_2[8][19] = 103;

    block_2[9][0] = 101;
    block_2[9][1] = 115;
    block_2[9][2] = 46;
    block_2[9][3] = 0;
    block_2[9][4] = 0;
    block_2[9][5] = 0;
    block_2[9][6] = 0;
    block_2[9][7] = 0;
    block_2[9][8] = 0;
    block_2[9][9] = 0;
    block_2[9][10] = 0;
    block_2[9][11] = 0;
    block_2[9][12] = 0;
    block_2[9][13] = 0;
    block_2[9][14] = 0;
    block_2[9][15] = 0;
    block_2[9][16] = 0;
    block_2[9][17] = 0;
    block_2[9][18] = 0;
    block_2[9][19] = 0;

    block_2[10][0] = 162;
    block_2[10][1] = 156;
    block_2[10][2] = 193;
    block_2[10][3] = 244;
    block_2[10][4] = 155;
    block_2[10][5] = 195;
    block_2[10][6] = 239;
    block_2[10][7] = 229;
    block_2[10][8] = 112;
    block_2[10][9] = 56;
    block_2[10][10] = 28;
    block_2[10][11] = 119;
    block_2[10][12] = 197;
    block_2[10][13] = 243;
    block_2[10][14] = 245;
    block_2[10][15] = 253;
    block_2[10][16] = 189;
    block_2[10][17] = 147;
    block_2[10][18] = 251;
    block_2[10][19] = 113;

    block_2[11][0] = 175;
    block_2[11][1] = 146;
    block_2[11][2] = 62;
    block_2[11][3] = 139;
    block_2[11][4] = 247;
    block_2[11][5] = 229;
    block_2[11][6] = 35;
    block_2[11][7] = 227;
    block_2[11][8] = 247;
    block_2[11][9] = 101;
    block_2[11][10] = 134;
    block_2[11][11] = 72;
    block_2[11][12] = 183;
    block_2[11][13] = 17;
    block_2[11][14] = 211;
    block_2[11][15] = 206;
    block_2[11][16] = 214;
    block_2[11][17] = 164;
    block_2[11][18] = 101;
    block_2[11][19] = 127;

    unknowns_t* out_2 = find_lost_words(block_2, size_2);
    bool* mapping_2 = out_2->unknown_map;
    uint8_t amount_2 = out_2->unknowns_amount;

    printf("Mapping_2:\n[");
    for (int i = 0; i < size_2; i++) {
        if (mapping_2[i]) {
            printf("true");
        }
        else {
            printf("false");
        }
        if (i != size_2 - 1) {
            printf(" ");
        }
    }
    printf("]\n");
    printf("Amount_2: %d\n", amount_2);
}
*/

// make_block
/*
uint32_t word_size = 3;
uint32_t redundancy = 4;

uint8_t** make_block(uint8_t* data, uint8_t size) {
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

void test_make_block_1() {
    // Initialize data, size, redundancy & word_size
    uint8_t size_1 = 3;
    uint8_t* data_1 = malloc(sizeof(uint8_t) * size_1);
    data_1[0] = 0;
    data_1[1] = 0;
    data_1[2] = 0;
    data_1[3] = 111;
    data_1[4] = 118;
    data_1[5] = 101;
    data_1[6] = 0;
    data_1[7] = 0;
    data_1[8] = 0;
    data_1[9] = 151;
    data_1[10] = 140;
    data_1[11] = 120;
    data_1[12] = 15;
    data_1[13] = 96;
    data_1[14] = 173;
    data_1[15] = 70;
    data_1[16] = 82;
    data_1[17] = 203;
    data_1[18] = 214;
    data_1[19] = 245;
    data_1[20] = 65;

    uint8_t** out_1 = make_block(data_1, size_1);
    printf("[");
    for (int i = 0; i < size_1 + redundancy; i++) {
        if (i == 0) printf("[");
        else printf(" [");
        for (int j = 0; j < word_size; j++) {
            printf("%d\t", out_1[i][j]);
        }
        if (i == size_1 + redundancy - 1) printf("]");
        else printf("]\n");
    }
    printf("]\n");
}

void test_make_block_2() {
    // Initialize data, size, redundancy & word_size
    uint8_t size_2 = 2;
    uint8_t* data_2 = malloc(sizeof(uint8_t) * size_2);

    data_2[0] = 0;
    data_2[1] = 0;
    data_2[2] = 0;
    data_2[3] = 58;
    data_2[4] = 41;
    data_2[5] = 0;
    data_2[6] = 48;
    data_2[7] = 218;
    data_2[8] = 196;
    data_2[9] = 135;
    data_2[10] = 164;
    data_2[11] = 243;
    data_2[12] = 122;
    data_2[13] = 252;
    data_2[14] = 234;
    data_2[15] = 80;
    data_2[16] = 117;
    data_2[17] = 232;

    uint8_t** out_2 = make_block(data_2, size_2);
    printf("[");
    for (int i = 0; i < size_2 + redundancy; i++) {
        if (i == 0) printf("[");
        else printf(" [");
        for (int j = 0; j < word_size; j++) {
            printf("%d\t", out_2[i][j]);
        }
        if (i == size_2 + redundancy - 1) printf("]");
        else printf("]\n");
    }
    printf("]\n");
}
*/

// process_block
/*
uint8_t word_size;
uint8_t** coeffs;
typedef struct {
    uint8_t** A;
    uint8_t** B;
} linear_system_t;
linear_system_t* make_linear_system(bool* unknown_indexes,uint8_t nb_unk,uint8_t** current_block,uint8_t block_size) {
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
            if (unknown_indexes[j]) {
                A[i][temp] = coeffs[i][j];
                temp += 1;
            }
            else {
                B[i] = gf_256_full_add_vector(B[i], gf_256_mul_vector(current_block[j], coeffs[i][j], block_size),block_size);
            }
        }
    }

    // Allocate memory to store the results in a struct and return it
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) return NULL;
    output->A = A;
    output->B = B;
    return output;
}
typedef struct {
    bool* unknown_map;
    uint8_t unknowns_amount;
} unknowns_t;
unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Initialize an array of boolean of size 'size' to false & the unknowns to 0
    bool unknown_indexes[size];
    for (int i = 0; i < size; i++) {
        unknown_indexes[i] = false;
    }
    uint8_t unknowns = 0;

    // Mapping the locations with lost values and counting the unknowns
    for (int i = 0; i < size; i++) {
        uint8_t count = 0;
        for (int j = 0; j < word_size; j++) {
            count += block[i][j];
        }
        // A symbol with only 0's is considered as lost
        if (count == 0) {
            unknown_indexes[i] = true;
            unknowns += 1;
        }
    }

    // Allocate memory to store the results in a struct and return it
    unknowns_t* output = malloc(sizeof(unknowns_t));
    if (output == NULL) return NULL;
    output->unknown_map = unknown_indexes;
    output->unknowns_amount = unknowns;

    return output;
}
uint8_t** process_block(uint8_t** block, uint8_t size) {
    // Crée par Cédric le 13/04/22

    // Import the data from the other functions
    unknowns_t* input_unknowns = find_lost_words(block, size);
    bool* unknown_indexes = input_unknowns->unknown_map;
    uint8_t unknowns = input_unknowns->unknowns_amount;
    linear_system_t* input_linear_system = make_linear_system(unknown_indexes, unknowns, block, size);
    uint8_t** A = input_linear_system->A;
    uint8_t** B = input_linear_system->B;

    // Gaussian elimination 'in place'
    gf_256_gaussian_elimination(A, B, word_size, unknowns);

    // For each index marked as 'true', replace the data
    uint8_t temp = 0;
    for (int i = 0; i < size; i++) {
        if (unknown_indexes[i]) {
            //=============================================================//
//            printf("unknown_indexes[%d]: true\n", i);
            //=============================================================//
            block[i] = B[temp];
            temp += 1;
        }
        //=============================================================//
//        else printf("unknown_indexes[%d]: false\n", i);
        //=============================================================//
    }

    // Return the solved block
    return block;
}
void test_process_block(){
    coeffs = malloc(sizeof(uint8_t*) * 4);
    if (coeffs == NULL) return;
    for (int i = 0; i < 4; i++) {
        coeffs[i] = malloc(sizeof(uint8_t) * 3);
        if (coeffs[i] == NULL) return;
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

    uint8_t size = 3;
    word_size = 3;
    uint8_t** block = malloc(sizeof(uint8_t*) * 7);
    if (block == NULL) return;
    for (int i = 0; i < 7; i++) {
        block[i] = malloc(sizeof(uint8_t) * size);
        if (block[i] == NULL) return;
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
    //=============================================================//
    printf("block:\n");
    printf_matrix(block, 7, 3);
    //=============================================================//

    uint8_t** out = process_block(block, size);
    //=============================================================//
    printf("\nprocessed_block:\n");
    printf_matrix(out, 7, 3);
    //=============================================================//
}
*/

int main(){
//    CU_initialize_registry();
//    CU_pSuite suite = CU_add_suite("tinymt32", 0, 0);
//    CU_add_test(suite, "correct_coeffs", test_tinymt32_gen_42);
//    CU_basic_run_tests();
//    CU_basic_show_failures(CU_get_failure_list());
//    test_MLS();
//    test_gen_coefs();
//    test_find_lost_1();
//    test_find_lost_2();
//    test_make_block_1();
//    test_make_block_2();
//    test_process_block();
    printf("\nTHE END\n");
}
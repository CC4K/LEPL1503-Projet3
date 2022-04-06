#include "../headers/tinymt32.h"
#include <CUnit/Basic.h>
#include "../headers/system.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>

void test_gaussian() {

    uint8_t** A = malloc(sizeof(int*)*3);
    uint8_t** b = malloc(sizeof(int*)*3);
    uint8_t **expected = malloc(sizeof(int*)*3);
    uint32_t symbol_size = 3;
    uint32_t system_size = 3;
    for (int i = 0; i < 3; ++i) {
        A[i] = malloc(sizeof(int)*3);
        b[i] = malloc(sizeof(int));
        expected[i] = malloc(sizeof(int));
        b[i][0] = 1;
    }
    A[0][0] = 1;
    A[0][1] = 2;
    A[0][2] = 3;
    A[1][0] = 1;
    A[1][1] = 2;
    A[1][2] = 2;
    A[2][0] = 2;
    A[2][1] = 3;
    A[2][2] = 1;
    expected[0][0] = -1;
    expected[1][0] = 1;
    expected[2][0] = 0;
    gf_256_gaussian_elimination(A,b,symbol_size,system_size);
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            printf("%" PRId8 "\n", A[i][j]);
        }
    }
    for (int i = 0; i < 3; ++i) {
        if (b[i] != expected[i]) {
            printf("Nope\n");
        }
        printf("%" PRId8 "\n", *b[i]);
    }
    printf("Yep\n");
}

int main(){

}

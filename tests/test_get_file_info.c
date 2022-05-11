//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Romain on 30/04/22.                            //
//===========================================================//

// Libraries
#include <stdlib.h>
#include <stdio.h>
#include <CUnit/Basic.h>
#include <stdint.h>
#include <string.h>

// Setup structure used by function to test
typedef struct {
    uint32_t* seed;
    uint32_t* block_size;
    uint32_t* word_size;
    uint32_t* redundancy;
    uint64_t* message_size;
} file_data_t;
file_data_t* file_data;

// Function to test
file_data_t* get_file_info(char* filename) {
    // Fait par Jacques le 15/04/22

    // Allocate memory for the returned data
    file_data_t* output = malloc(sizeof(file_data_t));
    if (output == NULL) return NULL;

    FILE* fileptr;
    uint32_t* buf;

    // Open the file
    fileptr = fopen(filename, "rb");

    // Create a buffer which contains the first 24 bytes
    buf = malloc(4 * sizeof(uint32_t)+1 * sizeof(uint64_t));
    fread(buf,4 * sizeof(uint32_t)+1 * sizeof(uint64_t),1,fileptr);

    // Allocate memory for the structure pointers
    output->seed = malloc(sizeof(uint32_t));
    output->block_size = malloc(sizeof(uint32_t));
    output->word_size = malloc(sizeof(uint32_t));
    output->redundancy = malloc(sizeof(uint32_t));
    output->message_size = malloc(sizeof(uint64_t));

    // Check if malloc didn't fail
    if (output->seed == NULL) return NULL;
    if (output->block_size == NULL) return NULL;
    if (output->word_size == NULL) return NULL;
    if (output->redundancy == NULL) return NULL;
    if (output->message_size == NULL) return NULL;

    // Store each value
    *output->seed = be32toh((uint32_t) * buf);
    *output->block_size = be32toh((uint32_t) * (buf+1));
    *output->word_size = be32toh((uint32_t) * (buf+2));
    *output->redundancy = be32toh((uint32_t) * (buf+3));
    *output->message_size = be64toh(*((uint64_t *) buf+2));

    // Close the file
    fclose(fileptr);

    // Free the buffer
    free(buf);

    return output;
}

void test_GFI() {
    char* fullpath = "input_binary/small.bin";
    file_data = get_file_info(fullpath);
    if (file_data == NULL) {
        CU_FAIL("file_data is NULL");
    }
    CU_ASSERT_EQUAL(*file_data->seed,42);
    CU_ASSERT_EQUAL(*file_data->block_size,3);
    CU_ASSERT_EQUAL(*file_data->word_size,3);
    CU_ASSERT_EQUAL(*file_data->redundancy,4);
    CU_ASSERT_EQUAL(*file_data->message_size,23);
}

int main() {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("getfileinfo", 0, 0);
    CU_add_test(suite, "correct_getfileinfo", test_GFI);
    CU_basic_run_tests();
    CU_basic_show_failures(CU_get_failure_list());
    CU_cleanup_registry();
}

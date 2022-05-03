//
// Created by cedric_k on 3/05/22.
//

#ifndef GROUPEY2_FILE_DATA_H
#define GROUPEY2_FILE_DATA_H

// Structure to store file information
typedef struct {
    uint32_t* seed;
    uint32_t* block_size;
    uint32_t* word_size;
    uint32_t* redundancy;
    uint64_t* message_size;
} file_data_t;

#endif //GROUPEY2_FILE_DATA_H
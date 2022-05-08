#ifndef GROUPEY2_THREAD_INFO_H
#define GROUPEY2_THREAD_INFO_H

/**
 * Structure to store producer data
 */
typedef struct {
    FILE* input_file;
    FILE* output_stream;
    uint32_t nb_blocks;
    uint8_t* buf;
    uint8_t** coeffs;
    uint64_t word_size;
    uint32_t block_size;
    uint32_t redundancy;
    uint64_t message_size;
    bool verbose;
    uint64_t filelen;
    bool contains_uncomplete_block;
    uint8_t*** blocks;
    uint32_t nb_remaining_symbols;
    uint32_t true_length_last_symbols;
    uint8_t** decoded;

} thread_infos_t;

#endif //GROUPEY2_THREAD_INFO_H

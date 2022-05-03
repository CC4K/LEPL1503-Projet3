//
// Created by cedric_k on 3/05/22.
//

#ifndef GROUPEY2_ARGS_H
#define GROUPEY2_ARGS_H

// Structure for program args
typedef struct {
    DIR* input_dir;
    char input_dir_path[PATH_MAX];
    FILE* output_stream;
    uint8_t nb_threads;
    bool verbose;
    uint8_t** coeffs;
} args_t;

#endif //GROUPEY2_ARGS_H
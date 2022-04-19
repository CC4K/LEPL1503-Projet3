//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Jacques, Romain, Cédric & Pierre on 15/03/22.  //
//===========================================================//

//================= Libraries and imports ===================//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "headers/tinymt32.h"
#include "headers/system.h"

//====================== Structures =========================//

// Structure for program args
typedef struct {
    DIR* input_dir;
    char input_dir_path[PATH_MAX];
    FILE* output_stream;
    uint8_t nb_threads;
    bool verbose;
    uint8_t** coeffs;
} args_t;

// Structure for file informations
typedef struct {
    uint32_t* seed;
    uint32_t* block_size;
    uint32_t* word_size;
    uint32_t* redundancy;
    uint64_t* message_size;
} file_data_t;

// Structure for missing words
typedef struct {
    bool* unknown_map;
    uint8_t unknowns_amount;
} unknowns_t;

// Structure for linear system
typedef struct {
    uint8_t** A;
    uint8_t** B;
} linear_system_t;

//================== Global Variables =======================//
file_data_t* file_data;
uint8_t** coeffs = NULL;
uint8_t word_size = 0;

//====================== Functions ==========================//

/**
 * Build the block based on the data and the size of a block
 * @param data: the block data in binary form. If the input file is given, it will be cut
 *              'size' symbols of size 'word_size' bytes, followed by 'redundancy' symbols of size 'word_size'
 * @param size: number of source symbols in a block
 * @return block: the built block in the form of a matrix (one line = one symbol)
 */
uint8_t** make_block(uint8_t* data, uint8_t size) {
    // Fait par Jacques le 12/04/22
    // TODO: à verifier

    // Allocate memory for the returned block
    uint8_t** block = malloc(sizeof(uint8_t) * (size + *(file_data->redundancy)));
    if(block == NULL) return NULL;

    for (int i = 0; i < (size + *(file_data->redundancy)); i++) {
        block[i] = malloc(sizeof(uint8_t) * (*(file_data->word_size)));
        if (block[i] == NULL) return NULL;
    }

    for (int i = 0; i < (size + (*(file_data->redundancy))) ; i++) {
        for (int j = 0; j < (*(file_data->word_size)); j++) {
            block[i][j] = data[i * (*(file_data->word_size)) + j];
        }
    }
    return block;
}

/**
 * Based on a block, find the lost source symbols et index them in 'unknown_indexes'
 * A symbol is considered as lost in the block if the symbol only contains 0's
 * @param block: the said block
 * @param size: the size of the block
 * @return unknown_indexes: table of size 'size' mapping with source symbols
 *                          The input 'i' is 'true' if the source symbol 'i' is lost
 * @return unknwowns: the number of lost source symbols
 */
unknowns_t* find_lost_words(uint8_t** block, uint8_t size) {
    // Crée par Cédric le 15/04/22
    // TODO: à vérifier

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

/**
 * Build a linear system Ax=b from the blocks given in args
 * @param unknown_indexes: index of the lost source symbols of a block. The input 'i' is 'true' if the symbol 'i' is lost
 * @param nb_unk: the amount of unknowns in the system - the size of the system
 * @param current_block: the block of symbols to solve
 * @param block_size: the amount of source symbols in the block
 * @return A: the coefficients matrix
 * @return B: the independents terms vector. Each element of B is the same size as a data vector (packet)
 */
linear_system_t* make_linear_system(bool* unknown_indexes,uint8_t nb_unk,uint8_t** current_block,uint8_t block_size) {
    // Crée par Romain le 15/04/22

    // Allocate memory for the two matrices
    uint8_t** A = malloc(sizeof(uint8_t * ) * nb_unk);
    for (size_t i = 0; i < nb_unk; ++i) {
        A[i] = malloc(sizeof(uint8_t)*block_size);
    }
    uint8_t** B = malloc(sizeof(uint8_t * ) * nb_unk);
    for (size_t i = 0; i < nb_unk; ++i) {
        B[i] = malloc(sizeof(uint8_t)*word_size);
    }

    for (int i = 0; i < nb_unk; i++) {
        B[i] = current_block[block_size + i];
    }

    for (int i = 0; i < nb_unk; i++) {
        int temp = 0;
        for (int j = 0; j < block_size; j++) {
            if (unknown_indexes[j] == true) {
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

/**
 * Based on a block, find the unknowns (e.g., lost source symbols) and build the corresponding linear system.
 * This simple version considers there are always as many unknowns as there are redundancy symbols, that is to say
 * we will always build a system with as many equations as there are redundancy symbols
 * @param block: the said block
 * @param size: the size of the block
 * @return block: the solved block
 */
uint8_t** process_block(uint8_t** block, uint8_t size) {
    // Crée par Cédric le 13/04/22
    // TODO: à vérifier

    // Import the data from the other functions
    unknowns_t* input_unknowns = find_lost_words(block, size);
    bool* unknown_indexes = input_unknowns->unknown_map;
    uint8_t unknowns = input_unknowns->unknowns_amount;
    linear_system_t* input_linear_system = make_linear_system(unknown_indexes, unknowns, block, size);
    uint8_t** A = input_linear_system->A;
    uint8_t** B = input_linear_system->B;

    // Gaussian elimination 'in place'
    gf_256_gaussian_elimination(A, B, word_size, size);

    // For each index marked as 'true', replace the data
    uint8_t temp = 0;
    for (int i = 0; i < size; i++) {
        if (unknown_indexes[i]) {
            block[i] = B[temp];
            temp += 1;
        }
    }

    // Return the solved block
    return block;
}

/**
 * Help function. Returns a string stored in binary in the given block
 * @param block: the said block
 * @param size: the size of the block
 * @return str: the block's string converted into binary
 */
char* block_to_string(uint8_t** block, uint32_t size) {
    // Fait par jacques le 13/04/22
    // fonctionne et testé avec matrice carré seulement
    // TODO: verifier si la matrice est carré ou pas

    // Allocate memory for the returned string
    char* str = malloc(sizeof(char) * (size * size)); // TODO: verifier si bien une matrice carrée
    if(str == NULL) return NULL;

    int index = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) { // TODO: la meme vérifier la taille de la matrice
            if (block[i][j] == 0) {
                return str;
            }
            str[index] = (char) block[i][j];
            index++;
        }
    }
    str[index] = '\0';
    return str;
}

/**
 * Writes the block in binary in 'output_file'
 * @param output_file: the output file
 * @param block: the said block
 * @param size: the size of the block
 * @param word_size: the size of each symbol in the block
 */
void write_block(FILE* output_file, uint8_t** block, uint8_t size, uint8_t word_size) {
    typedef unsigned char Byte;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < word_size; j++) {
            if ((output_file == stdout) || (output_file == stderr)) {
                printf("%c", (char) block[i][j]);
            }
            else {
                fprintf(output_file, "%d", (Byte) block[i][j]);
            }
        }
    }
}

/**
 * Writes in the block in binary in the file 'output_file'. This function is different from the last since it needs to
 * manage the case where the last symbol of the last block is not of the same size as the other symbols of its block
 * @param output_file: the output_file
 * @param block: the said block
 * @param size: the size of the block
 * @param word_size: the size of a 'full' symbol
 * @param last_word_size: the size of the very last word of the last block
 */
void write_last_block(FILE* output_file, uint8_t** block, uint8_t size, uint8_t word_size, uint8_t last_word_size) {
    typedef unsigned char Byte;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < word_size; j++) {
            if ((output_file == stdout) || (output_file == stderr)) {
                printf("%c", (char) block[i][j]);
            }
            else {
                fprintf(output_file, "%d", (Byte) block[i][j]);
            }
        }
    }

    for (int i = 0; i < last_word_size; i++) {
        if ((output_file == stdout) || (output_file == stderr)) {
            printf("%c", (char) block[size - 1][i]);
        }
        else {
            fprintf(output_file, "%d", (Byte) block[size - 1][i]);
        }
    }
}

/**
 * Retrieve the data from the 'data' block as specified in the statement
 * @param filename: the file's 'Absolute Path'
 * @return output: a structure which contains pointers to the seed, the word_size, the block_size, the redundancy and message_size
 *                 - seed: the seed for random numbers generation
 *                 - block_size: size of a block - size of source symbols in the block
 *                 - word_size: the size of a 'full' symbol in a block
 *                 - redundancy: the amount of redundancy symbols in the block
 *                 - message_size: the size (in bytes) of the original file we wish to recover.
 *                                 This value only take into consideration the file data thus without recovery symbols
 *                                 nor the data listed above
 */
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
    *output->message_size = be64toh((uint64_t) * (buf+2));

    // Close the file
    fclose(fileptr);

    // Free the buffer
    free(buf);

    return output;
}

/**
 * Shows how to run a program
 * @param prog_name
 */
void usage(char* prog_name) {
    fprintf(stderr, "USAGE:\n");
    fprintf(stderr, "    %s [OPTIONS] input_dir\n", prog_name);
    fprintf(stderr, "    input_dir: path to the directory containing the instance files with the encoded messages\n");
    fprintf(stderr, "    -f output_file: path to the output file containing all decoded messages\n");
    fprintf(stderr, "    -n n_threads (default: 4): set the number of computing threads that will be used to execute the RLC algorithm\n");
    fprintf(stderr, "    -v : enable debugging messages. If not set, no such messages will be displayed (except error messages on failure)\n");
}


int parse_args(args_t* args, int argc, char* argv[]){
    memset(args, 0, sizeof(args_t));

    // Default values of the arguments
    args->nb_threads = 4;
    args->verbose = false;
    args->output_stream = stdout;
    int opt;
    while ((opt = getopt(argc, argv, "n:vf:")) != -1) {
        switch (opt) {
            case 'n':
                args->nb_threads = atoi(optarg);
                if (args->nb_threads == 0) {
                    fprintf(stderr, "The number of computing threads must be a positive integer, got: %s\n", optarg);
                    return -1;
                }
                break;
            case 'v':
                args->verbose = true;
                break;
            case 'f':
                args->output_stream = fopen(optarg, "w");
                if (args->output_stream == NULL) {
                    fprintf(stderr, "Impossible to open the output file %s: %s\n", optarg, strerror(errno));
                    return -1;
                }
                break;
            case '?':
                usage(argv[0]);
                return 1;
            default:
                usage(argv[0]);
        }
    }

    if (optind == argc) {
        fprintf(stderr, "You must provide an input directory containing the instance files!\n");
        return -1;
    }

    // Source: https://stackoverflow.com/questions/11736060/how-to-read-all-files-in-a-folder-using-c
    if (NULL == (args->input_dir = opendir(argv[optind]))) {
        fprintf(stderr, "Impossible to open the directory containing the input instance files %s: %s\n", argv[optind], strerror(errno));
        return -1;
    }
    // The following line is not very secure... Ask Prof. Legay and/or wait for LINGI2144 for more information :-)
    strcpy(args->input_dir_path, argv[optind++]);

    return 0;
}

//===================== MAIN FUNCTION =======================//

int main(int argc, char* argv[]) {
    args_t args;
    int err = parse_args(&args, argc, argv);
    if (err == -1) exit(EXIT_FAILURE);

    else if (err == 1) exit(EXIT_SUCCESS);

    // The following lines (and every code already present in this skeleton) can be removed, it is just an example to show you how to use the program arguments
    fprintf(stderr, "\tnumber of threads executing the RLC decoding algorithm in parallel: %" PRIu32 "\n", args.nb_threads);
    fprintf(stderr, "\tverbose mode: %s\n", args.verbose ? "enabled" : "disabled");

    // This is an example of how to open the instance files of the input directory. You may move/edit it during the project
    struct dirent *directory_entry;
    FILE *input_file;
    while ((directory_entry = readdir(args.input_dir))) {
        // Ignore parent and current directory
        if (!strcmp(directory_entry->d_name, ".")) continue;

        if (!strcmp(directory_entry->d_name, "..")) continue;

        // Add the directory path to the filename to open it
        char full_path[PATH_MAX];
        memset(full_path, 0, sizeof(char) * PATH_MAX);
        strcpy(full_path, args.input_dir_path);
        strcat(full_path, "/");
        strcat(full_path, directory_entry->d_name);

        input_file = fopen(full_path, "r");
        if (input_file == NULL) {
            fprintf(stderr, "Failed to open the input file %s: %s\n", full_path, strerror(errno));
            goto file_read_error;
        }
        if (args.verbose) {
            // This is a simple example of how to use the verbose mode
            fprintf(stderr, "Successfully opened the file %s\n", full_path);
        }

        // TODO: parse the input binary file, decode the encoded message with RLC and write the output in the output stream following the statement
        // retourne structure avec seed, word_size ...
        // TODO: vérifié si c'est bien de faire une variable global ou si on fait le malloc ici
        file_data = get_file_info(full_path);

        if (args.verbose) {
            printf("Information of the file :");
            printf("Seed : %d", *file_data->seed);
            printf("Block_size : %d", *file_data->block_size);
            printf("Word_size : %d", *file_data->word_size);
            printf("Redundancy : %d", *file_data->redundancy);
        }

        //TODO: avancer le curseur du fichier apres les 24 premiers Bytes


        // Malloc inside function
        // TODO: nss & nrs not allocate
        uint32_t nss;
        uint32_t nrs;
        coeffs = gen_coefs(*file_data->seed,nss, nrs);
        if(args.verbose){
            if(coeffs == NULL){
                printf("You have to generate coefficients before printing them!");
            }
            printf("Coefficient: \n");
            printf("[");
            for (int i = 0; i < nss; ++i) {
                printf("[");
                for (int j = 0; j < nrs; ++j) {
                    printf("%d ", coeffs[i][j]);
                }
                printf("]\n");
            }
            printf("]\n");
        }


        /*
        // You may modify or delete the following lines. This is just an example of how to use tinymt32
        uint32_t seed = 42; // Replace with the seed from the instance file!

        tinymt32_t prng;
        memset(&prng, 0, sizeof(tinymt32_t));
        // Do not modify these values!
        prng.mat1 = 0x8f7011ee;
        prng.mat2 = 0xfc78ff1f;
        prng.tmat = 0x3793fdff;
        tinymt32_init(&prng, seed);

        // You can generate coefficients by calling this function
        // Do not forget that we use byte values, so we have to
        // cast the uint32_t returned value to only keep the last 8 bits.
        uint8_t coef = (uint8_t)tinymt32_generate_uint32(&prng);
        if (args.verbose)
        {
            printf("Coefficient: %u\n", coef);
        }
        */

        //Free variables
        //TODO: mettre au bon endrois dés que on utilise plus la variable
        free(file_data);
        free(coeffs);

        // Close this instance file
        fclose(input_file);
    }

    // Close the input directory and the output file
    err = closedir(args.input_dir);
    if (err < 0) {
        fprintf(stderr, "Error while closing the input directory containing the instance files\n");
    }
    if (args.output_stream != stdout) {
        fclose(args.output_stream);
    }
    return 0;

    file_read_error:
    err = closedir(args.input_dir);
    if (err < 0) {
        fprintf(stderr, "Error while closing the input directory containing the instance files\n");
    }
    if (args.output_stream != stdout) {
        fclose(args.output_stream);
    }
    exit(EXIT_FAILURE);
}

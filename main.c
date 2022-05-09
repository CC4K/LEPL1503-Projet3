//===========================================================//
// LEPL1503-Projet_3                                         //
// Created by Jacques, Romain, Cédric & Pierre on 15/03/22.  //
//===========================================================//

//================= Libraries and headers ===================//
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <limits.h>
#include <stdbool.h>
#include <fcntl.h>
#include <math.h>
#include <time.h>
#include "headers/system.h"
#include "headers/args.h"
#include "headers/file_data.h"
#include "headers/block_process.h"
#include "headers/portable_endian.h"

//======================= Functions =========================//
/**
 * Retrieves the data from the 'data' block as specified in the statement
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
    // Made by Jacques

    // Allocate memory for the returned data
    file_data_t* output = malloc(sizeof(file_data_t));
    if (output == NULL) return NULL;

    FILE* fileptr;
    uint32_t* buf;

    // Open the file
    fileptr = fopen(filename, "rb");

    // Create a buffer which contains the first 24 bytes
    buf = malloc(4 * sizeof(uint32_t)+1 * sizeof(uint64_t));
    int j = fread(buf,4 * sizeof(uint32_t)+1 * sizeof(uint64_t),1,fileptr);
    if(j == 0){
        exit(EXIT_FAILURE);
    }

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

/**
 * Help function that returns a string stored in binary in the given block in verbose mode
 * @param block: the said block
 * @param size: the size of the block
 * @return str: the block's string converted into binary
 */
char* block_to_string(uint8_t** block, uint32_t size) {
    // Made by Jacques

    // Allocate memory for the returned string
    char* str = malloc(sizeof(char) * ((size * word_size)+1));
    if(str == NULL) return NULL;

    // Record block elements in the string array
    int index = 0;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < word_size; j++) {
            // Stop at the first 0 we meet
            if (block[i][j] == 0) {
                // Add end of string and return
                str[index] = '\0';
                return str;
            }
            str[index] = (char) block[i][j];
            index++;
        }
    }

    // Add end of string and return
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
void write_block(FILE* output_file, uint8_t** block, uint32_t size, uint64_t word_size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < word_size; j++) {
            if ((output_file == stdout) || (output_file == stderr)) {
                if (!verbose) printf("%c", (char) block[i][j]);
            }
            else {
                fprintf(output_file, "%c", (char)(block[i][j]));
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
void write_last_block(FILE* output_file, uint8_t** block, uint8_t size, uint64_t word_size, uint32_t last_word_size) {
    for (int i = 0; i < size-1; i++) {
        for (int j = 0; j < word_size; j++) {
            if ((output_file == stdout) || (output_file == stderr)) {
                if (!verbose) printf("%c", (char) block[i][j]);
            }
            else {
                fprintf(output_file, "%c", (char) block[i][j]);
            }
        }
    }

    for (int i = 0; i < last_word_size; i++) {
        if ((output_file == stdout) || (output_file == stderr)) {
            if (!verbose) printf("%c", (char) block[size - 1][i]);
        }
        else {
            fprintf(output_file, "%c", (char) block[size - 1][i]);
        }
    }
}

/**
 * Help function to print n x m matrices in verbose mode
 * @param matrix: the matrix to print
 * @param n: number of lines
 * @param m: number of columns
 */
void printf_matrix(uint8_t** matrix, uint8_t n, uint8_t m) {
    // Made by Cédric

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

/**
 * Shows the arguments used during program execution
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

/**
 * Reads the arguments passed during execution to store them in args_t structure
 */
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

//================================================= MAIN FUNCTION ====================================================//
int main(int argc, char* argv[]) {
    // Variable to calculate time taken by the program
    clock_t time;
    time = clock();

    //========================================== Read user arguments =================================================//
    args_t args;
    int err = parse_args(&args, argc, argv);
    if (err == -1) exit(EXIT_FAILURE);
    else if (err == 1) exit(EXIT_SUCCESS);

    // Loop on each file in input_directory
    struct dirent *directory_entry;
    FILE* input_file;
    while ((directory_entry = readdir(args.input_dir))) {
        // Ignore parent and current directory
        if (!strcmp(directory_entry->d_name, ".")) continue;
        if (!strcmp(directory_entry->d_name, "..")) continue;

        // Add the directory path to the filename to open it
        char full_path[PATH_MAX];
        memset(full_path, 0, sizeof(char) * PATH_MAX);
        strcpy(full_path, args.input_dir_path);
        strcat(full_path, directory_entry->d_name);

        printf("\nfull : %s",full_path);

        // Setup verbose global variable
        verbose = args.verbose;

        //======================================= Open input file ====================================================//
        input_file = fopen(full_path, "r");
        if (input_file == NULL) {
            printf("========================================================================================================\n");
            fprintf(stderr, "Failed to open the input file %s: %s\n", full_path, strerror(errno));
            goto file_read_error;
        }
        if (verbose) {
            printf("========================================================================================================\n");
            fprintf(stderr, "Successfully opened the file %s\n", full_path);
        }

        //======================================= Get file infos =====================================================//
        file_data_t* file_data = get_file_info(full_path);
        if (file_data == NULL) {
            printf("Can't get file Infos");
            return -1;
        }
        if (verbose) {
            printf(">> seed : %d \n", *file_data->seed);
            printf(">> block_size : %d \n", *file_data->block_size);
            printf(">> word_size : %d \n", *file_data->word_size);
            printf(">> redundancy : %d \n", *file_data->redundancy);
            printf(">> message_size : %lu\n", *file_data->message_size);
        }

        // Setup main global variables
        word_size = *file_data->word_size;
        redundancy = *file_data->redundancy;
        uint64_t message_size = *file_data->message_size;
        uint32_t block_size = *file_data->block_size;
        uint32_t step = word_size * (block_size + redundancy);
        bool contains_uncomplete_block = false;
        bool has_output = (args.output_stream != stdout) && (args.output_stream != stderr);
        uint32_t readed = 0;

        //=============================== Generate matrix of coefficients ============================================//
        coeffs = gen_coefs(*file_data->seed, redundancy, block_size);
        // TODO: free file_data information
        free(file_data->seed);
        free(file_data->block_size);
        free(file_data->word_size);
        free(file_data->redundancy);
        free(file_data->message_size);
        free(file_data);

        if (verbose) {
            if (coeffs == NULL) {
                printf("You have to generate coefficients before printing them!\n");
            }
            else {
                printf(">> coefficients :\n");
                printf_matrix(coeffs, redundancy, block_size);
            }
        }

        //============================== Create buffer for input binary data =========================================//
        fseek(input_file, 0, SEEK_END);
        long filelen = ftell(input_file);
        rewind(input_file);
        uint8_t* buf = malloc(sizeof(char) * filelen);
        int i = fread(buf, filelen, 1, input_file);
        if(i == 0){
            exit(EXIT_FAILURE);
        }


        if (verbose) {
            printf(">> binary data : \n");
            for (int i = 24; i < filelen; i++) {
                printf("%d ", buf[i]);
            }
            printf("\n");
        }

        //================================== Detect number of full blocks ============================================//
        double num = (double) (filelen - 24);
        double den = (double) word_size * ((double) block_size + (double) redundancy);
        uint32_t nb_blocks = ceil(num/den);

        if (message_size != (nb_blocks * block_size * word_size)) {
            nb_blocks--;
            contains_uncomplete_block = true;
            if(verbose){
                printf("--------------------------------------------------------------------------------------------------------\n");
                printf("This file contains non-full blocks\n\n");
            }
        }
        if (!contains_uncomplete_block) {
            printf("--------------------------------------------------------------------------------------------------------\n");
            printf("This file doesn't contain non-full blocks\n\n");
        }

        //=========================== Write size of name, size and name to output ====================================//
        if (!has_output && !verbose) {
            fprintf(stdout, "%c", htobe32(strlen(directory_entry->d_name)));
            fprintf(stdout, "%c", htobe32(message_size));
            fprintf(stdout, "%s", directory_entry->d_name);
        }
        else if (has_output) {
            uint32_t bytes_len_directory_entry_name = htobe32(strlen(directory_entry->d_name));
            uint64_t bytes_message_size = htobe64(message_size);
            fwrite(&bytes_len_directory_entry_name, sizeof(uint32_t), 1, args.output_stream);
            fwrite(&bytes_message_size, sizeof(uint64_t), 1, args.output_stream);
            fprintf(args.output_stream, "%s", directory_entry->d_name);
        }



        // CONSUMER STARTS HERE


        //================================= Write full blocks to output ==============================================//
        for (int i = 0; i < nb_blocks; i++) {
            uint8_t* temps_buf = malloc(sizeof(uint8_t) * step);
            for (int j = 0; j < step; j++) {
                temps_buf[j] = buf[(i * step) + j + 24];
            }
            uint8_t** current_block = make_block(temps_buf, block_size);
            // TODO: free temps_buf (step)
            free(temps_buf);
            uint8_t** response = process_block(current_block,block_size);

            if (verbose) {
                printf(">> processed block %d :\n", i);
                printf_matrix(response, (block_size + redundancy), word_size);
                printf(">> to_string :\n");
                char* str = block_to_string(response, block_size);
                printf("%s", str);
                // TODO: free str ((block_size * word_size)+1)
                free(str);
                printf("\n\n--------------------------------------------------------------------------------------------------------\n");
            }

            write_block(args.output_stream,response,block_size, word_size);
            // TODO: free response LINES: (block_size + redundancy) | COLUMNS: word_size
            free_matrix(response, block_size + redundancy);

            readed += step;
        }

        //================================= Write last block to output ===============================================//
        uint32_t readed_symbols = block_size * word_size * nb_blocks;
        uint8_t* temps_buf = malloc(sizeof(uint8_t) * filelen-24-readed);
        for (int i = 0; i < filelen-24-readed; ++i) {
            temps_buf[i] = buf[24+readed + i];
        }
        // TODO: free buf (filelen)
        free(buf);

        uint32_t nb_remaining_symbols = ((filelen-24-readed) / word_size) - redundancy;
        if (contains_uncomplete_block) {
            uint8_t** last_block = make_block(temps_buf, nb_remaining_symbols);
            // TODO: free temps_buf (filelen-24-readed)
            free(temps_buf);
            uint8_t** decoded = process_block(last_block,nb_remaining_symbols);

            // TODO: free coefficients (last used in process_block) LINES: redundancy
            free_matrix(coeffs, redundancy);

            uint8_t padding = readed_symbols + nb_remaining_symbols * word_size - message_size;
            uint32_t true_length_last_symbol = word_size - padding;

            if (verbose) {
                printf(">> last processed block :\n");
                printf_matrix(decoded, (filelen-24-readed) / word_size, word_size);
                printf(">> to_string :\n");
                char* str = block_to_string(decoded, block_size);
                printf("%s", str);
                // TODO: free str (sizeof(char) * ((block_size * word_size)+1))
                free(str);
                printf("\n========================================================================================================\n\n");
            }

            write_last_block(args.output_stream,decoded,nb_remaining_symbols, word_size,true_length_last_symbol);
            // TODO: free decoded LINES: (nb_remaining_symbols + redundancy)
            free_matrix(decoded, nb_remaining_symbols + redundancy);
        }

        // Close the input file
        fclose(input_file);
    }

    // Calculate the time taken
    time = clock() - time;
    double time_taken = ((double) time)/CLOCKS_PER_SEC;
    printf("The program took %f seconds to execute\n", time_taken);
    bool has_output = (args.output_stream != stdout) && (args.output_stream != stderr);
    if (!verbose && !has_output) printf("\n");

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

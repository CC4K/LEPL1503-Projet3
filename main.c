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
#include "headers/tinymt32.h"

typedef struct
{
    DIR *input_dir;
    char input_dir_path[PATH_MAX];
    FILE *output_stream;
    uint8_t nb_threads;
    bool verbose;
} args_t;

typedef struct {
    uint32_t* seed;
    uint32_t* block_size;
    uint32_t* word_size;
    uint32_t* redundancy;
    uint64_t* message_size;
}file_data_t;

//Global variable for all file infos
file_data_t* file_data;

/*
    Récupère les informations du bloc `data`, comme spécifiées dans l'énoncé
        :param data: les 24 premieers bytes brutes du fichier
        :return pointeur sur un file_data_t qui comporte des pointeurs vers la seed, block_size,word_size et message_size
            seed = seed pour PRNG
            block_size = taille d'un block de symbole source
            word_size = la taille d'un mot
            redundancy = nb de coef de redondance qui protegent les blocs
            message_size = la taille du message initial a récupérer
*/
file_data_t* get_file_info(uint8_t* data){
    //TODO:vérifier
    //Fait par Jacques le 12/04/22
    file_data_t * output = malloc(sizeof(file_data_t));
    if(output == NULL){
        return NULL;
    }
    for (int i = 0; i < 24; ++i) {
        //Seed [0:4]
        if(0<=i && i<4){
            output->seed[i] = data[i];
        }
        //block_size [4:8]
        if(4<= i && i<8){
            output->block_size[i-4] = data[i];
        }
        //word_size [8:12]
        if(8<= i && i<12){
            output->word_size[i-8] = data[i];
        }
        //redundancy [12:16]
        if(12<= i && i<16){
            output->redundancy[i-12] = data[i];
        }
        //message_size [16:24]
        if(16<= i && i<24){
            output->message_size[i-16] = data[i];
        }
    }
    return output;
}

void usage(char *prog_name)
{
    fprintf(stderr, "USAGE:\n");
    fprintf(stderr, "    %s [OPTIONS] input_dir\n", prog_name);
    fprintf(stderr, "    input_dir: path to the directory containing the instance files with the encoded messages\n");
    fprintf(stderr, "    -f output_file: path to the output file containing all decoded messages\n");
    fprintf(stderr, "    -n n_threads (default: 4): set the number of computing threads that will be used to execute the RLC algorithm\n");
    fprintf(stderr, "    -v : enable debugging messages. If not set, no such messages will be displayed (except error messages on failure)\n");
}

int parse_args(args_t *args, int argc, char *argv[])
{
    memset(args, 0, sizeof(args_t));

    // Default values of the arguments
    args->nb_threads = 4;
    args->verbose = false;
    args->output_stream = stdout;
    int opt;
    while ((opt = getopt(argc, argv, "n:vf:")) != -1)
    {
        switch (opt)
        {
        case 'n':
            args->nb_threads = atoi(optarg);
            if (args->nb_threads == 0)
            {
                fprintf(stderr, "The number of computing threads must be a positive integer, got: %s\n", optarg);
                return -1;
            }
            break;
        case 'v':
            args->verbose = true;
            break;
        case 'f':
            args->output_stream = fopen(optarg, "w");
            if (args->output_stream == NULL)
            {
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

    if (optind == argc)
    {
        fprintf(stderr, "You must provide an input directory containing the instance files!\n");
        return -1;
    }

    // Source: https://stackoverflow.com/questions/11736060/how-to-read-all-files-in-a-folder-using-c
    if (NULL == (args->input_dir = opendir(argv[optind])))
    {
        fprintf(stderr, "Impossible to open the directory containing the input instance files %s: %s\n", argv[optind], strerror(errno));
        return -1;
    }
    // The following line is not very secure... Ask Prof. Legay and/or wait for LINGI2144 for more information :-)
    strcpy(args->input_dir_path, argv[optind++]);

    return 0;
}

/*
Construit le bloc sur base des données et de la taille d'un bloc

    :param data: les données du bloc en format binaire. Si le fichier d'input est bien formé, celui-ci est découpé
                 `size` symboles de taille `word_size` bytes, suivis de `redundancy` symboles de taille `word_size`
    :param size: le nombre de symboles sources dans un bloc
    :return block: le block construit, sous la forme d'une matrice (une ligne = un symbole)
*/
uint8_t** make_block(uint8_t* data,uint8_t size){
    //TODO: verifié
    //Fait par Jacques le 12/04/22
    uint8_t **block = malloc(sizeof(uint8_t));
    if(block == NULL){
        return NULL;
    }
    for (int i = 0; i < (*size + *(file_data->redundancy)) ; ++i) {
        for (int j = 0; j < *(file_data->word_size); ++j) {
            block[i][j] = data[i * (*(file_data->word_size)) + j];
        }
    }
    return block;
}

int main(int argc, char *argv[])
{
    args_t args;
    int err = parse_args(&args, argc, argv);
    if (err == -1)
    {
        exit(EXIT_FAILURE);
    }
    else if (err == 1)
    {
        exit(EXIT_SUCCESS);
    }

    // The following lines (and every code already present in this skeleton) can be removed, it is just an example to show you how to use the program arguments
    fprintf(stderr, "\tnumber of threads executing the RLC decoding algorithm in parallel: %" PRIu32 "\n", args.nb_threads);
    fprintf(stderr, "\tverbose mode: %s\n", args.verbose ? "enabled" : "disabled");

    // This is an example of how to open the instance files of the input directory. You may move/edit it during the project
    struct dirent *directory_entry;
    FILE *input_file;
    while ((directory_entry = readdir(args.input_dir)))
    {
        // Ignore parent and current directory
        if (!strcmp(directory_entry->d_name, "."))
        {
            continue;
        }
        if (!strcmp(directory_entry->d_name, ".."))
        {
            continue;
        }

        // Add the directory path to the filename to open it
        char full_path[PATH_MAX];
        memset(full_path, 0, sizeof(char) * PATH_MAX);
        strcpy(full_path, args.input_dir_path);
        strcat(full_path, "/");
        strcat(full_path, directory_entry->d_name);

        input_file = fopen(full_path, "r");
        if (input_file == NULL)
        {
            fprintf(stderr, "Failed to open the input file %s: %s\n", full_path, strerror(errno));
            goto file_read_error;
        }
        if (args.verbose)
        {
            // This is a simple example of how to use the verbose mode
            fprintf(stderr, "Successfully opened the file %s\n", full_path);
        }

        // TODO: parse the input binary file, decode the encoded message with RLC and write the output in the output stream following the statement
        
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

        // Close this instance file
        fclose(input_file);
    }

    // Close the input directory and the output file
    err = closedir(args.input_dir);
    if (err < 0)
    {
        fprintf(stderr, "Error while closing the input directory containing the instance files\n");
    }
    if (args.output_stream != stdout)
    {
        fclose(args.output_stream);
    }
    return 0;

file_read_error:
    err = closedir(args.input_dir);
    if (err < 0)
    {
        fprintf(stderr, "Error while closing the input directory containing the instance files\n");
    }
    if (args.output_stream != stdout)
    {
        fclose(args.output_stream);
    }
    exit(EXIT_FAILURE);
}
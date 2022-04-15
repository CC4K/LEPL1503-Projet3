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
#include "headers/system.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>


typedef struct {
    DIR* input_dir;
    char input_dir_path[PATH_MAX];
    FILE* output_stream;
    uint8_t nb_threads;
    bool verbose;
} args_t;


typedef struct {
    uint8_t** A;
    uint8_t** B;
} linear_system_t;


typedef struct {
    uint32_t* seed;
    uint32_t* block_size;
    uint32_t* word_size;
    uint32_t* redundancy;
    uint64_t* message_size;
} file_data_t;


// Global variable for all file infos
file_data_t* file_data;
uint8_t** coeffs = NULL;

//====================================================================================================================//


/**
 * Construit un système linéaire Ax=b à partir des blocs donnés en argument
 * @param unknown_indexes: tableau des index des symboles source d'un bloc. L'entrée i est `True` si le symbole i est perdu
 * @param nb_unk: le nombre d'inconnues dans le système - la taille du système
 * @param current_block: le bloc de symboles à résoudre
 * @param block_size: le nombre de symboles sources dans le bloc
 * @return A: la matrice de coefficients
 * @return B: le vecteur de termes indépendants. Chaque élément de B est de la même taille qu'un vecteur de données (paquet)
 */
linear_system_t* make_linear_system(uint8_t* unknown_indexes, uint32_t nb_unk, uint8_t** current_block, uint32_t block_size){
    // Crée par Cédric le 13/04/22
    // TODO: à verifier
    linear_system_t* output = malloc(sizeof(linear_system_t));
    if (output == NULL) return NULL;
    uint8_t** A = malloc(sizeof(int) * (nb_unk*nb_unk));
    if (A == NULL) return NULL;
    uint8_t** B = malloc(sizeof(int) * (nb_unk*block_size)); // block_size devrait être word_size dcp faudrait voir cmt on l'implémente
    if (B == NULL) return NULL;

    for (int i = 0; i < nb_unk; i++) {
        B[i] = current_block[block_size + i];
    }

    for (int i = 0; i < nb_unk; i++) {
        int temp = 0;
        for (int j = 0; j < block_size; j++) {
            if (unknown_indexes[j]){    // Si c'est une inconnue
                A[i][temp] = coeffs[i][j];
                temp += 1;
            }
            else{
                B[i] = gf_256_full_add_vector(B[i], gf_256_mul_vector(current_block[j], coeffs[i][j], block_size), block_size);
            }
        }
    }
    return output;
}


/**
 * Construit le bloc sur base des données et de la taille d'un bloc
 * @param data: les données du bloc en format binaire. Si le fichier d'input est bien formé, celui-ci est découpé
 *              `size` symboles de taille `word_size` bytes, suivis de `redundancy` symboles de taille `word_size`
 * @param size: le nombre de symboles sources dans un bloc
 * @return block: le block construit, sous la forme d'une matrice (une ligne = un symbole)
 */
uint8_t** make_block(uint8_t* data, uint8_t size){
    // Fait par Jacques le 12/04/22
    // TODO: à verifier
    uint8_t** block = malloc(sizeof(uint8_t)*(size + *(file_data->redundancy)));
    if(block == NULL) return NULL;
    for (int i = 0; i < (size + *(file_data->redundancy)); ++i) {
        block[i] = malloc(sizeof(uint8_t) * (*(file_data->word_size)));
        if(block[i] == NULL){return NULL;}
    }

    for (int i = 0; i < (size + (*(file_data->redundancy))) ; ++i) {
        for (int j = 0; j < (*(file_data->word_size)); ++j) {
            block[i][j] = data[i * (*(file_data->word_size)) + j];
        }
    }
    return block;
}


/**
 * Sur base d'un bloc, trouve les inconnues (i.e., symboles sources perdus) et construit le système linéaire
 * correspondant. Cette version simple considère qu'il y a toujours autant d'inconnues que de symboles de redondance,
 * c'est-à-dire qu'on va toujours construire un système avec autant d'équations que de symboles de redondances
 * @param block: le bloc en question
 * @param size: la taille du bloc
 * @return block: retourne le bloc résolu
 */
uint8_t** process_block(uint8_t** block, uint8_t size){
    // Crée par Cédric le 13/04/22
    // TODO: à faire après avoir terminé find_lost_words(), make_linear_systems() matrix_solve() et leurs sous-fct°
    return block;
}



/**
 *Fonction d'aide. Retourne un string stocké en binaire dans le bloc passé en argument
 *
 *:param block: le bloc en question
 *:param size: la taille du bloc
 *:return s: le string du bloc converti en binaire
 */
char* block_to_string(uint8_t *block, uint32_t size){
    //TODO: a Verifié
    //fait par jacques le 13/04/22
    char* str = malloc(sizeof(char)*(size* strlen(*block[0])));
    if(str == NULL){return NULL;}

    int index = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < strlen(*block[0]); ++j) {
            if(block[i][j] == 0){
                return str;
            }
            str[index] = (char) block[i][j];
            index++;
        }
    }
    return str;
}

/**
 * Récupère les informations du bloc 'data', comme spécifiées dans l'énoncé
 * @param filename: le Path Absulue du fichier
 * @return output: un structure qui contient des pointeurs vers la seed, le word_size, block_size, redundancy et message_size
 *                 - seed: le seed pour la génération de nombre aléatoire
 *                 - block_size: la taille d'un bloc - le nombre de symboles sources dans le bloc
 *                 - word_size: la taille d'un symbole 'complet' dans un bloc
 *                 - redundancy: le nombre de symboles de redondance dans le bloc
 *                 - message_size: la taille (en bytes) du fichier initial que nous souhaitons récupérer.
 *                                 Cette valeur ne prend en compte que les données du fichier, donc sans symbole de réparation
 *                                 ni les informations reprises ci-dessus
 */
file_data_t* get_file_info(char* filename){
    //fait pas jacques le 15/04/22
    //Fonctionnel et testé

    //alloue mémoire de la struc que on retourne
    file_data_t *output = malloc(sizeof(file_data_t));
    if(output == NULL){return NULL;}

    FILE* fileptr;
    uint32_t * buf;

    //ouvre le fichier
    fileptr = fopen(filename, "rb");

    //creer un buf qui contient les 24 premier Bytes;
    buf = malloc(4* sizeof(uint32_t) + 1 * sizeof(uint64_t));
    fread(buf,4* sizeof(uint32_t) + 1 * sizeof(uint64_t),1,fileptr);

    //alloue la memoire pour les pointeurs de la structure
    output->seed = malloc(sizeof(uint32_t));
    output->block_size = malloc(sizeof(uint32_t));
    output->word_size = malloc(sizeof(uint32_t));
    output->redundancy = malloc(sizeof(uint32_t));
    output->message_size = malloc(sizeof(uint64_t));

    //verifie si malloc a fonctionné
    if(output->seed == NULL){return NULL;}
    if(output->block_size == NULL){return NULL;}
    if(output->word_size == NULL){return NULL;}
    if(output->redundancy == NULL){return NULL;}
    if(output->message_size == NULL){return NULL;}

    //chaque valeur est associé
    *output->seed = be32toh((uint32_t)*buf);
    *output->block_size = be32toh((uint32_t)*(buf+1));
    *output->word_size = be32toh((uint32_t)*(buf+2));
    *output->redundancy = be32toh((uint32_t)*(buf+3));
    *output->message_size = be64toh((uint64_t)*(buf+2));



    //uncomment to see the result or call the verbose if you run main.c
    /*
        printf("seed: %d\n",*output->seed);
        printf("block_size: %d\n", *output->block_size);
        printf("word_size: %d\n", *output->word_size);
        printf("redundancy: %d\n", *output->redundancy);
        printf("message_size: %lu\n", *output->message_size);
    */

    //ferme le fichier
    fclose(fileptr);

    //free le buf
    free(buf);

    return output;
}


// fonctions ci-dessous initialement dans main.c

//Fonctionnel
int parse_args(args_t* args, int argc, char* argv[]){
    memset(args, 0, sizeof(args_t));

    // Default values of the arguments
    args->nb_threads = 4;
    args->verbose = false;
    args->output_stream = stdout;
    int opt;
    while ((opt = getopt(argc, argv, "n:vf:")) != -1){
        switch (opt){
            case 'n':
                args->nb_threads = atoi(optarg);
                if (args->nb_threads == 0){
                    fprintf(stderr, "The number of computing threads must be a positive integer, got: %s\n", optarg);
                    return -1;
                }
                break;
            case 'v':
                args->verbose = true;
                break;
            case 'f':
                args->output_stream = fopen(optarg, "w");
                if (args->output_stream == NULL){
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

    if (optind == argc){
        fprintf(stderr, "You must provide an input directory containing the instance files!\n");
        return -1;
    }

    // Source: https://stackoverflow.com/questions/11736060/how-to-read-all-files-in-a-folder-using-c
    if (NULL == (args->input_dir = opendir(argv[optind]))){
        fprintf(stderr, "Impossible to open the directory containing the input instance files %s: %s\n", argv[optind], strerror(errno));
        return -1;
    }
    // The following line is not very secure... Ask Prof. Legay and/or wait for LINGI2144 for more information :-)
    strcpy(args->input_dir_path, argv[optind++]);

    return 0;
}

void usage(char* prog_name){
    fprintf(stderr, "USAGE:\n");
    fprintf(stderr, "    %s [OPTIONS] input_dir\n", prog_name);
    fprintf(stderr, "    input_dir: path to the directory containing the instance files with the encoded messages\n");
    fprintf(stderr, "    -f output_file: path to the output file containing all decoded messages\n");
    fprintf(stderr, "    -n n_threads (default: 4): set the number of computing threads that will be used to execute the RLC algorithm\n");
    fprintf(stderr, "    -v : enable debugging messages. If not set, no such messages will be displayed (except error messages on failure)\n");
}

int main(int argc, char *argv[]){
    args_t args;
    int err = parse_args(&args, argc, argv);
    if (err == -1){
        exit(EXIT_FAILURE);
    }
    else if (err == 1){
        exit(EXIT_SUCCESS);
    }

    // The following lines (and every code already present in this skeleton) can be removed, it is just an example to show you how to use the program arguments
    fprintf(stderr, "\tnumber of threads executing the RLC decoding algorithm in parallel: %" PRIu32 "\n", args.nb_threads);
    fprintf(stderr, "\tverbose mode: %s\n", args.verbose ? "enabled" : "disabled");

    // This is an example of how to open the instance files of the input directory. You may move/edit it during the project
    struct dirent *directory_entry;
    FILE *input_file;
    while ((directory_entry = readdir(args.input_dir))){
        // Ignore parent and current directory
        if (!strcmp(directory_entry->d_name, ".")){
            continue;
        }
        if (!strcmp(directory_entry->d_name, "..")){
            continue;
        }

        // Add the directory path to the filename to open it
        char full_path[PATH_MAX];
        memset(full_path, 0, sizeof(char) * PATH_MAX);
        strcpy(full_path, args.input_dir_path);
        strcat(full_path, "/");
        strcat(full_path, directory_entry->d_name);

        input_file = fopen(full_path, "r");
        if (input_file == NULL){
            fprintf(stderr, "Failed to open the input file %s: %s\n", full_path, strerror(errno));
            goto file_read_error;
        }
        if (args.verbose){
            // This is a simple example of how to use the verbose mode
            fprintf(stderr, "Successfully opened the file %s\n", full_path);
        }

        // TODO: parse the input binary file, decode the encoded message with RLC and write the output in the output stream following the statement
        //retounr structure avec seed, word_size ...
        //TODO: vérifié si c'est bien de faire une variable global ou si on fait le malloc ici
        file_data = get_file_info(full_path);

        if(args.verbose){
            printf("Information sur le fichier :");
            printf("Seed : %d", *file_data->seed);
            printf("Block_size : %d", *file_data->block_size);
            printf("Word_size : %d", *file_data->word_size);
            printf("Seed : %d", *file_data->redundancy);
        }

        //

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
    if (err < 0){
        fprintf(stderr, "Error while closing the input directory containing the instance files\n");
    }
    if (args.output_stream != stdout){
        fclose(args.output_stream);
    }
    return 0;

file_read_error:
    err = closedir(args.input_dir);
    if (err < 0){
        fprintf(stderr, "Error while closing the input directory containing the instance files\n");
    }
    if (args.output_stream != stdout){
        fclose(args.output_stream);
    }
    exit(EXIT_FAILURE);
}

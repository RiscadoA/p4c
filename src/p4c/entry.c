#include <stdio.h>
#include <stdlib.h>

#include <p4c/lexer.h>

/*
    Usage:
        p4c [-o outfile] infile...
*/

int main(int argc, char** argv) {
    int in_file_count = 0;
    const char** in_files;
    const char* out_file;

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Argument parsing failed:\n-o passed without an argument\n");
                return 1;
            }

            out_file = argv[++i];
        }
        else {
            in_file_count += 1;
        }
    }

    in_files = (char**)malloc(in_file_count * sizeof(char*));

    for (int i = 1, j = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            ++i;
        }
        else {
            in_files[j++] = argv[i];
        }
    }

    if (in_file_count == 0) {
        fprintf(stderr, "Argument parsing failed:\nNo input file passed\n");
        return 1;
    }
    else if (in_file_count > 1) {
        fprintf(stderr, "Argument parsing failed:\nMore than one input file passed (not yet supported)\n");
        return 1;
    }

    char** in_source_code = (char**)malloc(in_file_count * sizeof(char*));

    for (int i = 0; i < in_file_count; ++i) {
        fprintf(stderr, "%s\n", in_files[i]);
        FILE* f = fopen(in_files[i], "rb");
        if (f == NULL) {
            fprintf(stderr, "Couldn't read file %s\n", in_files[i]);
            return 2;
        }

        fseek(f, 0, SEEK_END);
        size_t sz = ftell(f);
        fseek(f, 0, SEEK_SET);

        in_source_code[i] = (char*)malloc(sz + 1);
        fread(in_source_code[i], 1, sz, f);
        in_source_code[i][sz] = '\0';
    }

    free(in_files);

    // Run lexer
    p4c_token_t* tokens = (p4c_token_t*)malloc(65536 * sizeof(p4c_token_t));
    int token_count = p4c_run_lexer(in_source_code[0], tokens, 65536);

    for (int i = 0; i < token_count; ++i) {
        p4c_print_token(&tokens[i]);
        fprintf(stdout, "\n");
    }

    for (int i = 0; i < in_file_count; ++i) {
        free(in_source_code[i]);
    }
    free(in_source_code);

    fflush(stdout);
    return 0;
}
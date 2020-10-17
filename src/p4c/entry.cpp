#include <iostream>
#include <vector>
#include <string>

/*
    Usage:
        p4c [-o outfile] infile...
*/

int main(int argc, char** argv) {
    std::vector<std::string> in_files = {};
    std::string out_file = "out.as";

    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 >= argc) {
                std::cout << "Argument parsing failed:\n";
                std::cout << "-o passed without an argument\n";
                return 1;
            }

            out_file = argv[++i];
        }
        else {
            in_files.push_back(argv[i]);
        }
    }

    if (in_files.empty()) {
        std::cout << "Argument parsing failed:\n";
        std::cout << "No input files passed\n";
        return 1;
    }



    return 0;
}
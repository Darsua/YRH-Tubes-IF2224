#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "include/lexer.h"

using namespace std;

void print_usage(const char* program_name) {
    cout << "Usage: " << program_name << " <input_file>" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    const char* input_file = argv[1];
    FILE* file = read_file(input_file);
    if (file == NULL)
        return 1;
    vector<Token*> tokens = lex(file);
    return 0;
}
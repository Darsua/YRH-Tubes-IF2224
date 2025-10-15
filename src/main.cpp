#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <chrono>
#include "include/lexer.h"

using namespace std;

void print_usage(const char* program_name) {
    cout << "Usage: " << program_name << " [--time] <input_file>" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        print_usage(argv[0]);
        return 1;
    }
    
    bool show_time = false;
    const char* input_file;
    
    if (argc == 3 && strcmp(argv[1], "--time") == 0) {
        show_time = true;
        input_file = argv[2];
    } else if (argc == 2) {
        input_file = argv[1];
    } else {
        print_usage(argv[0]);
        return 1;
    }
    
    FILE* file = read_file(input_file);
    if (file == NULL)
        return 1;
    
    auto start_time = std::chrono::high_resolution_clock::now();
    vector<Token*> tokens = lex(file);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    if (show_time) {
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        cout << "\nLexical analysis completed in " << duration.count() << " microseconds ("
             << duration.count() / 1000.0 << " milliseconds)" << endl;
    }
    
    return 0;
}
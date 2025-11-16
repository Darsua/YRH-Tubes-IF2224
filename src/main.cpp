#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <chrono>
#include "include/lexer.h"
#include "include/parser.h"

using namespace std;

void print_usage(const char* program_name) {
    cout << "Usage: " << program_name << " [options] <input_file>" << endl;
    cout << "Options:" << endl;
    cout << "  -s, --switch      Use switch-based lexer instead of DFA" << endl;
    cout << "  -d, --dfa         Specify custom DFA rules file (default: rules/pascal_lexicon.dfa)" << endl;
    cout << "  -t, --time        Show timing information" << endl;
    cout << "  -l, --lexer       Lexer only mode (skip syntax analysis)" << endl;
    cout << "  --tree-only       Print only parse tree without other output" << endl;
    cout << "  -h, --help        Show this help message" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }
    
    bool show_time = false;
    bool use_switch = false;
    bool lexer_only = false;
    bool tree_only = false;
    const char* input_file = nullptr;
    const char* dfa_rules_file = nullptr;
    
    // PARSE ARGUMEN
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--switch") == 0) {
            use_switch = true;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dfa") == 0) {
            if (i + 1 >= argc) {
                cout << "Option " << argv[i] << " requires an argument" << endl;
                print_usage(argv[0]);
                return 1;
            }
            dfa_rules_file = argv[++i];
        } else if (strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--time") == 0) {
            show_time = true;
        } else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--lexer") == 0) {
            lexer_only = true;
        } else if (strcmp(argv[i], "--tree-only") == 0) {
            tree_only = true;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        } else if (argv[i][0] == '-') {
            cout << "Unknown option: " << argv[i] << endl;
            print_usage(argv[0]);
            return 1;
        } else {
            if (input_file != nullptr) {
                cout << "Multiple input files specified" << endl;
                print_usage(argv[0]);
                return 1;
            }
            input_file = argv[i];
        }
    }
    
    if (input_file == nullptr) {
        cout << "No input file specified" << endl;
        print_usage(argv[0]);
        return 1;
    }
    
    // PILIH MODE
    if (!tree_only) {
        cout << "Using " << (use_switch ? "switch-based" : "DFA-based") << " lexer" << endl;
        if (!use_switch) {
            cout << "DFA rules file: " << (dfa_rules_file ? dfa_rules_file : "rules/pascal_lexicon.dfa") << endl;
        }
        cout << "Processing file: " << input_file << endl;
        cout << "----------------------------------------" << endl;
    }
    
    FILE* file = read_file(input_file);
    if (file == NULL) {
        cout << "Failed to open file: " << input_file << endl;
        return 1;
    }
    
    LexerMode mode = use_switch ? SWITCH_MODE : DFA_MODE;
    Lexer lexer(mode, dfa_rules_file ? string(dfa_rules_file) : "rules/pascal_lexicon.dfa");
    
    auto start_time = std::chrono::high_resolution_clock::now();
    vector<Token*> tokens = lexer.lex(file);
    auto end_time = std::chrono::high_resolution_clock::now();
    
    if (!tree_only) {
        cout << "----------------------------------------" << endl;
        cout << "Tokenization completed successfully!" << endl;
        cout << "Total tokens: " << tokens.size() << endl;
        
        if (show_time) {
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            cout << "Lexical analysis completed in " << duration.count() << " microseconds ("
                 << duration.count() / 1000.0 << " milliseconds)" << endl;
        }
    }
    
    // PARSING (default behavior unless --lexer is specified)
    if (!lexer_only) {
        if (!tree_only) {
            cout << "----------------------------------------" << endl;
            cout << "Starting syntax analysis..." << endl;
            cout << "----------------------------------------" << endl;
        }
        
        try {
            auto parse_start = std::chrono::high_resolution_clock::now();
            Parser parser(tokens);
            auto parseTree = parser.parse();
            auto parse_end = std::chrono::high_resolution_clock::now();
            
            if (tree_only) {
                parseTree->print();
            } else {
                cout << "----------------------------------------" << endl;
                cout << "Parse Tree:" << endl;
                cout << "----------------------------------------" << endl;
                parseTree->print();
                
                cout << "----------------------------------------" << endl;
                cout << "Syntax analysis completed successfully!" << endl;
                
                if (show_time) {
                    auto parse_duration = std::chrono::duration_cast<std::chrono::microseconds>(parse_end - parse_start);
                    cout << "Syntax analysis completed in " << parse_duration.count() << " microseconds ("
                         << parse_duration.count() / 1000.0 << " milliseconds)" << endl;
                }
            }
        } catch (const exception& e) {
            cerr << "Parser error: " << e.what() << endl;
            for (Token* token : tokens) {
                delete token;
            }
            return 1;
        }
    }
    
    for (Token* token : tokens) {
        delete token;
    }
    
    return 0;
}
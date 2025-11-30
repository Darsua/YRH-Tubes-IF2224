#include <iostream>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <chrono>
#include "include/lexer.h"
#include "include/parser.h"
#include "include/ast_builder.h"
#include "include/semantic_analyzer.h"
#include "include/symbol_table.h"

using namespace std;

void print_usage(const char* program_name) {
    cout << "Usage: " << program_name << " [options] <input_file>" << endl;
    cout << "Options:" << endl;
    cout << "  -s, --switch      Use switch-based lexer instead of DFA" << endl;
    cout << "  -d, --dfa         Specify custom DFA rules file (default: rules/pascal_lexicon.dfa)"
         << endl;
    cout << "  -t, --time        Show timing information" << endl;
    cout << "  -l, --lexer       Show lexer (tokenization) results" << endl;
    cout << "  --show-lexer      Show lexer (tokenization) results in addition to semantic output"
         << endl;
    cout << "  --show-parser     Show parser (parse tree) results in addition to semantic output"
         << endl;
    cout << "  -h, --help        Show this help message" << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    bool show_time = false;
    bool use_switch = false;
    bool show_lexer = false;   // if true: print lexer output in addition to semantic results
    bool show_parser = false;  // if true: print parse tree in addition to semantic results
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
            show_lexer = true;
        } else if (strcmp(argv[i], "--show-lexer") == 0) {
            show_lexer = true;
        } else if (strcmp(argv[i], "--show-parser") == 0) {
            show_parser = true;
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
    cout << "Using " << (use_switch ? "switch-based" : "DFA-based") << " lexer" << endl;
    if (!use_switch) {
        cout << "DFA rules file: " << (dfa_rules_file ? dfa_rules_file : "rules/pascal_lexicon.dfa")
             << endl;
    }
    cout << "Processing file: " << input_file << endl;
    cout << "----------------------------------------" << endl;

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

    // Print lexer results only when requested
    if (show_lexer) {
        cout << "----------------------------------------" << endl;
        cout << "Tokenization completed successfully!" << endl;
        cout << "Total tokens: " << tokens.size() << endl;

        if (show_time) {
            auto duration =
                std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
            cout << "Lexical analysis completed in " << duration.count() << " microseconds ("
                 << duration.count() / 1000.0 << " milliseconds)" << endl;
        }
    }

    // If user asked to show lexer details, print tokens (but continue)
    if (show_lexer) {
        for (Token* token : tokens) {
            cout << token->toString() << endl;
        }
    }

    // PARSING
    if (show_parser) {
        cout << "----------------------------------------" << endl;
        cout << "Starting syntax analysis..." << endl;
        cout << "----------------------------------------" << endl;
    }

    try {
        auto parse_start = std::chrono::high_resolution_clock::now();
        Parser parser(tokens);
        auto parseTree = parser.parse();
        auto parse_end = std::chrono::high_resolution_clock::now();

        if (show_parser) {
            cout << "----------------------------------------" << endl;
            cout << "Parse Tree:" << endl;
            cout << "----------------------------------------" << endl;
            parseTree->print();

            cout << "----------------------------------------" << endl;
            cout << "Syntax analysis completed successfully!" << endl;

            if (show_time) {
                auto parse_duration =
                    std::chrono::duration_cast<std::chrono::microseconds>(parse_end - parse_start);
                cout << "Syntax analysis completed in " << parse_duration.count()
                     << " microseconds (" << parse_duration.count() / 1000.0 << " milliseconds)"
                     << endl;
            }
        }

        // AST BUILDING
        cout << "----------------------------------------" << endl;
        cout << "Building AST..." << endl;
        cout << "----------------------------------------" << endl;

        auto ast_start = std::chrono::high_resolution_clock::now();
        ASTBuilder astBuilder;
        auto ast = astBuilder.buildAST(parseTree);
        auto ast_end = std::chrono::high_resolution_clock::now();

        if (!ast) {
            cerr << "AST building failed" << endl;
            for (Token* token : tokens) {
                delete token;
            }
            return 1;
        }
        cout << "AST built successfully!" << endl;
        if (show_time) {
            auto ast_duration =
                std::chrono::duration_cast<std::chrono::microseconds>(ast_end - ast_start);
            cout << "AST building completed in " << ast_duration.count() << " microseconds ("
                 << ast_duration.count() / 1000.0 << " milliseconds)" << endl;
        }

        // SEMANTIC ANALYSIS
        cout << "----------------------------------------" << endl;
        cout << "Starting semantic analysis..." << endl;
        cout << "----------------------------------------" << endl;

        auto sem_start = std::chrono::high_resolution_clock::now();
        SymbolTable symbolTable;
        SemanticAnalyzer semanticAnalyzer(symbolTable);
        semanticAnalyzer.analyze(ast);
        auto sem_end = std::chrono::high_resolution_clock::now();

        auto errors = semanticAnalyzer.getErrors();

        if (!errors.empty()) {
            cout << "----------------------------------------" << endl;
            cout << "SEMANTIC ERRORS:" << endl;
            cout << "----------------------------------------" << endl;
            for (const auto& error : errors) {
                cout << "Error: " << error << endl;
            }
        } else {
            cout << "✓ No semantic errors found" << endl;
        }

        cout << "----------------------------------------" << endl;
        cout << "SYMBOL TABLE:" << endl;
        cout << "----------------------------------------" << endl;
        symbolTable.printAll();

        cout << "----------------------------------------" << endl;
        cout << "DECORATED AST:" << endl;
        cout << "----------------------------------------" << endl;
        ast->print(0);

        cout << "----------------------------------------" << endl;
        cout << "Semantic analysis completed!" << endl;

        if (show_time) {
            auto sem_duration =
                std::chrono::duration_cast<std::chrono::microseconds>(sem_end - sem_start);
            cout << "Semantic analysis completed in " << sem_duration.count() << " microseconds ("
                 << sem_duration.count() / 1000.0 << " milliseconds)" << endl;
        }
        cout << "----------------------------------------" << endl;

        // Return error code if semantic errors found
        for (Token* token : tokens) {
            delete token;
        }
        return errors.empty() ? 0 : 1;

    } catch (const exception& e) {
        cerr << "Parser error: " << e.what() << endl;
        for (Token* token : tokens) {
            delete token;
        }
        return 1;
    }

    for (Token* token : tokens) {
        delete token;
    }

    return 0;
}
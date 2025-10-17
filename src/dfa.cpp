#include "include/dfa.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <utility>

using namespace std;

void DFA::addTransition(const string& from_state, char input, const string& to_state) {
    transitions[make_pair(from_state, input)] = to_state;
}

void DFA::setStartState(const string& state) {
    start_state = state;
}

void DFA::addFinalState(const string& state) {
    if (find(final_states.begin(), final_states.end(), state) == final_states.end()) {
        final_states.push_back(state);
    }
}

string DFA::getNextState(const string& current_state, char input) const {
    auto it = transitions.find(make_pair(current_state, input));
    if (it != transitions.end()) {
        return it->second;
    }
    return "ERROR";
}

string DFA::getStartState() const {
    return start_state;
}

bool DFA::isFinalState(const string& state) const {
    return find(final_states.begin(), final_states.end(), state) != final_states.end();
}

bool DFA::loadDFAFromFile(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) return false;
    
    string line;
    
    while (getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Handle start state definition
        if (line.find("Start_state =") == 0) {
            size_t pos = line.find('=');
            if (pos != string::npos) {
                string start = line.substr(pos + 1);
                // Trim whitespace
                start.erase(0, start.find_first_not_of(" \t"));
                start.erase(start.find_last_not_of(" \t") + 1);
                setStartState(start);
            }
            continue;
        }
        
        // Handle final states definition
        if (line.find("Final_state =") == 0) {
            size_t pos = line.find('=');
            if (pos != string::npos) {
                string states_str = line.substr(pos + 1);
                // Trim whitespace
                states_str.erase(0, states_str.find_first_not_of(" \t"));
                states_str.erase(states_str.find_last_not_of(" \t") + 1);
                
                // Split by comma and add each final state
                istringstream ss(states_str);
                string state;
                while (getline(ss, state, ',')) {
                    // Trim whitespace from each state
                    state.erase(0, state.find_first_not_of(" \t"));
                    state.erase(state.find_last_not_of(" \t") + 1);
                    if (!state.empty()) {
                        addFinalState(state);
                    }
                }
            }
            continue;
        }
        
        // Handle transition rules
        istringstream iss(line);
        string from_state, input_str, to_state;
        
        if (iss >> from_state >> input_str >> to_state) {
            char input;
            if (input_str == "SPACE") {
                input = ' ';
            } else if (input_str.length() == 1) {
                input = input_str[0];
            } else {
                continue; // Skip invalid transitions
            }
            addTransition(from_state, input, to_state);
        }
    }
    
    file.close();
    
    return !transitions.empty();
}

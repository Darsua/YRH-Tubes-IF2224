#ifndef DFA_H
#define DFA_H

#include <map>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class DFA {
private:
    map<pair<string, char>, string> transitions;
    string start_state;
    vector<string> final_states;
    
public:    
    void addTransition(const string& from_state, char input, const string& to_state);
    void setStartState(const string& state);
    void addFinalState(const string& state);
    
    string getNextState(const string& current_state, char input) const;
    string getStartState() const;
    bool isFinalState(const string& state) const;
    
    bool loadDFAFromFile(const string& filename);
};

#endif // DFA_H
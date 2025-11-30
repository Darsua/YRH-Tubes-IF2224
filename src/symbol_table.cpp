#include "include/symbol_table.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

// Reserved words sesuai Lampiran C (29 reserved words)
const vector<string> SymbolTable::RESERVED_WORDS = {
    "and", "array", "begin", "case", "const", "div", "downto", "do", 
    "else", "end", "for", "function", "if", "mod", "not", "of", "or", 
    "procedure", "program", "record", "repeat", "string", "then", "to", 
    "type", "until", "var", "while", "packed"
};

// ==================== CONSTRUCTOR ====================

SymbolTable::SymbolTable() : level(0) {
    // Reserve space untuk efisiensi
    tab.reserve(1000);
    btab.reserve(100);
    atab.reserve(100);
    
    // Initialize reserved words (index 0-28)
    initializeReservedWords();
    
    // Create global block (btab[0])
    BtabEntry globalBlock;
    globalBlock.last = 28;  // Last reserved word
    globalBlock.lpar = 0;
    globalBlock.psze = 0;
    globalBlock.vsze = 0;
    btab.push_back(globalBlock);
    
    // Push global block ke display
    display.push(0);
    
    // Initialize standard identifiers (integer, boolean, etc)
    initializeStandardIdentifiers();
}

// ==================== SCOPE MANAGEMENT ====================

int SymbolTable::enterScope(int psize, int vsize) {
    level++;
    
    // Create new block
    BtabEntry newBlock;
    newBlock.last = tab.size() - 1;  // Will be updated as symbols are added
    newBlock.lpar = 0;  // Will be set if this is a procedure/function
    newBlock.psze = psize;
    newBlock.vsze = vsize;
    
    int blockIndex = btab.size();
    btab.push_back(newBlock);
    
    // Push to display stack
    display.push(blockIndex);
    
    return blockIndex;
}

void SymbolTable::exitScope() {
    if (level > 0) {
        display.pop();
        level--;
    }
}

int SymbolTable::getCurrentBlock() const {
    if (!display.empty()) {
        return display.top();
    }
    return 0;
}

// ==================== SYMBOL OPERATIONS (TAB) ====================

int SymbolTable::addSymbol(const TabEntry& entry) {
    int index = tab.size();
    tab.push_back(entry);
    
    // Update name index untuk quick lookup
    nameIndex[entry.identifier].push_back(index);
    
    // Update current block's last pointer
    int currentBlock = getCurrentBlock();
    if (currentBlock >= 0 && currentBlock < (int)btab.size()) {
        btab[currentBlock].last = index;
    }
    
    return index;
}

TabEntry* SymbolTable::lookupSymbol(const string& name, bool currentScopeOnly) {
    // Quick lookup using name index
    auto it = nameIndex.find(name);
    if (it == nameIndex.end()) {
        return nullptr;
    }
    
    // Search in reverse order (most recent first)
    const vector<int>& indices = it->second;
    
    if (currentScopeOnly) {
        // Search only in current block
        int currentBlock = getCurrentBlock();
        if (currentBlock < 0 || currentBlock >= (int)btab.size()) {
            return nullptr;
        }
        
        int lastInBlock = btab[currentBlock].last;
        
        // Traverse linked list of identifiers in this block
        for (int i = lastInBlock; i >= 0; i = tab[i].link) {
            if (tab[i].identifier == name) {
                return &tab[i];
            }
            // Stop if we've gone too far back
            if (i == 0 || (currentBlock > 0 && i < btab[currentBlock - 1].last)) {
                break;
            }
        }
    } else {
        // Search from current scope upwards to global
        // Return the most recent declaration that is accessible
        for (auto rit = indices.rbegin(); rit != indices.rend(); ++rit) {
            int idx = *rit;
            // Check if this symbol is in an accessible scope
            if (tab[idx].lev <= level) {
                return &tab[idx];
            }
        }
    }
    
    return nullptr;
}

TabEntry* SymbolTable::getSymbol(int index) {
    if (index >= 0 && index < (int)tab.size()) {
        return &tab[index];
    }
    return nullptr;
}

const TabEntry* SymbolTable::getSymbol(int index) const {
    if (index >= 0 && index < (int)tab.size()) {
        return &tab[index];
    }
    return nullptr;
}

void SymbolTable::updateSymbol(int index, const TabEntry& entry) {
    if (index >= 0 && index < (int)tab.size()) {
        tab[index] = entry;
    }
}

// ==================== BLOCK OPERATIONS (BTAB) ====================

int SymbolTable::addBlock(const BtabEntry& entry) {
    int index = btab.size();
    btab.push_back(entry);
    return index;
}

BtabEntry* SymbolTable::getBlock(int index) {
    if (index >= 0 && index < (int)btab.size()) {
        return &btab[index];
    }
    return nullptr;
}

const BtabEntry* SymbolTable::getBlock(int index) const {
    if (index >= 0 && index < (int)btab.size()) {
        return &btab[index];
    }
    return nullptr;
}

void SymbolTable::updateBlock(int index, const BtabEntry& entry) {
    if (index >= 0 && index < (int)btab.size()) {
        btab[index] = entry;
    }
}

// ==================== ARRAY OPERATIONS (ATAB) ====================

int SymbolTable::addArray(const AtabEntry& entry) {
    int index = atab.size();
    atab.push_back(entry);
    return index;
}

AtabEntry* SymbolTable::getArray(int index) {
    if (index >= 0 && index < (int)atab.size()) {
        return &atab[index];
    }
    return nullptr;
}

const AtabEntry* SymbolTable::getArray(int index) const {
    if (index >= 0 && index < (int)atab.size()) {
        return &atab[index];
    }
    return nullptr;
}

// ==================== INITIALIZATION ====================

void SymbolTable::initializeReservedWords() {
    // Add all 29 reserved words to tab (index 0-28)
    for (size_t i = 0; i < RESERVED_WORDS.size(); i++) {
        TabEntry entry;
        entry.identifier = RESERVED_WORDS[i];
        entry.link = (i > 0) ? i - 1 : 0;
        entry.obj = static_cast<int>(ObjectClass::RESERVED);
        entry.typ = static_cast<int>(TypeCode::NOTYP);
        entry.ref = 0;
        entry.nrm = 1;
        entry.lev = 0;
        entry.adr = 0;
        
        tab.push_back(entry);
        nameIndex[entry.identifier].push_back(i);
    }
}

void SymbolTable::initializeStandardIdentifiers() {
    // Add standard types: integer (index 29), real (30), boolean (31), char (32)
    
    // Integer type
    TabEntry intType;
    intType.identifier = "integer";
    intType.link = 28;  // Link to last reserved word
    intType.obj = static_cast<int>(ObjectClass::TYPE);
    intType.typ = static_cast<int>(TypeCode::INTS);
    intType.ref = 0;
    intType.nrm = 1;
    intType.lev = 0;
    intType.adr = 1;  // Size = 1
    int intIdx = addSymbol(intType);
    
    // Real type
    TabEntry realType;
    realType.identifier = "real";
    realType.link = intIdx;
    realType.obj = static_cast<int>(ObjectClass::TYPE);
    realType.typ = static_cast<int>(TypeCode::REALS);
    realType.ref = 0;
    realType.nrm = 1;
    realType.lev = 0;
    realType.adr = 1;  // Size = 1
    int realIdx = addSymbol(realType);
    
    // Boolean type
    TabEntry boolType;
    boolType.identifier = "boolean";
    boolType.link = realIdx;
    boolType.obj = static_cast<int>(ObjectClass::TYPE);
    boolType.typ = static_cast<int>(TypeCode::BOOLS);
    boolType.ref = 0;
    boolType.nrm = 1;
    boolType.lev = 0;
    boolType.adr = 1;  // Size = 1
    int boolIdx = addSymbol(boolType);
    
    // Char type
    TabEntry charType;
    charType.identifier = "char";
    charType.link = boolIdx;
    charType.obj = static_cast<int>(ObjectClass::TYPE);
    charType.typ = static_cast<int>(TypeCode::CHARS);
    charType.ref = 0;
    charType.nrm = 1;
    charType.lev = 0;
    charType.adr = 1;  // Size = 1
    int charIdx = addSymbol(charType);
    
    // Add standard procedures: read, readln, write, writeln
    
    // read procedure
    TabEntry readProc;
    readProc.identifier = "read";
    readProc.link = charIdx;
    readProc.obj = static_cast<int>(ObjectClass::PROCEDURE);
    readProc.typ = static_cast<int>(TypeCode::NOTYP);
    readProc.ref = 0;  // No block (built-in)
    readProc.nrm = 1;
    readProc.lev = 0;
    readProc.adr = 0;
    int readIdx = addSymbol(readProc);
    
    // readln procedure
    TabEntry readlnProc;
    readlnProc.identifier = "readln";
    readlnProc.link = readIdx;
    readlnProc.obj = static_cast<int>(ObjectClass::PROCEDURE);
    readlnProc.typ = static_cast<int>(TypeCode::NOTYP);
    readlnProc.ref = 0;
    readlnProc.nrm = 1;
    readlnProc.lev = 0;
    readlnProc.adr = 1;
    int readlnIdx = addSymbol(readlnProc);
    
    // write procedure
    TabEntry writeProc;
    writeProc.identifier = "write";
    writeProc.link = readlnIdx;
    writeProc.obj = static_cast<int>(ObjectClass::PROCEDURE);
    writeProc.typ = static_cast<int>(TypeCode::NOTYP);
    writeProc.ref = 0;
    writeProc.nrm = 1;
    writeProc.lev = 0;
    writeProc.adr = 2;
    int writeIdx = addSymbol(writeProc);
    
    // writeln procedure
    TabEntry writelnProc;
    writelnProc.identifier = "writeln";
    writelnProc.link = writeIdx;
    writelnProc.obj = static_cast<int>(ObjectClass::PROCEDURE);
    writelnProc.typ = static_cast<int>(TypeCode::NOTYP);
    writelnProc.ref = 0;
    writelnProc.nrm = 1;
    writelnProc.lev = 0;
    writelnProc.adr = 3;
    addSymbol(writelnProc);
    
    // Update global block's last
    btab[0].last = tab.size() - 1;
}

// ==================== DEBUGGING & PRINTING ====================

void SymbolTable::printTab() const {
    cout << "\n=== TAB (Identifier Table) ===" << endl;
    cout << setw(5) << "Idx" 
         << setw(15) << "Identifier" 
         << setw(6) << "Link"
         << setw(12) << "Obj"
         << setw(10) << "Type"
         << setw(6) << "Ref"
         << setw(5) << "Nrm"
         << setw(5) << "Lev"
         << setw(6) << "Adr" << endl;
    cout << string(80, '-') << endl;
    
    for (size_t i = 0; i < tab.size(); i++) {
        const TabEntry& entry = tab[i];
        cout << setw(5) << i
             << setw(15) << entry.identifier
             << setw(6) << entry.link
             << setw(12) << objectClassToString(entry.obj)
             << setw(10) << typeCodeToString(entry.typ)
             << setw(6) << entry.ref
             << setw(5) << entry.nrm
             << setw(5) << entry.lev
             << setw(6) << entry.adr << endl;
    }
}

void SymbolTable::printBtab() const {
    cout << "\n=== BTAB (Block Table) ===" << endl;
    cout << setw(5) << "Idx"
         << setw(8) << "Last"
         << setw(8) << "Lpar"
         << setw(8) << "Psze"
         << setw(8) << "Vsze" << endl;
    cout << string(40, '-') << endl;
    
    for (size_t i = 0; i < btab.size(); i++) {
        const BtabEntry& entry = btab[i];
        cout << setw(5) << i
             << setw(8) << entry.last
             << setw(8) << entry.lpar
             << setw(8) << entry.psze
             << setw(8) << entry.vsze << endl;
    }
}

void SymbolTable::printAtab() const {
    cout << "\n=== ATAB (Array Table) ===" << endl;
    cout << setw(5) << "Idx"
         << setw(8) << "Xtyp"
         << setw(8) << "Etyp"
         << setw(8) << "Eref"
         << setw(8) << "Low"
         << setw(8) << "High"
         << setw(8) << "Elsz"
         << setw(8) << "Size" << endl;
    cout << string(65, '-') << endl;
    
    for (size_t i = 0; i < atab.size(); i++) {
        const AtabEntry& entry = atab[i];
        cout << setw(5) << i
             << setw(8) << entry.xtyp
             << setw(8) << entry.etyp
             << setw(8) << entry.eref
             << setw(8) << entry.low
             << setw(8) << entry.high
             << setw(8) << entry.elsz
             << setw(8) << entry.size << endl;
    }
}

void SymbolTable::printAll() const {
    printTab();
    printBtab();
    if (!atab.empty()) {
        printAtab();
    }
}

void SymbolTable::printBlock(int blockIndex) const {
    if (blockIndex < 0 || blockIndex >= (int)btab.size()) {
        cout << "Invalid block index: " << blockIndex << endl;
        return;
    }
    
    cout << "\n=== Block " << blockIndex << " Symbols ===" << endl;
    const BtabEntry& block = btab[blockIndex];
    
    cout << "Block info: last=" << block.last 
         << ", lpar=" << block.lpar
         << ", psze=" << block.psze
         << ", vsze=" << block.vsze << endl;
    
    cout << "\nSymbols in this block:" << endl;
    cout << setw(5) << "Idx" 
         << setw(15) << "Identifier" 
         << setw(12) << "Obj"
         << setw(10) << "Type"
         << setw(5) << "Lev" << endl;
    cout << string(50, '-') << endl;
    
    // Find start of block
    int start = (blockIndex > 0) ? btab[blockIndex - 1].last + 1 : 0;
    
    // Print symbols in this block
    for (int i = start; i <= block.last && i < (int)tab.size(); i++) {
        const TabEntry& entry = tab[i];
        cout << setw(5) << i
             << setw(15) << entry.identifier
             << setw(12) << objectClassToString(entry.obj)
             << setw(10) << typeCodeToString(entry.typ)
             << setw(5) << entry.lev << endl;
    }
}

void SymbolTable::printDisplay() const {
    cout << "\n=== Display Stack ===" << endl;
    cout << "Current level: " << level << endl;
    cout << "Stack (top to bottom): ";
    
    // Need to copy stack to print it
    stack<int> temp = display;
    vector<int> blocks;
    while (!temp.empty()) {
        blocks.push_back(temp.top());
        temp.pop();
    }
    
    reverse(blocks.begin(), blocks.end());
    for (size_t i = 0; i < blocks.size(); i++) {
        cout << blocks[i];
        if (i < blocks.size() - 1) cout << " -> ";
    }
    cout << endl;
}

// ==================== HELPER FUNCTIONS ====================

string objectClassToString(int obj) {
    switch (obj) {
        case 0: return "CONST";
        case 1: return "VAR";
        case 2: return "TYPE";
        case 3: return "PROC";
        case 4: return "FUNC";
        case 5: return "RESERVED";
        default: return "UNKNOWN";
    }
}

string typeCodeToString(int typ) {
    switch (typ) {
        case 0: return "NOTYP";
        case 1: return "INT";
        case 2: return "REAL";
        case 3: return "BOOL";
        case 4: return "CHAR";
        case 5: return "ARRAY";
        case 6: return "RECORD";
        default: return "UNKNOWN";
    }
}

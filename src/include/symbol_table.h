#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iostream>

using namespace std;

enum class ObjectClass {
    CONSTANT = 0,
    VARIABLE = 1, 
    TYPE = 2,  
    PROCEDURE = 3,
    FUNCTION = 4,
    RESERVED = 5,
    PROGRAM = 6
};

enum class TypeCode {
    NOTYP = 0,      
    INTS = 1,        
    REALS = 2,      
    BOOLS = 3,      
    CHARS = 4,      
    ARRAYS = 5,
    RECORDS = 6
};

struct TabEntry {
    string identifier;     // Nama identifier
    int link;              // Link ke identifier sebelumnya di block yang sama
    int obj;               // Object class (constant, variable, type, procedure, function)
    int typ;               // Type code
    int ref;               // Reference ke btab/atab (untuk composite types)
    int nrm;               // Normal: 1 = normal variable, 0 = var parameter (by reference)
    int lev;               // Lexical level (0 = global, 1+ = nested)
    int adr;               // Address/value/offset
    
    TabEntry() : identifier(""), link(0), obj(0), typ(0), ref(0), nrm(1), lev(0), adr(0) {}
};

struct BtabEntry {
    int last;   // Index tab terakhir di block ini
    int lpar;   // Index parameter terakhir (0 jika tidak ada parameter)
    int psze;   // Parameter size (total ukuran parameter)
    int vsze;   // Variable size (total ukuran variabel lokal)
    
    BtabEntry() : last(0), lpar(0), psze(0), vsze(0) {}
    BtabEntry(int l, int lp, int ps, int vs) : last(l), lpar(lp), psze(ps), vsze(vs) {}
};

struct AtabEntry {
    int xtyp;   // Index type (tipe untuk indeks array)
    int etyp;   // Element type (tipe elemen array)
    int eref;   // Element reference (untuk nested array/record)
    int low;    // Lower bound
    int high;   // Upper bound
    int elsz;   // Element size
    int size;   // Total array size
    
    AtabEntry() : xtyp(0), etyp(0), eref(0), low(0), high(0), elsz(0), size(0) {}
    AtabEntry(int x, int e, int er, int l, int h, int es, int s) 
        : xtyp(x), etyp(e), eref(er), low(l), high(h), elsz(es), size(s) {}
};

class SymbolTable {
private:
    vector<TabEntry> tab;       
    vector<BtabEntry> btab;     
    vector<AtabEntry> atab;    
    
    stack<int> display;         
    int level;
    
    // Helper untuk quick lookup by name
    unordered_map<string, vector<int>> nameIndex;
    
    // Reserved words
    static const vector<string> RESERVED_WORDS;

public:
    SymbolTable();

    // ==================== SCOPE MANAGEMENT ====================

    int enterScope(int psize = 0, int vsize = 0);
    void exitScope();
    int getCurrentBlock() const;
    int getCurrentLevel() const;
    
    // ==================== SYMBOL OPERATIONS (TAB) ====================
    
    /**
     * Add new symbol to identifier table
     * @param entry Symbol entry to add
     * @return Index in tab where symbol was added
     */
    int addSymbol(const TabEntry& entry);
    
    /**
     * Lookup symbol by name
     * Searches from current scope upwards to global scope
     * @param name Identifier name
     * @param currentScopeOnly If true, only search in current block
     * @return Pointer to TabEntry if found, nullptr otherwise
     */
    TabEntry* lookupSymbol(const string& name, bool currentScopeOnly = false);
    
    /**
     * Get symbol by index
     * @param index Index in tab
     * @return Pointer to TabEntry
     */
    TabEntry* getSymbol(int index);
    
    /**
     * Get symbol by index (const version)
     */
    const TabEntry* getSymbol(int index) const;
    
    /**
     * Update symbol at given index
     * @param index Index in tab
     * @param entry New entry data
     */
    void updateSymbol(int index, const TabEntry& entry);
    
    /**
     * Get tab size
     */
    int getTabSize() const;
    
    // ==================== BLOCK OPERATIONS (BTAB) ====================
    
    /**
     * Add new block
     * @param entry Block entry
     * @return Index in btab
     */
    int addBlock(const BtabEntry& entry);
    
    /**
     * Get block by index
     * @param index Index in btab
     * @return Pointer to BtabEntry
     */
    BtabEntry* getBlock(int index);
    
    /**
     * Get block by index (const version)
     */
    const BtabEntry* getBlock(int index) const;
    
    /**
     * Update block at given index
     * @param index Index in btab
     * @param entry New entry data
     */
    void updateBlock(int index, const BtabEntry& entry);
    
    /**
     * Get btab size
     */
    int getBtabSize() const;
    
    // ==================== ARRAY OPERATIONS (ATAB) ====================
    
    /**
     * Add new array type
     * @param entry Array entry
     * @return Index in atab
     */
    int addArray(const AtabEntry& entry);
    
    /**
     * Get array by index
     * @param index Index in atab
     * @return Pointer to AtabEntry
     */
    AtabEntry* getArray(int index);
    
    /**
     * Get array by index (const version)
     */
    const AtabEntry* getArray(int index) const;
    
    /**
     * Get atab size
     */
    int getAtabSize() const;
    
    // ==================== INITIALIZATION ====================
    
    /**
     * Initialize reserved words
     */
    void initializeReservedWords();
    
    /**
     * Initialize standard types and functions
     */
    void initializeStandardIdentifiers();
    
    // ==================== DEBUGGING & PRINTING ====================
    
    /**
     * Print entire symbol table (tab)
     */
    void printTab() const;
    
    /**
     * Print block table (btab)
     */
    void printBtab() const;
    
    /**
     * Print array table (atab)
     */
    void printAtab() const;
    
    /**
     * Print all tables
     */
    void printAll() const;
    
    /**
     * Print symbols in specific block
     * @param blockIndex Index of block in btab
     */
    void printBlock(int blockIndex) const;
    
    /**
     * Print display stack (for debugging)
     */
    void printDisplay() const;
};

// Helper functions untuk convert enum ke string
string objectClassToString(int obj);
string typeCodeToString(int typ);

#endif

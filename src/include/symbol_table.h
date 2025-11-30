#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <vector>
#include <stack>
#include <unordered_map>
#include <iostream>

using namespace std;

enum class ObjectClass {
    CONSTANT = 0,    // Konstanta
    VARIABLE = 1,    // Variabel
    TYPE = 2,        // Type definition
    PROCEDURE = 3,   // Prosedur
    FUNCTION = 4,    // Fungsi
    RESERVED = 5     // Reserved word
};

// Type codes untuk tab.typ
enum class TypeCode {
    NOTYP = 0,      // No type
    INTS = 1,       // Integer
    REALS = 2,      // Real
    BOOLS = 3,      // Boolean
    CHARS = 4,      // Char
    ARRAYS = 5,     // Array
    RECORDS = 6     // Record
};

// Tab entry - identifier table
// Sesuai spesifikasi: identifiers, link, obj, type, ref, nrm, lev, adr
struct TabEntry {
    string identifier;      // Nama identifier
    int link;              // Link ke identifier sebelumnya di block yang sama
    int obj;               // Object class (constant, variable, type, procedure, function)
    int typ;               // Type code
    int ref;               // Reference ke btab/atab (untuk composite types)
    int nrm;               // Normal: 1 = normal variable, 0 = var parameter (by reference)
    int lev;               // Lexical level (0 = global, 1+ = nested)
    int adr;               // Address/value/offset
    
    TabEntry() : identifier(""), link(0), obj(0), typ(0), ref(0), nrm(1), lev(0), adr(0) {}
};

// Btab entry - block table
// Sesuai spesifikasi: last, lpar, psze, vsze
struct BtabEntry {
    int last;   // Index tab terakhir di block ini
    int lpar;   // Index parameter terakhir (0 jika tidak ada parameter)
    int psze;   // Parameter size (total ukuran parameter)
    int vsze;   // Variable size (total ukuran variabel lokal)
    
    BtabEntry() : last(0), lpar(0), psze(0), vsze(0) {}
    BtabEntry(int l, int lp, int ps, int vs) : last(l), lpar(lp), psze(ps), vsze(vs) {}
};

// Atab entry - array table
// Sesuai spesifikasi: xtyp, etyp, eref, low, high, elsz, size
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

/**
 * Symbol Table Class
 * Manages three tables: tab (identifiers), btab (blocks), atab (arrays)
 * Uses stack-based scope management with display register
 */
class SymbolTable {
private:
    vector<TabEntry> tab;       // Identifier table
    vector<BtabEntry> btab;     // Block table
    vector<AtabEntry> atab;     // Array table
    
    stack<int> display;         // Display register - stack of block indices
    int level;                  // Current lexical level
    
    // Helper untuk quick lookup by name (optional optimization)
    unordered_map<string, vector<int>> nameIndex;
    
    // Reserved words yang sudah ter-install di tab[0..28]
    static const vector<string> RESERVED_WORDS;

public:
    /**
     * Constructor - Initialize symbol table
     * Reserved words akan diisi di index 0-28
     * Global block (btab[0]) akan dibuat
     */
    SymbolTable();
    
    // ==================== SCOPE MANAGEMENT ====================
    
    /**
     * Enter new scope (create new block)
     * @param psize Parameter size
     * @param vsize Variable size
     * @return Index of new block in btab
     */
    int enterScope(int psize = 0, int vsize = 0);
    
    /**
     * Exit current scope
     */
    void exitScope();
    
    /**
     * Get current block index
     */
    int getCurrentBlock() const;
    
    /**
     * Get current lexical level
     */
    int getCurrentLevel() const { return level; }
    
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
    int getTabSize() const { return tab.size(); }
    
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
    int getBtabSize() const { return btab.size(); }
    
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
    int getAtabSize() const { return atab.size(); }
    
    // ==================== INITIALIZATION ====================
    
    /**
     * Initialize reserved words (index 0-28 in tab)
     * Reserved words dari Lampiran C:
     * AND, ARRAY, BEGIN, CASE, CONST, DIV, DOWNTO, DO, ELSE, END,
     * FOR, FUNCTION, IF, MOD, NOT, OF, OR, PROCEDURE, PROGRAM, RECORD,
     * REPEAT, STRING, THEN, TO, TYPE, UNTIL, VAR, WHILE, PACKED
     */
    void initializeReservedWords();
    
    /**
     * Initialize standard types and functions
     * Adds: integer, real, boolean, char types
     * Adds: read, write, readln, writeln functions
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

#endif // SYMBOL_TABLE_H

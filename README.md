# <p align="center" width=300px>NieR: FiniteAutomata</p> <p align="center" width=100px>A Pascal-S Compiler</p>

<p align="center"><img width="400" alt="image" src="https://github.com/user-attachments/assets/6ef29817-36db-4306-ac96-e9a1e56bdad8" /></p>
<p align="center" width=300px><i>Our YoRHa programmer, 2B. (by KornArt)</i></p>

# Identitas Kelompok
- Muhammad Ra'if Alkautsar (13523011)
- Fajar Kurniawan (13523027)
- Darrel Adinarya Sunanda (13523061)
- Reza Ahmad Syarif (13523119)

# Deskripsi Program
Pascal-S Compiler adalah program untuk menganalisis dan memproses source code Pascal-S melalui dua tahap utama:

1. **Lexical Analysis (Analisis Leksikal)**: Mengubah kode sumber dari kumpulan karakter mentah menjadi unit-unit bermakna yang disebut token menggunakan Deterministic Finite Automata (DFA).

2. **Syntax Analysis (Analisis Sintaksis)**: Membangun parse tree dari token-token yang dihasilkan oleh lexer untuk memverifikasi struktur sintaksis program sesuai dengan grammar Pascal-S.

Program ini mengimplementasikan dua pendekatan untuk lexical analysis:
- **DFA-based Lexer**: Menggunakan aturan DFA yang didefinisikan dalam file eksternal untuk fleksibilitas
- **Switch-based Lexer**: Menggunakan struktur switch-case untuk performa yang lebih baik

Untuk syntax analysis, program menggunakan **Recursive Descent Parser** yang memproses token secara top-down sesuai dengan grammar Pascal-S yang telah ditentukan.

# Requirements
- GCC
- Make

# How-To

## Cara Instalasi
Untuk mengompilasi program menjadi binary, jalankan instruksi:

```bash
make all
```

Atau untuk rebuild lengkap:

```bash
make clean
make
```

Binary akan tersimpan di direktori `bin/compiler`.

## Cara Menjalankan Program

### Mode Default (Analisis Leksikal + Sintaksis)
Secara default, program akan menjalankan analisis leksikal dan sintaksis:

```bash
./bin/compiler <file.pas>
```

Program akan menampilkan token-token yang dihasilkan dan parse tree dari source code.

### Mode Lexer Only
Untuk menjalankan **hanya** analisis leksikal tanpa parsing:

```bash
./bin/compiler -l <file.pas>
# atau
./bin/compiler --lexer <file.pas>
```

Mode ini berguna untuk debugging lexer atau melihat hasil tokenisasi saja.

### Opsi Tambahan

**Menggunakan Switch-based Lexer:**
```bash
./bin/compiler -s <file.pas>
# atau
./bin/compiler --switch <file.pas>
```

**Menampilkan Informasi Waktu Eksekusi:**
```bash
./bin/compiler -t <file.pas>
# atau
./bin/compiler --time <file.pas>
```

**Menggunakan File DFA Rules Kustom:**
```bash
./bin/compiler -d <rules_file.dfa> <file.pas>
# atau
./bin/compiler --dfa <rules_file.dfa> <file.pas>
```

**Menampilkan Parse Tree Saja (tanpa output lain):**
```bash
./bin/compiler --tree-only <file.pas>
```

Opsi ini berguna untuk pipeline atau ketika hanya ingin melihat struktur parse tree tanpa informasi tambahan.

**Kombinasi Opsi:**
```bash
./bin/compiler -t <file.pas>                       # Parse dengan timing
./bin/compiler -s <file.pas>                       # Switch-based lexer + parser
./bin/compiler -l -t <file.pas>                    # Lexer only dengan timing
./bin/compiler --tree-only <file.pas>              # Hanya parse tree
./bin/compiler -s -l <file.pas>                    # Switch-based lexer tanpa parser
```

### Menampilkan Help
```bash
./bin/compiler -h
# atau
./bin/compiler --help
```

## Contoh Penggunaan

```bash
# Default: Analisis leksikal + sintaksis dengan DFA
./bin/compiler test/milestone-1/base.pas

# Lexer only dengan timing
./bin/compiler -l -t test/milestone-1/hard.pas

# Parsing dengan switch-based lexer
./bin/compiler -s test/milestone-2/complete.pas

# Hanya menampilkan parse tree
./bin/compiler --tree-only test/milestone-2/dasar.pas
```

Ganti `<file.pas>` dengan path file sumber Pascal-S yang ingin dianalisis.

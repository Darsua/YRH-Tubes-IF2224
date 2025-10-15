# <p align="center" width=300px>NieR: FiniteAutomata: A Pascal-S Lexical Analyzer</p>

<p align="center"><img width="400" alt="image" src="https://github.com/user-attachments/assets/6ef29817-36db-4306-ac96-e9a1e56bdad8" /></p>
<p align="center" width=300px><i>Our YoRHa programmer, 2B. (by KornArt)</p>

# Identitas Kelompok
- Muhammad Ra'if Alkautsar (13523011)
- Fajar Kurniawan (13523027)
- Darrel Adinarya Sunanda (13523061)
- Reza Ahmad Syarif (13523119)

# Deskripsi Program
Lexical analyzer pada dasarnya adalah program untuk melakukan analisis leksikal dengan menggunakan Deterministic Finite Automata (DFA) untuk mengenali pola karakter dalam source. Pada repositori ini, Pascal-S Lexical Analyzer adalah program untuk mengubah source code Pascal-S dari kumpulan karakter mentah menjadi unit-unit bermakna yang disebut token. 

Pada program ini, aturan DFA didefinisikan pada sebuah file. Program akan membaca file aturan dan memproses kode sumber Pascal-S menjadi unit-unit token berdasarkan aturan tersebut.

# Requirements
- GCC
- Make

# How-To

## Cara Instalasi
Untuk mengompilasi program menjadi binary, jalankan instruksi:

```
make all
```

## Cara Menjalankan Program
Untuk menjalankan program terhadap suatu file Pascal-S, jalankan instruksi:

```
./bin/compiler [program_name].pas
```

Ganti [program_name].pas dengan nama file sumber Pascal-S yang ingin dianalisis.

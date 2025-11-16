program TestLexerIndonesiaLengkap;

{ ============================================
  Test Suite Lexical Analyzer - Milestone 2
  Semua Keyword Bahasa Indonesia
  ============================================ }

konstanta
    MAX_SIZE = 100;
    MIN_VALUE = 0;
    PI = 3.14159;
    NAMA_APP = 'Aplikasi Test';

tipe
    NilaiUjian = larik [1..10] dari integer;

variabel
    i, j, k: integer;
    hasil, total: real;
    selesai_flag: boolean;
    huruf: char;
    angka: NilaiUjian;
    mahasiswa: DataMahasiswa;
    status: StatusSiswa;

fungsi tambah(a, b: integer): integer;
mulai
    tambah := a + b;
selesai;

fungsi rata_rata(nilai: NilaiUjian; n: integer): real;
variabel
    idx: integer;
    jumlah: real;
mulai
    jumlah := 0;
    untuk idx := 1 ke n lakukan
        jumlah := jumlah + nilai[idx];
    rata_rata := jumlah bagi n;
selesai;

prosedur cetak_info(npm: integer; nama: char);
mulai
    { Cetak informasi mahasiswa }
    (* Format: NPM - Nama *)
selesai;

prosedur inisialisasi_array;
variabel
    idx: integer;
mulai
    untuk idx := 1 ke 10 lakukan
        angka[idx] := 0;
selesai;

mulai
    { ===== TEST CONDITIONAL (JIKA-MAKA-SELAIN-ITU) ===== }
    jika i > j maka
    mulai
        i := i + 1;
        j := j - 1;
    selesai
    selain_itu
    mulai
        i := i - 1;
        j := j + 1;
    selesai;
    
selesai.
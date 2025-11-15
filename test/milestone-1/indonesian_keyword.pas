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
    StatusSiswa = (aktif, lulus, cuti, dropout);
    
    DataMahasiswa = rekaman
        npm: integer;
        nama: char;
        ipk: real;
        aktif: boolean;
    selesai;
    
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
    { ===== TEST OPERATOR ARITMATIKA ===== }
    i := 100 bagi 3;
    j := 100 mod 3;
    k := i + j - 5;
    hasil := k * 2;
    total := hasil / 4;
    
    { ===== TEST OPERATOR LOGIKA ===== }
    selesai_flag := (i < j) dan (j < k);
    selesai_flag := (i > j) atau (k > j);
    selesai_flag := tidak selesai_flag;
    
    { ===== TEST CONDITIONAL (JIKA-MAKA-SELAIN-ITU) ===== }
    jika i > j maka
    mulai
        i := i + 1;
        j := j - 1;
    selesai
    selain-itu
    mulai
        i := i - 1;
        j := j + 1;
    selesai;
    
    { ===== TEST WHILE LOOP (SELAMA-LAKUKAN) ===== }
    i := 0;
    total := 0;
    selama i < 10 lakukan
    mulai
        i := i + 1;
        total := total + i;
    selesai;
    
    { ===== TEST FOR ASCENDING (UNTUK-KE-LAKUKAN) ===== }
    hasil := 0;
    untuk i := 1 ke 10 lakukan
    mulai
        hasil := hasil + i;
        angka[i] := i * 2;
    selesai;
    
    { ===== TEST FOR DESCENDING (UNTUK-TURUN-KE-LAKUKAN) ===== }
    hasil := 0;
    untuk i := 10 turun-ke 1 lakukan
    mulai
        hasil := hasil + i;
    selesai;
    
    { ===== TEST REPEAT-UNTIL (ULANGI-SAMAPAI) ===== }
    i := 0;
    ulangi
        i := i + 1;
        total := total + i;
    samapai i >= 10;
    
    { ===== TEST CASE STATEMENT (KASUS) ===== }
    k := 2;
    kasus k dari
        1: i := 10;
        2: mulai
            i := 20;
            j := 30;
        selesai;
        3: i := 30;
    selain-itu
        i := 0;
    selesai;
    
    { ===== TEST RECORD (REKAMAN) ===== }
    mahasiswa.npm := 13519999;
    mahasiswa.nama := 'A';
    mahasiswa.ipk := 3.75;
    mahasiswa.aktif := true;
    
    { ===== TEST ARRAY (LARIK DARI) ===== }
    untuk i := 1 ke 10 lakukan
    mulai
        angka[i] := i * i;
    selesai;
    
    { ===== TEST FUNCTION CALL (FUNGSI) ===== }
    k := tambah(5, 10);
    hasil := rata_rata(angka, 10);
    
    { ===== TEST PROCEDURE CALL (PROSEDUR) ===== }
    cetak_info(13519999, 'B');
    inisialisasi_array;
    
    { ===== TEST CONSTANT (KONSTANTA) ===== }
    i := MAX_SIZE;
    j := MIN_VALUE;
    hasil := PI * 2;
    
    { ===== TEST TYPE (TIPE) ===== }
    status := aktif;
    
    { ===== TEST DATA TYPES ===== }
    i := 42;                    { integer }
    hasil := 3.14159;           { real }
    selesai_flag := true;       { boolean }
    huruf := 'Z';               { char }
    
    { ===== TEST RELATIONAL OPERATORS ===== }
    jika i < j maka
        k := 1;
    
    jika i <= j maka
        k := 2;
    
    jika i > j maka
        k := 3;
    
    jika i >= j maka
        k := 4;
    
    jika i <> j maka
        k := 5;
    
    jika i = j maka
        k := 6;
    
    { ===== TEST NESTED LOOPS ===== }
    untuk i := 1 ke 5 lakukan
    mulai
        untuk j := 1 ke 5 lakukan
        mulai
            selama k < 100 lakukan
            mulai
                k := k + 1;
            selesai;
        selesai;
    selesai;
    
    { ===== TEST COMPLEX EXPRESSIONS ===== }
    hasil := ((i + j) * k - total) bagi (i mod j + 1);
    selesai_flag := ((i < j) dan (j < k)) atau (tidak (k > MAX_SIZE));
    
    { ===== TEST ALL KEYWORDS IN ONE STATEMENT ===== }
    jika (i < MAX_SIZE) dan (j > MIN_VALUE) maka
    mulai
        untuk k := 1 ke 10 lakukan
        mulai
            ulangi
                i := i + 1;
            samapai i >= j;
        selesai;
    selesai
    selain-itu
    mulai
        selama i < j lakukan
            i := i + 1;
    selesai;
    
selesai.
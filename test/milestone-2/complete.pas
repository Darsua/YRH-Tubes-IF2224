program TestSyntaxLengkap;

{ Test case komprehensif untuk semua fitur syntax Pascal-S dengan keyword Indonesia }

konstanta
    MAX_SISWA = 100;
    MIN_NILAI = 0;
    MAX_NILAI = 100;
    PI = 314;
    NAMA_SEKOLAH = 'SMA Negeri 1';

tipe
    NilaiRange = integer;
    DaftarNilai = larik [1..10] dari integer;
    MatriksNilai = larik [1..5] dari larik [1..5] dari real;

variabel
    i, j, k: integer;
    nilai, rata_rata, total: real;
    lulus: boolean;
    grade: char;
    nama: char;
    daftar_nilai: DaftarNilai;
    jumlah_siswa, siswa_lulus: integer;

{ Fungsi untuk menghitung faktorial }
fungsi faktorial(n: integer): integer;
variabel
    hasil, counter: integer;
mulai
    hasil := 1;
    counter := 1;
    
    selama counter <= n lakukan
    mulai
        hasil := hasil * counter;
        counter := counter + 1;
    selesai;
    
    faktorial := hasil;
selesai;

{ Fungsi untuk menghitung rata-rata }
fungsi hitung_rata_rata(nilai1, nilai2, nilai3: real): real;
variabel
    jumlah: real;
mulai
    jumlah := nilai1 + nilai2 + nilai3;
    hitung_rata_rata := jumlah bagi 3;
selesai;

{ Fungsi untuk cek bilangan prima }
fungsi is_prima(num: integer): boolean;
variabel
    pembagi, batas: integer;
    prima_flag: boolean;
mulai
    jika num < 2 maka
    mulai
        is_prima := false;
    selesai
    selain_itu
    mulai
        prima_flag := true;
        pembagi := 2;
        batas := num bagi 2;
        
        selama (pembagi <= batas) dan prima_flag lakukan
        mulai
            jika (num mod pembagi) = 0 maka
            mulai
                prima_flag := false;
            selesai;
            pembagi := pembagi + 1;
        selesai;
        
        is_prima := prima_flag;
    selesai;
selesai;

{ Prosedur untuk inisialisasi array }
prosedur init_array;
variabel
    idx: integer;
mulai
    untuk idx := 1 ke 10 lakukan
    mulai
        daftar_nilai[idx] := 0;
    selesai;
selesai;

{ Prosedur untuk print hasil }
prosedur print_hasil(msg: char; angka: integer);
mulai
    writeln(msg, angka);
selesai;

{ Prosedur untuk sorting sederhana (bubble sort) }
prosedur sort_nilai;
variabel
    x, y, temp: integer;
mulai
    untuk x := 1 ke 9 lakukan
    mulai
        untuk y := 1 ke (10 - x) lakukan
        mulai
            jika daftar_nilai[y] > daftar_nilai[y + 1] maka
            mulai
                temp := daftar_nilai[y];
                daftar_nilai[y] := daftar_nilai[y + 1];
                daftar_nilai[y + 1] := temp;
            selesai;
        selesai;
    selesai;
selesai;

{ Program Utama }
mulai
    { ========== TEST 1: Variabel dan Assignment ========== }
    i := 10;
    j := 20;
    k := i + j;
    
    nilai := 85;
    rata_rata := 0;
    lulus := false;
    grade := 'A';
    
    { ========== TEST 2: Operasi Aritmatika ========== }
    total := nilai + 10;
    total := total - 5;
    total := total * 2;
    total := total bagi 3;
    k := total mod 7;
    
    { ========== TEST 3: Operasi Logika ========== }
    lulus := (nilai > 60) dan (nilai < 100);
    lulus := (nilai < MIN_NILAI) atau (nilai > MAX_NILAI);
    lulus := tidak lulus;
    
    { ========== TEST 4: If-Then-Else ========== }
    jika nilai > 80 maka
    mulai
        grade := 'A';
        lulus := true;
    selesai
    selain_itu
    mulai
        jika nilai > 70 maka
        mulai
            grade := 'B';
            lulus := true;
        selesai
        selain_itu
        mulai
            grade := 'C';
            lulus := false;
        selesai;
    selesai;
    
    { ========== TEST 5: While Loop ========== }
    i := 0;
    total := 0;
    selama i < 10 lakukan
    mulai
        total := total + i;
        i := i + 1;
    selesai;
    
    { ========== TEST 6: For Loop Ascending ========== }
    total := 0;
    untuk i := 1 ke 10 lakukan
    mulai
        total := total + i;
        daftar_nilai[i] := i * 2;
    selesai;
    
    { ========== TEST 7: For Loop Descending ========== }
    j := 0;
    untuk i := 10 turun_ke 1 lakukan
    mulai
        j := j + daftar_nilai[i];
    selesai;
    
    { ========== TEST 8: Nested Loops ========== }
    untuk i := 1 ke 5 lakukan
    mulai
        untuk j := 1 ke 5 lakukan
        mulai
            k := i * j;
        selesai;
    selesai;
    
    { ========== TEST 9: Function Calls ========== }
    k := faktorial(5);
    rata_rata := hitung_rata_rata(80, 90, 85);
    lulus := is_prima(17);
    
    { ========== TEST 10: Procedure Calls ========== }
    init_array;
    print_hasil('Hasil: ', k);
    sort_nilai;
    
    { ========== TEST 11: Array Operations ========== }
    untuk i := 1 ke 10 lakukan
    mulai
        daftar_nilai[i] := i * 10;
    selesai;
    
    total := 0;
    untuk i := 1 ke 10 lakukan
    mulai
        total := total + daftar_nilai[i];
    selesai;
    
    { ========== TEST 12: Complex Expressions ========== }
    rata_rata := ((nilai + 10) * 2 - 5) bagi (3 + 2);
    lulus := ((nilai > 70) dan (nilai < 90)) atau (tidak (nilai < 60));
    k := (i + j) * (k - i) bagi (j mod 3 + 1);
    
    { ========== TEST 13: Nested If-Else ========== }
    jika nilai > 90 maka
    mulai
        grade := 'A';
        jika nilai > 95 maka
        mulai
            writeln('Excellent!');
        selesai;
    selesai
    selain_itu
    mulai
        jika nilai > 80 maka
        mulai
            grade := 'B';
        selesai
        selain_itu
        mulai
            jika nilai > 70 maka
            mulai
                grade := 'C';
            selesai
            selain_itu
            mulai
                grade := 'D';
            selesai;
        selesai;
    selesai;
    
    { ========== TEST 14: Complex Loop Conditions ========== }
    i := 1;
    j := 10;
    selama (i < j) dan (i < MAX_SISWA) lakukan
    mulai
        i := i + 1;
        j := j - 1;
        
        jika (i mod 2) = 0 maka
        mulai
            k := k + i;
        selesai;
    selesai;
    
    { ========== TEST 15: Multiple Function Calls ========== }
    k := faktorial(faktorial(3));
    rata_rata := hitung_rata_rata(
        hitung_rata_rata(80, 85, 90),
        hitung_rata_rata(75, 80, 85),
        hitung_rata_rata(90, 95, 100)
    );
    
    { ========== TEST 16: Final Summary ========== }
    writeln('Test Selesai!');
    writeln('Total nilai: ', total);
    writeln('Rata-rata: ', rata_rata);
    writeln('Grade: ', grade);
    
selesai.

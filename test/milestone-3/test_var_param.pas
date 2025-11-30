program TestVarParameter;

konstanta
    MAX_SIZE = 100;
    PI = 314;
    adalah_benar = true;

variabel
    a, b: integer;
    x, y: real;
    hasil: integer;

{ Test procedure with var parameter (by reference) }
prosedur tukar(variabel p, q: integer);
variabel
    temp: integer;
mulai
    temp := p;
    p := q;
    q := temp
selesai;

{ Test procedure with normal parameter (by value) }
prosedur incrementByValue(n: integer);
mulai
    n := n + 1
selesai;

{ Test function with var parameter }
fungsi hitung(variabel nilai1, nilai2: integer): integer;
variabel
    jumlah: integer;
mulai
    jumlah := nilai1 + nilai2;
    nilai1 := nilai1 * 2;
    hitung := jumlah
selesai;

{ Test procedure with mixed parameters }
prosedur campuran(a: integer; variabel b: integer; c: real);
mulai
    b := a + b
selesai;

mulai
    { Test constant value in adr field }
    a := MAX_SIZE;
    b := PI;
    
    { Test var parameter }
    a := 5;
    b := 10;
    tukar(a, b);
    writeln('Setelah tukar: a=', a, ', b=', b);
    
    { Test value parameter }
    a := 5;
    incrementByValue(a);
    writeln('Setelah increment: a=', a);
    
    { Test function with var parameter }
    a := 3;
    b := 7;
    hasil := hitung(a, b);
    writeln('Hasil=', hasil, ', a=', a);
    
    { Test mixed parameters }
    a := 10;
    b := 20;
    campuran(a, b, 3.14);
    writeln('Hasil campuran: b=', b)
selesai.

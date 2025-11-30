program TestConstRedeclaration;

{ Test Case: Constant Redeclaration }

konstanta
    MAX = 100;
    MIN = 0;
    MAX = 50;  { Error: MAX already declared }

variabel
    x: integer;

mulai
    x := MAX + MIN
selesai.

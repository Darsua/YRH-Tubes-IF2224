program TestRedeclaration;

{ Test Case 3: Error - Redeclaration of Identifiers }

variabel
    x: integer;
    y: real;
    x: boolean;  { Error: x already declared }

prosedur TestProc(a: integer);
variabel
    a: real;  { Error: a already declared as parameter }
    b: integer;
    b: char;  { Error: b already declared }
mulai
    b := 5;
selesai;

mulai
    x := 10;
selesai.

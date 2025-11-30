program TestTypeRedeclaration;

{ Test Case: Type Redeclaration }

tipe
    Counter = integer;
    Index = integer;
    Counter = real;  { Error: Counter already declared }

variabel
    c: Counter;
    idx: Index;

mulai
    c := 10;
    idx := 1
selesai.

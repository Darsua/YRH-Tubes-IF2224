program TestUndeclaredVar;

{ Test Case 1: Error - Undeclared Variable }

variabel
    x: integer;

mulai
    x := 5;
    y := 10;  { Error: y not declared }
    z := x + y;  { Error: z and y not declared }
selesai.

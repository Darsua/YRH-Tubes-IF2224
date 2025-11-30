program TestForNonVariable;

{ Test Case: For loop with non-variable identifier }

konstanta
    LIMIT = 10;

variabel
    x: integer;

prosedur dummy;
mulai
selesai;

mulai
    { Error: LIMIT is a constant, not a variable }
    untuk LIMIT := 1 ke 10 lakukan
        x := x + 1;

    { Error: dummy is a procedure, not a variable }
    untuk dummy := 1 ke 5 lakukan
        x := x + 1
selesai.

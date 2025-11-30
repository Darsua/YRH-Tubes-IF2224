program TestForUndeclared;

{ Test Case: For loop with undeclared variable }

variabel
    x, y: integer;

mulai
    { Error: i is not declared }
    untuk i := 1 ke 10 lakukan
        x := x + 1;

    { Error: j is not declared }
    untuk j := 10 turun_ke 1 lakukan
        y := y - 1
selesai.

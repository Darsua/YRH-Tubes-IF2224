program TestNestedScopes;

{ Test Case: Nested scopes and variable shadowing }

variabel
    x: integer;
    y: integer;

prosedur outer(a: integer);
variabel
    x: integer;  { Shadows global x }
    z: integer;

    prosedur inner(b: integer);
    variabel
        x: integer;  { Shadows outer's x and global x }
        w: integer;
    mulai
        x := b;      { Refers to inner's x }
        w := a;      { Refers to outer's parameter a }
        z := 100     { Refers to outer's z }
    selesai;

mulai
    x := a;          { Refers to outer's x }
    z := 50;
    inner(10)
selesai;

fungsi compute(n: integer): integer;
variabel
    result: integer;
    y: integer;      { Shadows global y }
mulai
    y := n * 2;
    result := y + x; { y is local, x is global }
    compute := result
selesai;

mulai
    x := 5;
    y := 10;
    outer(15);
    y := compute(20)
selesai.

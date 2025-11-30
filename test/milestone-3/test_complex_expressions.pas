program TestComplexExpressions;

{ Test Case: Complex expressions with operators and type inference }

variabel
    a, b, c: integer;
    x, y, z: real;
    flag1, flag2, result: boolean;

mulai
    { Integer arithmetic }
    a := 10;
    b := 20;
    c := a + b * 2 - 5;
    c := (a + b) * (c - a);

    { Real arithmetic }
    x := 3.14;
    y := 2.5;
    z := x + y;
    z := x * y bagi 2.0;

    { Mixed arithmetic (integer and real) }
    z := a + x;
    z := b * y;

    { Integer division and modulo }
    c := b bagi a;
    c := b mod a;

    { Relational operations }
    flag1 := a < b;
    flag1 := a <= b;
    flag1 := a > b;
    flag1 := a >= b;
    flag1 := a = b;
    flag1 := a <> b;

    { Real comparisons }
    flag2 := x < y;
    flag2 := x = y;
    flag2 := x <> y;

    { Logical operations }
    result := flag1 dan flag2;
    result := flag1 atau flag2;
    result := tidak flag1;
    result := (a > b) dan (x < y);
    result := (a = 10) atau (b = 20);

    { Unary operations }
    c := -a;
    c := -(a + b);
    x := -y;
    flag1 := tidak flag2;

    { Complex nested expressions }
    result := ((a > 0) dan (b < 100)) atau ((x >= y) dan tidak flag2);
    c := ((a + b) * 2) - ((c bagi 3) + (b mod 5));
    z := (x + y) * (z - 1.5) bagi 2.0;

    { Expressions in assignments with operations }
    a := a + 1;
    b := b - 1;
    c := c * 2;
    x := x bagi 2.0;

    { Combined expressions }
    flag1 := (a + b * c) > (b - a bagi 2);
    result := (x * y > 10.0) dan (a < 50) atau flag2
selesai.

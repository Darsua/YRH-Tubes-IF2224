program TestComplexCases;

{ Test Case 4: Complex Edge Cases with Nested Scopes and Multi-dimensional Arrays }

tipe
    Row = larik[1..3] dari integer;
    Matrix2D = larik[1..3] dari Row;
    Plane = larik[1..2] dari real;
    Matrix3D = larik[1..2] dari Plane;

variabel
    mat2d: Matrix2D;
    mat3d: Matrix3D;
    i, j, k: integer;
    sum: integer;
    average: real;

prosedur NestedProc(x: integer; variabel y: integer);
variabel
    local1: integer;
    
    prosedur InnerProc(a: integer);
    variabel
        local2: integer;
    mulai
        local2 := a + x;  { Access outer procedure parameter }
        local1 := local2;  { Access outer procedure local variable }
        y := local1;  { Modify outer var parameter }
    selesai;
    
mulai
    local1 := x * 2;
    InnerProc(local1);
selesai;

fungsi Calculate(a, b: integer): integer;
variabel
    result: integer;
mulai
    result := a + b;
    Calculate := result;
selesai;

mulai
    { Test multi-dimensional array access }
    untuk i := 1 ke 3 lakukan
        untuk j := 1 ke 3 lakukan
            mat2d[i][j] := i * j;
    
    { Test 3D array }
    untuk i := 1 ke 2 lakukan
        untuk j := 1 ke 2 lakukan
            untuk k := 1 ke 2 lakukan
                mat3d[i][j][k] := i + j + k;
    
    { Test nested procedure calls }
    sum := 0;
    NestedProc(10, sum);
    
    { Test function call in expression }
    sum := Calculate(5, 10) + Calculate(3, 7);
    
    { Test complex expression with nested array access }
    average := (mat3d[1][1][1] + mat3d[2][2][2]) / 2.0;
selesai.

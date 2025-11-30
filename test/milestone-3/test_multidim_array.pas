program TestMultiDimensionalArrays;

{ Test program for multi-dimensional array support }

tipe
    Vector = larik[1..5] dari integer;
    Matrix2D = larik[1..3] dari larik[1..4] dari real;
    Matrix3D = larik[1..2] dari larik[1..3] dari larik[1..4] dari integer;
    Cube = larik[0..9] dari larik[0..9] dari larik[0..9] dari char;

variabel
    v : Vector;
    m : Matrix2D;
    cube3d : Matrix3D;
    data : Cube;
    i, j, k : integer;
    total : real;

mulai
    { Initialize simple vector }
    untuk i := 1 ke 5 lakukan
        v[i] := i * 10;
    
    { Initialize 2D matrix }
    untuk i := 1 ke 3 lakukan
        untuk j := 1 ke 4 lakukan
            m[i][j] := i * j;
    
    { Calculate sum of 2D matrix }
    total := 0;
    untuk i := 1 ke 3 lakukan
        untuk j := 1 ke 4 lakukan
            total := total + m[i][j];
    
    { Initialize 3D matrix }
    untuk i := 1 ke 2 lakukan
        untuk j := 1 ke 3 lakukan
            untuk k := 1 ke 4 lakukan
                cube3d[i][j][k] := i + j + k;
    
    { Use the total }
    i := 0
selesai.

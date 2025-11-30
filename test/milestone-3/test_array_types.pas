program TestArrayTypes;

{ Test Case: Array type definitions and usage }

tipe
    IntArray = larik[1..10] dari integer;
    CharArray = larik[0..25] dari char;
    RealArray = larik[1..5] dari real;
    BoolArray = larik[1..3] dari boolean;

variabel
    numbers: IntArray;
    letters: CharArray;
    scores: RealArray;
    flags: BoolArray;
    i: integer;
    total: integer;

mulai
    { Initialize integer array }
    untuk i := 1 ke 10 lakukan
        numbers[i] := i * i;

    { Initialize char array }
    untuk i := 0 ke 25 lakukan
        letters[i] := 'A';

    { Initialize real array }
    scores[1] := 85.5;
    scores[2] := 90.0;
    scores[3] := 78.5;
    scores[4] := 92.0;
    scores[5] := 88.0;

    { Initialize boolean array }
    flags[1] := true;
    flags[2] := false;
    flags[3] := true;

    { Calculate sum of array }
    total := 0;
    untuk i := 1 ke 10 lakukan
        total := total + numbers[i];

    { Use array elements }
    numbers[5] := total;
    letters[0] := 'Z'
selesai.

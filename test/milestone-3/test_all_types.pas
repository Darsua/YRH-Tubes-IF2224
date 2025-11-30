program TestAllDataTypes;

{ Test Case: Comprehensive test for all data types }

konstanta
    MAX_INT = 2147483647;
    MIN_INT = -2147483648;
    ZERO = 0;
    PI = 3.14159;
    E = 2.71828;
    TRUE_FLAG = true;
    FALSE_FLAG = false;
    NEWLINE = '\n';
    TAB = '\t';
    LETTER_A = 'A';
    LETTER_Z = 'Z';

tipe
    IntArray = larik[1..10] dari integer;
    RealMatrix = larik[1..3] dari larik[1..3] dari real;
    CharArray = larik[0..255] dari char;

variabel
    { Integer variables }
    intVar1, intVar2, intSum: integer;

    { Real variables }
    realVar1, realVar2, realResult: real;

    { Boolean variables }
    boolVar1, boolVar2, boolResult: boolean;

    { Character variables }
    charVar1, charVar2: char;

    { Array variables }
    numbers: IntArray;
    matrix: RealMatrix;
    text: CharArray;

    { Loop counters }
    i, j, k: integer;

prosedur testIntegers;
variabel
    temp: integer;
mulai
    { Integer operations }
    intVar1 := MAX_INT;
    intVar2 := MIN_INT;
    intSum := ZERO;

    { Arithmetic operations }
    temp := intVar1 + intVar2;
    temp := intVar1 - intVar2;
    temp := intVar1 * 2;
    temp := intVar1 bagi 2;
    temp := intVar1 mod 10;

    { Negative numbers }
    temp := -intVar1;
    temp := -(-intVar2)
selesai;

prosedur testReals;
variabel
    temp: real;
mulai
    { Real operations }
    realVar1 := PI;
    realVar2 := E;

    { Arithmetic operations }
    temp := realVar1 + realVar2;
    temp := realVar1 - realVar2;
    temp := realVar1 * realVar2;
    temp := realVar1 bagi realVar2;

    { Negative reals }
    temp := -realVar1;
    temp := realVar1 * -1.0
selesai;

prosedur testBooleans;
variabel
    temp: boolean;
mulai
    { Boolean values }
    boolVar1 := true;
    boolVar2 := false;
    boolVar1 := TRUE_FLAG;
    boolVar2 := FALSE_FLAG;

    { Logical operations }
    temp := boolVar1 dan boolVar2;
    temp := boolVar1 atau boolVar2;
    temp := tidak boolVar1;
    temp := tidak (boolVar1 dan boolVar2);

    { Comparison results }
    temp := intVar1 > intVar2;
    temp := intVar1 < intVar2;
    temp := intVar1 = intVar2;
    temp := intVar1 <> intVar2;
    temp := intVar1 >= intVar2;
    temp := intVar1 <= intVar2;

    { Real comparisons }
    temp := realVar1 > realVar2;
    temp := realVar1 = realVar2;
    temp := realVar1 <> realVar2
selesai;

prosedur testCharacters;
variabel
    temp: char;
mulai
    { Character operations }
    charVar1 := 'A';
    charVar2 := 'Z';
    charVar1 := LETTER_A;
    charVar2 := LETTER_Z;

    { Special characters }
    temp := NEWLINE;
    temp := TAB;

    { Character comparisons }
    boolResult := charVar1 < charVar2;
    boolResult := charVar1 = charVar2;
    boolResult := charVar1 <> charVar2
selesai;

prosedur testArrays;
variabel
    sum: integer;
    avg: real;
mulai
    { Initialize integer array }
    untuk i := 1 ke 10 lakukan
        numbers[i] := i * 10;

    { Calculate sum }
    sum := 0;
    untuk i := 1 ke 10 lakukan
        sum := sum + numbers[i];

    { Initialize 2D real array }
    untuk i := 1 ke 3 lakukan
        untuk j := 1 ke 3 lakukan
            matrix[i][j] := i * j;

    { Calculate average of matrix }
    avg := 0.0;
    untuk i := 1 ke 3 lakukan
        untuk j := 1 ke 3 lakukan
            avg := avg + matrix[i][j];
    avg := avg bagi 9.0;

    { Initialize character array }
    untuk i := 0 ke 255 lakukan
        text[i] := 'X';

    { Access array elements }
    intSum := numbers[1] + numbers[10];
    realResult := matrix[2][2];
    charVar1 := text[0]
selesai;

fungsi computeSum(a: integer; b: integer): integer;
variabel
    result: integer;
mulai
    result := a + b;
    computeSum := result
selesai;

fungsi computeAverage(x: real; y: real): real;
variabel
    result: real;
mulai
    result := (x + y) bagi 2.0;
    computeAverage := result
selesai;

fungsi isPositive(n: integer): boolean;
mulai
    isPositive := n > 0
selesai;

fungsi getFirstChar(): char;
mulai
    getFirstChar := 'A'
selesai;

mulai
    { Test all data types }
    testIntegers;
    testReals;
    testBooleans;
    testCharacters;
    testArrays;

    { Test functions with different return types }
    intSum := computeSum(10, 20);
    realResult := computeAverage(5.5, 10.5);
    boolResult := isPositive(100);
    charVar1 := getFirstChar();

    { Mixed type expressions }
    realResult := intVar1 + realVar1;
    realResult := intSum * PI;

    { Complex boolean expressions }
    boolResult := (intVar1 > 0) dan (realVar1 < 10.0);
    boolResult := isPositive(intSum) atau (charVar1 = 'A');

    { Nested function calls }
    intSum := computeSum(computeSum(5, 10), computeSum(15, 20));
    boolResult := isPositive(computeSum(intVar1, intVar2));

    { Array with constants }
    numbers[1] := MAX_INT;
    numbers[2] := ZERO;
    matrix[1][1] := PI;
    matrix[2][2] := E;
    text[0] := LETTER_A;
    text[255] := LETTER_Z
selesai.

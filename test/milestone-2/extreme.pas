program ExtremeCompilerStressTest;

{ ========================================================================
  EXTREME COMPREHENSIVE STRESS TEST
  Tests every single grammar production rule with maximum complexity
  - Deeply nested structures (10+ levels)
  - Complex type definitions with nested arrays and records
  - Recursive-like functions with complex control flow
  - All operators and expressions in extreme combinations
  - Edge cases: negative numbers, empty strings, character escapes
  - Maximum parameter lists
  - All loop types with complex conditions
  ======================================================================== }

konstanta
    { Test negative numbers and floating point }
    NEG_INT = -2147483648;
    NEG_FLOAT = -3.14159265359;
    POS_FLOAT = 2.71828182846;
    ZERO = 0;
    ZERO_FLOAT = 0.0;
    EPSILON = 0.00001;
    
    { Test string edge cases }
    EMPTY = '';
    SINGLE_CHAR = 'X';
    LONG_STRING = 'The quick brown fox jumps over the lazy dog';
    ESCAPE_TEST = 'Line1\nLine2\tTabbed';
    
    { Test character constants }
    NEWLINE = '\n';
    TAB = '\t';
    QUOTE = '\'';
    
    { Test boolean-like constants }
    MAX_INT = 2147483647;
    MIN_INT = -2147483648;
    MAX_ITERATIONS = 1000;

tipe
    { Simple type aliases }
    Integer = integer;
    Real = real;
    Boolean = boolean;
    Char = char;
    
    { Subrange types }
    Digit = 0..9;
    Letter = 'A'..'Z';
    Percentage = 0..100;
    
    { Single dimension arrays }
    IntArray = larik [1..100] dari integer;
    RealArray = larik [0..99] dari real;
    CharArray = larik [1..256] dari char;
    BoolArray = larik [0..31] dari boolean;
    
    { Multi-dimensional arrays }
    Matrix2D = larik [1..10] dari larik [1..10] dari real;
    Matrix3D = larik [1..5] dari larik [1..5] dari larik [1..5] dari integer;
    Matrix4D = larik [1..3] dari larik [1..3] dari larik [1..3] dari larik [1..3] dari real;
    
    { Nested array with subrange }
    DigitMatrix = larik [0..9] dari larik [0..9] dari Digit;
    
    { Arrays of arrays using type aliases }
    IntArrayArray = larik [1..10] dari IntArray;
    MatrixArray = larik [1..5] dari Matrix2D;
    
    { Record types }
    Point = rekaman
        x, y, z: real;
        label: char;
        active: boolean;
    selesai;
    
    Vector = rekaman
        magnitude: real;
        direction: real;
        normalized: boolean;
    selesai;
    
    NestedRecord = rekaman
        id: integer;
        position: Point;
        velocity: Vector;
        name: CharArray;
        timestamp: real;
    selesai;
    
    ComplexRecord = rekaman
        header: rekaman
            version: integer;
            checksum: integer;
        selesai;
        data: Matrix3D;
        footer: rekaman
            status: boolean;
            error_code: integer;
        selesai;
    selesai;
    
    { Arrays of records }
    PointArray = larik [1..100] dari Point;
    RecordMatrix = larik [1..10] dari larik [1..10] dari NestedRecord;

variabel
    { Simple variables of all basic types }
    i, j, k, m, n, p, q, r, s, t: integer;
    x, y, z, w, u, v: real;
    flag1, flag2, flag3, flag4, flag5: boolean;
    ch1, ch2, ch3: char;
    
    { Arrays }
    numbers: IntArray;
    values: RealArray;
    characters: CharArray;
    flags: BoolArray;
    
    { Multi-dimensional arrays }
    matrix: Matrix2D;
    cube: Matrix3D;
    hypercube: Matrix4D;
    digitGrid: DigitMatrix;
    
    { Records }
    point1, point2, point3: Point;
    vec1, vec2: Vector;
    nested: NestedRecord;
    complex: ComplexRecord;
    
    { Arrays of records }
    points: PointArray;
    recordGrid: RecordMatrix;
    
    { Complex nested types }
    matrixCollection: MatrixArray;
    intArrays: IntArrayArray;
    
    { Test variables }
    result, temp, counter, sum: integer;
    average, total, ratio: real;
    found, valid, ready, done: boolean;

{ ========================================================================
  UTILITY FUNCTIONS - Test function declarations with various signatures
  ======================================================================== }

{ Function with no parameters }
fungsi getConstant: real;
mulai
    getConstant := 3.14159;
selesai;

{ Function with single parameter }
fungsi square(n: integer): integer;
mulai
    square := n * n;
selesai;

{ Function with multiple parameters of same type }
fungsi add(a, b, c: integer): integer;
mulai
    add := a + b + c;
selesai;

{ Function with multiple parameters of different types }
fungsi multiply(a: integer; b: real; c: integer): real;
mulai
    multiply := a * b * c;
selesai;

{ Function with many parameters (stress test) }
fungsi complexCalc(a, b: integer; x, y, z: real; flag: boolean): real;
variabel
    temp1, temp2: real;
mulai
    jika flag maka
    mulai
        temp1 := (a + b) * x;
        temp2 := y bagi (z + 0.001);
        complexCalc := temp1 + temp2;
    selesai
    selain_itu
    mulai
        complexCalc := 0.0 - (x + y + z);
    selesai;
selesai;

{ ========================================================================
  MATHEMATICAL FUNCTIONS - Complex expressions and operators
  ======================================================================== }

fungsi absolute(n: integer): integer;
mulai
    jika n < 0 maka
    mulai
        absolute := 0 - n;
    selesai
    selain_itu
    mulai
        absolute := n;
    selesai;
selesai;

fungsi max(a, b: integer): integer;
mulai
    jika a > b maka
    mulai
        max := a;
    selesai
    selain_itu
    mulai
        max := b;
    selesai;
selesai;

fungsi min(a, b: integer): integer;
mulai
    jika a < b maka
    mulai
        min := a;
    selesai
    selain_itu
    mulai
        min := b;
    selesai;
selesai;

fungsi power(base: real; exponent: integer): real;
variabel
    result: real;
    i: integer;
mulai
    result := 1.0;
    
    jika exponent > 0 maka
    mulai
        untuk i := 1 ke exponent lakukan
        mulai
            result := result * base;
        selesai;
    selesai
    selain_itu
    mulai
        jika exponent < 0 maka
        mulai
            untuk i := 1 ke (0 - exponent) lakukan
            mulai
                result := result * base;
            selesai;
            result := 1.0 bagi result;
        selesai;
    selesai;
    
    power := result;
selesai;

fungsi factorial(n: integer): integer;
variabel
    result, i: integer;
mulai
    result := 1;
    untuk i := 2 ke n lakukan
    mulai
        result := result * i;
    selesai;
    factorial := result;
selesai;

fungsi fibonacci(n: integer): integer;
variabel
    a, b, temp, counter: integer;
mulai
    jika n <= 1 maka
    mulai
        fibonacci := n;
    selesai
    selain_itu
    mulai
        a := 0;
        b := 1;
        counter := 2;
        
        selama counter <= n lakukan
        mulai
            temp := a + b;
            a := b;
            b := temp;
            counter := counter + 1;
        selesai;
        
        fibonacci := b;
    selesai;
selesai;

fungsi gcd(a, b: integer): integer;
variabel
    temp: integer;
mulai
    selama b <> 0 lakukan
    mulai
        temp := a mod b;
        a := b;
        b := temp;
    selesai;
    gcd := a;
selesai;

fungsi lcm(a, b: integer): integer;
mulai
    lcm := (a * b) bagi gcd(a, b);
selesai;

{ ========================================================================
  BOOLEAN AND LOGICAL FUNCTIONS - Test all logical operators
  ======================================================================== }

fungsi isPrime(n: integer): boolean;
variabel
    i: integer;
    prime: boolean;
mulai
    jika n <= 1 maka
    mulai
        isPrime := false;
    selesai
    selain_itu
    mulai
        prime := true;
        i := 2;
        
        selama (i * i <= n) dan prime lakukan
        mulai
            jika n mod i = 0 maka
            mulai
                prime := false;
            selesai;
            i := i + 1;
        selesai;
        
        isPrime := prime;
    selesai;
selesai;

fungsi isEven(n: integer): boolean;
mulai
    isEven := n mod 2 = 0;
selesai;

fungsi isOdd(n: integer): boolean;
mulai
    isOdd := tidak (n mod 2 = 0);
selesai;

fungsi inRange(val, low, high: integer): boolean;
mulai
    inRange := (val >= low) dan (val <= high);
selesai;

fungsi complexBoolean(a, b, c: integer): boolean;
variabel
    temp1, temp2, temp3: boolean;
mulai
    temp1 := (a > 0) dan (b > 0);
    temp2 := (c >= 0) atau (c <= -100);
    temp3 := tidak ((a = b) atau (b = c));
    
    complexBoolean := (temp1 dan temp2) atau temp3;
selesai;

{ ========================================================================
  ARRAY MANIPULATION PROCEDURES - Test array indexing
  ======================================================================== }

prosedur fillArray(start, step: integer);
variabel
    idx, value: integer;
mulai
    value := start;
    untuk idx := 1 ke 100 lakukan
    mulai
        numbers[idx] := value;
        value := value + step;
    selesai;
selesai;

prosedur fillMatrix;
variabel
    row, col: integer;
mulai
    untuk row := 1 ke 10 lakukan
    mulai
        untuk col := 1 ke 10 lakukan
        mulai
            matrix[row][col] := row * 10.0 + col;
        selesai;
    selesai;
selesai;

prosedur fillCube;
variabel
    i, j, k: integer;
mulai
    untuk i := 1 ke 5 lakukan
    mulai
        untuk j := 1 ke 5 lakukan
        mulai
            untuk k := 1 ke 5 lakukan
            mulai
                cube[i][j][k] := i * 100 + j * 10 + k;
            selesai;
        selesai;
    selesai;
selesai;

prosedur fillHypercube;
variabel
    i, j, k, l: integer;
mulai
    untuk i := 1 ke 3 lakukan
    mulai
        untuk j := 1 ke 3 lakukan
        mulai
            untuk k := 1 ke 3 lakukan
            mulai
                untuk l := 1 ke 3 lakukan
                mulai
                    hypercube[i][j][k][l] := i * 1000.0 + j * 100.0 + k * 10.0 + l;
                selesai;
            selesai;
        selesai;
    selesai;
selesai;

fungsi sumArray: integer;
variabel
    idx, total: integer;
mulai
    total := 0;
    untuk idx := 1 ke 100 lakukan
    mulai
        total := total + numbers[idx];
    selesai;
    sumArray := total;
selesai;

fungsi sumMatrix: real;
variabel
    row, col: integer;
    total: real;
mulai
    total := 0.0;
    untuk row := 1 ke 10 lakukan
    mulai
        untuk col := 1 ke 10 lakukan
        mulai
            total := total + matrix[row][col];
        selesai;
    selesai;
    sumMatrix := total;
selesai;

{ ========================================================================
  RECORD MANIPULATION - Test record field access
  ======================================================================== }

prosedur initPoint(x_val, y_val, z_val: real; lbl: char);
mulai
    point1.x := x_val;
    point1.y := y_val;
    point1.z := z_val;
    point1.label := lbl;
    point1.active := true;
selesai;

fungsi distanceFromOrigin(pt: Point): real;
variabel
    temp: real;
mulai
    temp := pt.x * pt.x + pt.y * pt.y + pt.z * pt.z;
    distanceFromOrigin := temp;
selesai;

prosedur copyPoint;
mulai
    point2.x := point1.x;
    point2.y := point1.y;
    point2.z := point1.z;
    point2.label := point1.label;
    point2.active := point1.active;
selesai;

{ ========================================================================
  DEEPLY NESTED CONTROL STRUCTURES - Stress test
  ======================================================================== }

prosedur extremelyNestedIf(depth: integer);
mulai
    jika depth > 0 maka
    mulai
        jika depth > 1 maka
        mulai
            jika depth > 2 maka
            mulai
                jika depth > 3 maka
                mulai
                    jika depth > 4 maka
                    mulai
                        jika depth > 5 maka
                        mulai
                            jika depth > 6 maka
                            mulai
                                jika depth > 7 maka
                                mulai
                                    jika depth > 8 maka
                                    mulai
                                        jika depth > 9 maka
                                        mulai
                                            result := 10;
                                        selesai
                                        selain_itu
                                        mulai
                                            result := 9;
                                        selesai;
                                    selesai
                                    selain_itu
                                    mulai
                                        result := 8;
                                    selesai;
                                selesai
                                selain_itu
                                mulai
                                    result := 7;
                                selesai;
                            selesai
                            selain_itu
                            mulai
                                result := 6;
                            selesai;
                        selesai
                        selain_itu
                        mulai
                            result := 5;
                        selesai;
                    selesai
                    selain_itu
                    mulai
                        result := 4;
                    selesai;
                selesai
                selain_itu
                mulai
                    result := 3;
                selesai;
            selesai
            selain_itu
            mulai
                result := 2;
            selesai;
        selesai
        selain_itu
        mulai
            result := 1;
        selesai;
    selesai
    selain_itu
    mulai
        result := 0;
    selesai;
selesai;

prosedur extremelyNestedLoops;
variabel
    i1, i2, i3, i4, i5, i6, i7, i8: integer;
mulai
    counter := 0;
    
    untuk i1 := 1 ke 2 lakukan
    mulai
        untuk i2 := 1 ke 2 lakukan
        mulai
            untuk i3 := 1 ke 2 lakukan
            mulai
                untuk i4 := 1 ke 2 lakukan
                mulai
                    untuk i5 := 1 ke 2 lakukan
                    mulai
                        untuk i6 := 1 ke 2 lakukan
                        mulai
                            untuk i7 := 1 ke 2 lakukan
                            mulai
                                untuk i8 := 1 ke 2 lakukan
                                mulai
                                    counter := counter + 1;
                                selesai;
                            selesai;
                        selesai;
                    selesai;
                selesai;
            selesai;
        selesai;
    selesai;
selesai;

{ ========================================================================
  COMPLEX EXPRESSION TESTS - All operators combined
  ======================================================================== }

fungsi megaExpression(a, b, c, d: integer): real;
variabel
    temp1, temp2, temp3, temp4: real;
    int1, int2: integer;
mulai
    { Arithmetic operators: +, -, *, /, bagi, mod }
    int1 := ((a + b) * c - d) bagi (a mod b + 1);
    int2 := (a * b) - (c bagi d) + (a mod c);
    
    { Relational operators: =, <>, <, <=, >, >= }
    jika (a = b) atau (a <> c) maka
    mulai
        temp1 := 1.0;
    selesai
    selain_itu
    mulai
        temp1 := 0.0;
    selesai;
    
    jika (a < b) dan (b <= c) dan (c > d) dan (d >= a) maka
    mulai
        temp2 := 2.0;
    selesai
    selain_itu
    mulai
        temp2 := 0.0;
    selesai;
    
    { Logical operators: dan, atau, tidak }
    jika ((a > 0) dan (b > 0)) atau (tidak (c < 0)) maka
    mulai
        temp3 := 3.0;
    selesai
    selain_itu
    mulai
        temp3 := 0.0;
    selesai;
    
    { Complex nested expression }
    temp4 := ((a + b * c) bagi (d + 1)) + 
             ((a - b) * (c + d)) bagi ((a mod b) + 1);
    
    { Combine everything }
    megaExpression := (temp1 + temp2 + temp3 + temp4) + int1 + int2;
selesai;

fungsi allOperatorsTest: boolean;
variabel
    a, b, c: integer;
    result1, result2, result3: boolean;
mulai
    a := 10;
    b := 20;
    c := 30;
    
    { Test all relational operators }
    result1 := (a = 10) dan (b <> 10) dan (a < b) dan (a <= b) dan (b > a) dan (b >= a);
    
    { Test logical operators with relational }
    result2 := ((a < b) dan (b < c)) atau (tidak (a > c));
    
    { Test arithmetic in boolean context }
    result3 := ((a + b) = c) dan ((c - b) = a) dan ((a * 2) < c);
    
    allOperatorsTest := result1 dan result2 dan result3;
selesai;

{ ========================================================================
  EDGE CASE TESTS - Negative numbers, zero, empty strings
  ======================================================================== }

prosedur testNegativeNumbers;
variabel
    neg1, neg2, neg3: integer;
    negf1, negf2: real;
mulai
    { Negative integer literals }
    neg1 := -100;
    neg2 := -2147483648;
    neg3 := 0 - 50;
    
    { Negative float literals }
    negf1 := -3.14159;
    negf2 := -0.001;
    
    { Arithmetic with negatives }
    result := (0 - 10) + (0 - 20) * (0 - 5);
    
    { Comparison with negatives }
    jika (neg1 < 0) dan (neg2 < neg1) maka
    mulai
        flag1 := true;
    selesai;
selesai;

prosedur testZeros;
variabel
    zero_int: integer;
    zero_real: real;
mulai
    zero_int := 0;
    zero_real := 0.0;
    
    { Operations with zero }
    result := zero_int + 0;
    total := zero_real + 0.0;
    
    { Division by near-zero (avoid actual division by zero) }
    total := 1.0 bagi (zero_real + 0.0001);
    
    { Zero comparisons }
    flag1 := zero_int = 0;
    flag2 := zero_real = 0.0;
    flag3 := zero_int <= 0;
    flag4 := zero_real >= 0.0;
selesai;

prosedur testStringEdgeCases;
mulai
    { Empty string }
    characters[1] := '';
    
    { Single character }
    ch1 := 'X';
    ch2 := '\n';
    ch3 := '\'';
    
    { String with escapes }
    characters[2] := 'Line1\nLine2';
    characters[3] := 'Tab\there';
selesai;

{ ========================================================================
  LOOP VARIATIONS - Test all loop types with edge cases
  ======================================================================== }

prosedur testForwardLoop;
variabel
    idx: integer;
mulai
    sum := 0;
    untuk idx := 1 ke 100 lakukan
    mulai
        sum := sum + idx;
    selesai;
selesai;

prosedur testBackwardLoop;
variabel
    idx: integer;
mulai
    sum := 0;
    untuk idx := 100 turun_ke 1 lakukan
    mulai
        sum := sum + idx;
    selesai;
selesai;

prosedur testWhileLoop;
variabel
    count: integer;
mulai
    count := 0;
    sum := 0;
    
    selama count < 100 lakukan
    mulai
        sum := sum + count;
        count := count + 1;
    selesai;
selesai;

prosedur testNestedForLoops;
variabel
    i, j, k: integer;
mulai
    sum := 0;
    
    untuk i := 1 ke 10 lakukan
    mulai
        untuk j := 1 ke 10 lakukan
        mulai
            untuk k := 1 ke 10 lakukan
            mulai
                sum := sum + (i * 100 + j * 10 + k);
            selesai;
        selesai;
    selesai;
selesai;

prosedur testComplexLoopConditions;
variabel
    count: integer;
mulai
    count := 0;
    
    selama (count < 100) dan (sum < 10000) lakukan
    mulai
        sum := sum + count * count;
        count := count + 1;
    selesai;
    
    selama (count > 0) atau (sum = 0) lakukan
    mulai
        count := count - 1;
        jika count < 0 maka
        mulai
            count := 0;
        selesai;
    selesai;
selesai;

{ ========================================================================
  PROCEDURE CALL TESTS - Various parameter combinations
  ======================================================================== }

prosedur noProcedure;
mulai
    result := 42;
selesai;

prosedur singleParamProc(value: integer);
mulai
    result := value * 2;
selesai;

prosedur multiParamProc(a, b, c: integer; x, y: real);
mulai
    result := a + b + c;
    total := x + y;
selesai;

prosedur manyParamProc(a, b, c, d, e, f, g, h: integer);
mulai
    result := a + b + c + d + e + f + g + h;
selesai;

prosedur nestedCalls;
mulai
    singleParamProc(square(5));
    multiParamProc(add(1, 2, 3), max(10, 20), min(5, 15), 3.14, 2.71);
    manyParamProc(1, 2, 3, 4, 5, 6, 7, 8);
selesai;

{ ========================================================================
  MAIN PROGRAM - Execute all stress tests
  ======================================================================== }

mulai
    { ===== Test 1: Initialize all variables ===== }
    i := 0;
    j := 0;
    k := 0;
    x := 0.0;
    y := 0.0;
    z := 0.0;
    flag1 := false;
    flag2 := false;
    ch1 := 'A';
    result := 0;
    sum := 0;
    total := 0.0;
    
    { ===== Test 2: Negative numbers and edge cases ===== }
    testNegativeNumbers();
    testZeros();
    testStringEdgeCases();
    
    { ===== Test 3: Mathematical functions ===== }
    i := absolute(-100);
    j := max(50, 75);
    k := min(50, 75);
    result := factorial(10);
    i := fibonacci(15);
    j := gcd(48, 18);
    k := lcm(12, 18);
    x := power(2.0, 10);
    y := power(10.0, -2);
    
    { ===== Test 4: Boolean and logical operations ===== }
    flag1 := isPrime(17);
    flag2 := isEven(42);
    flag3 := isOdd(43);
    flag4 := inRange(50, 0, 100);
    flag5 := complexBoolean(10, 20, 30);
    valid := allOperatorsTest();
    
    { ===== Test 5: Array operations ===== }
    fillArray(1, 2);
    fillMatrix();
    fillCube();
    fillHypercube();
    sum := sumArray();
    total := sumMatrix();
    
    { ===== Test 6: Record operations ===== }
    initPoint(3.0, 4.0, 5.0, 'P');
    copyPoint();
    x := distanceFromOrigin(point1);
    
    { ===== Test 7: Complex expressions ===== }
    x := megaExpression(10, 20, 30, 5);
    y := complexCalc(5, 10, 1.5, 2.5, 3.5, true);
    
    { ===== Test 8: Deeply nested structures ===== }
    extremelyNestedIf(10);
    extremelyNestedLoops();
    
    { ===== Test 9: Loop variations ===== }
    testForwardLoop();
    testBackwardLoop();
    testWhileLoop();
    testNestedForLoops();
    testComplexLoopConditions();
    
    { ===== Test 10: Procedure calls ===== }
    noParamProc();
    singleParamProc(100);
    multiParamProc(1, 2, 3, 4.5, 6.7);
    manyParamProc(1, 2, 3, 4, 5, 6, 7, 8);
    nestedCalls();
    
    { ===== Test 11: Ultra-complex nested expression ===== }
    result := ((fibonacci(10) + factorial(5)) * gcd(48, 18)) bagi 
              (max(absolute(-50), square(5)) + min(10, 20));
    
    x := ((power(2.0, 10) + power(10.0, -2)) * 3.14159) bagi 
         (megaExpression(5, 10, 15, 20) + 0.001);
    
    { ===== Test 12: Complex boolean expressions ===== }
    flag1 := ((result > 0) dan (result < MAX_INT)) atau 
             (tidak ((x = y) atau (y = z)));
    
    flag2 := (isPrime(17) dan isEven(42)) atau 
             (isOdd(43) dan inRange(50, 0, 100));
    
    flag3 := (((i > j) dan (j > k)) atau ((i < j) dan (j < k))) dan
             (tidak (i = j));
    
    { ===== Test 13: Array access in complex expressions ===== }
    numbers[1] := fibonacci(10);
    numbers[2] := factorial(5);
    numbers[3] := gcd(numbers[1], numbers[2]);
    
    matrix[1][1] := power(2.0, numbers[1] bagi 10);
    matrix[2][2] := megaExpression(numbers[1], numbers[2], numbers[3], 5);
    
    cube[1][1][1] := numbers[1] + numbers[2] + numbers[3];
    hypercube[1][1][1][1] := matrix[1][1] + matrix[2][2];
    
    { ===== Test 14: Final stress test - everything combined ===== }
    jika (allOperatorsTest() dan complexBoolean(10, 20, 30)) atau 
       (isPrime(result) dan inRange(result, MIN_INT, MAX_INT)) maka
    mulai
        untuk i := 1 ke 10 lakukan
        mulai
            untuk j := 1 ke 10 lakukan
            mulai
                jika (i = j) atau ((i + j) mod 2 = 0) maka
                mulai
                    matrix[i][j] := power(i, j) + megaExpression(i, j, i + j, i * j);
                selesai
                selain_itu
                mulai
                    matrix[i][j] := (i * 10.0 + j) bagi (i + j + 0.001);
                selesai;
            selesai;
        selesai;
        
        done := true;
    selesai
    selain_itu
    mulai
        done := false;
    selesai;
    
selesai.
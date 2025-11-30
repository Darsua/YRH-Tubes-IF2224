program ComprehensiveParserStressTest;

{ ========================================
  COMPREHENSIVE PARSER STRESS TEST
  Tests all grammar rules and edge cases
  ======================================== }

konstanta
    MAX_DEPTH = -100;
    MIN_VAL = 0;
    PI = 3.14159;
    E = 2.71828;
    TOLERANCE = 0.001;
    EMPTY_STR = '';
    GREETING = 'Hello, Parser!';

tipe
    ScoreRange = integer;
    Matrix = larik [1..10] dari larik [1..10] dari real;
    Vector = larik [0..99] dari integer;
    NestedArray = larik [1..5] dari larik [1..5] dari larik [1..5] dari integer;

variabel
    i, j, k, temp, result: integer;
    x, y, z, sum, avg: real;
    flag, done, valid, ready: boolean;
    ch, grade: char;
    scores: Vector;
    mat: Matrix;
    depth: NestedArray;

{ ========================================
  TEST 1: Nested Function Declarations
  ======================================== }

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

fungsi power(base: real; exp: integer): real;
variabel
    result: real;
    i: integer;
mulai
    result := 1.0;
    
    jika exp > 0 maka
    mulai
        untuk i := 1 ke exp lakukan
        mulai
            result := result * base;
        selesai;
    selesai
    selain_itu
    mulai
        jika exp < 0 maka
        mulai
            untuk i := 1 ke (0 - exp) lakukan
            mulai
                result := result * base;
            selesai;
            result := 1.0 bagi result;
        selesai;
    selesai;
    
    power := result;
selesai;

{ ========================================
  TEST 2: Complex Nested Expression
  ======================================== }

fungsi complexCalc(a, b, c: real): real;
variabel
    temp1, temp2, temp3: real;
mulai
    temp1 := ((a + b) * c) bagi (a - b + 1.0);
    temp2 := (a * a + b * b) bagi (c * c + 0.001);
    temp3 := ((temp1 + temp2) * (temp1 - temp2)) bagi (temp1 * temp2 + 1.0);
    
    jika (temp3 > 0.0) dan (temp3 < 100.0) maka
    mulai
        complexCalc := temp3;
    selesai
    selain_itu
    mulai
        jika temp3 < 0.0 maka
        mulai
            complexCalc := 0.0 - temp3;
        selesai
        selain_itu
        mulai
            complexCalc := 0.0;
        selesai;
    selesai;
selesai;

{ ========================================
  TEST 3: Array Manipulation
  ======================================== }

prosedur initMatrix;
variabel
    row, col: integer;
mulai
    untuk row := 1 ke 10 lakukan
    mulai
        untuk col := 1 ke 10 lakukan
        mulai
            mat[row][col] := 0.0;
        selesai;
    selesai;
selesai;

prosedur fillVector(start, step: integer);
variabel
    idx, val: integer;
mulai
    val := start;
    untuk idx := 0 ke 99 lakukan
    mulai
        scores[idx] := val;
        val := val + step;
    selesai;
selesai;

fungsi sumVector: real;
variabel
    idx: integer;
    total: real;
mulai
    total := 0.0;
    untuk idx := 0 ke 99 lakukan
    mulai
        total := total + scores[idx];
    selesai;
    sumVector := total;
selesai;

{ ========================================
  TEST 4: Deeply Nested Control Flow
  ======================================== }

prosedur nestedLoops;
variabel
    i, j, k, count: integer;
mulai
    count := 0;
    
    untuk i := 1 ke 5 lakukan
    mulai
        untuk j := 1 ke 5 lakukan
        mulai
            untuk k := 1 ke 5 lakukan
            mulai
                depth[i][j][k] := count;
                count := count + 1;
                
                jika (i = j) dan (j = k) maka
                mulai
                    depth[i][j][k] := count * 10;
                selesai;
            selesai;
        selesai;
    selesai;
selesai;

prosedur complexConditions(val: integer);
mulai
    jika val > 0 maka
    mulai
        jika val mod 2 = 0 maka
        mulai
            jika val < 100 maka
            mulai
                result := val * 2;
            selesai
            selain_itu
            mulai
                result := val bagi 2;
            selesai;
        selesai
        selain_itu
        mulai
            jika val mod 3 = 0 maka
            mulai
                result := val + 10;
            selesai
            selain_itu
            mulai
                result := val - 10;
            selesai;
        selesai;
    selesai
    selain_itu
    mulai
        jika val = 0 maka
        mulai
            result := 0;
        selesai
        selain_itu
        mulai
            result := 0 - val;
        selesai;
    selesai;
selesai;

{ ========================================
  TEST 5: Complex Boolean Expressions
  ======================================== }

fungsi checkRange(val, min, max: integer): boolean;
mulai
    checkRange := (val >= min) dan (val <= max);
selesai;

fungsi isValid(x, y: real): boolean;
variabel
    temp: boolean;
mulai
    temp := (x > 0.0) dan (y > 0.0);
    temp := temp dan (x < 100.0);
    temp := temp dan (y < 100.0);
    temp := temp dan tidak ((x = y));
    
    isValid := temp;
selesai;

{ ========================================
  TEST 6: Recursive-like Iteration
  ======================================== }

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

{ ========================================
  TEST 7: String and Character Operations
  ======================================== }

prosedur printGrade(score: real);
mulai
    jika score >= 90.0 maka
    mulai
        grade := 'A';
    selesai
    selain_itu
    mulai
        jika score >= 80.0 maka
        mulai
            grade := 'B';
        selesai
        selain_itu
        mulai
            jika score >= 70.0 maka
            mulai
                grade := 'C';
            selesai
            selain_itu
            mulai
                jika score >= 60.0 maka
                mulai
                    grade := 'D';
                selesai
                selain_itu
                mulai
                    grade := 'F';
                selesai;
            selesai;
        selesai;
    selesai;
selesai;

{ ========================================
  TEST 8: Mixed Loop Directions
  ======================================== }

prosedur reverseProcess;
variabel
    fwd, bwd: integer;
mulai
    { Forward loop }
    untuk fwd := 1 ke 50 lakukan
    mulai
        scores[fwd] := fwd * 2;
    selesai;
    
    { Backward loop }
    untuk bwd := 99 turun_ke 50 lakukan
    mulai
        scores[bwd] := (100 - bwd) * 2;
    selesai;
selesai;

{ ========================================
  MAIN PROGRAM - Stress Test All Features
  ======================================== }

mulai
    { Initialize variables with complex expressions }
    i := (5 + 10) * 2 - (3 bagi 2);
    j := factorial(5) + fibonacci(10);
    k := gcd(48, 18);
    
    x := PI * 2.0;
    y := E + PI;
    z := power(2.0, 10);
    
    flag := (i > j) atau (j > k);
    done := tidak flag;
    valid := (x > 0.0) dan (y > 0.0) dan (z > 0.0);
    
    { Test array initialization }
    initMatrix();
    fillVector(1, 2);
    
    { Test nested loops }
    nestedLoops();
    
    { Complex arithmetic with function calls }
    sum := 0.0;
    untuk i := 1 ke 10 lakukan
    mulai
        sum := sum + power(i, 2);
    selesai;
    
    avg := sum bagi 10.0;
    
    { Test deeply nested conditionals }
    jika (sum > 100.0) dan (avg > 10.0) maka
    mulai
        jika flag atau done maka
        mulai
            jika valid dan ready maka
            mulai
                result := 1;
            selesai
            selain_itu
            mulai
                result := 0;
            selesai;
        selesai
        selain_itu
        mulai
            result := -1;
        selesai;
    selesai;
    
    { Test while loops with complex conditions }
    i := 0;
    selama (i < 10) dan (sum < 1000.0) lakukan
    mulai
        sum := sum + power(2.0, i);
        i := i + 1;
    selesai;
    
    { Test mixed expressions }
    x := ((sum + avg) * PI) bagi (E + 1.0);
    y := complexCalc(x, sum, avg);
    z := (x + y) bagi (x - y + 0.001);
    
    { Test array access in expressions }
    temp := 0;
    untuk i := 0 ke 9 lakukan
    mulai
        temp := temp + scores[i] * scores[99 - i];
    selesai;
    
    { Test boolean expressions }
    flag := (temp > 0) dan (temp < MAX_DEPTH);
    done := (temp >= MIN_VAL) atau (temp <= 0);
    valid := tidak ((flag dan done) atau (temp = 0));
    
    { Test procedure calls with complex parameters }
    complexConditions(factorial(5) + fibonacci(8));
    printGrade(sumVector() bagi 100.0);
    
    { Test nested array access }
    untuk i := 1 ke 5 lakukan
    mulai
        untuk j := 1 ke 5 lakukan
        mulai
            mat[i][j] := depth[i][j][i] + depth[j][i][j];
        selesai;
    selesai;
    
    { Final complex expression }
    jika checkRange(50, 0, 100) maka
    mulai
        result := ((fibonacci(10) + factorial(5)) * gcd(48, 18)) bagi 2;
    selesai
    selain_itu
    mulai
        result := ((fibonacci(10) + factorial(5)) * gcd(48, 18)) bagi 1;
    selesai;
    
    { Test reverse loop }
    reverseProcess();
    
    { Test final condition }
    jika isValid(x, y) dan checkRange(result, MIN_VAL, MAX_DEPTH) maka
    mulai
        done := true;
    selesai
    selain_itu
    mulai
        done := false;
    selesai;
    
selesai.
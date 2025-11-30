program TestConstants;

{ Test Case: Constant declarations and usage in expressions }

konstanta
    MAX_SIZE = 100;
    MIN_VALUE = 0;
    PI = 3.14;
    GREETING = 'Hello';
    FLAG = true;
    LETTER = 'A';

variabel
    size: integer;
    value: real;
    flag: boolean;
    ch: char;

mulai
    { Use constants in assignments }
    size := MAX_SIZE;
    value := PI * 2;
    flag := FLAG;
    ch := LETTER;

    { Use constants in expressions }
    size := MAX_SIZE - MIN_VALUE;
    value := PI + 1.5;

    { Use constants in conditionals }
    jika size > MAX_SIZE maka
        size := MAX_SIZE;

    { Use constants in loops }
    untuk size := MIN_VALUE ke MAX_SIZE lakukan
        value := value + 1
selesai.

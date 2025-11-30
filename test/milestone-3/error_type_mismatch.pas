program TestTypeMismatch;

{ Test Case 2: Error - Type Mismatches in Binary Operations }

variabel
    a: integer;
    b: real;
    c: boolean;
    d: char;

mulai
    { Error: Cannot add integer and boolean }
    a := a + c;
    
    { Error: Cannot subtract integer and char }
    a := a - d;
    
    { Error: Cannot multiply boolean and real }
    b := c * b;
    
    { Error: Cannot compare boolean and integer }
    c := (c > a);
    
    { Error: AND requires boolean operands }
    a := a dan b;
    
    { Error: OR requires boolean operands }
    c := a atau d;
selesai.

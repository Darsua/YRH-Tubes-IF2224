program TestProcCallErrors;

{ Test Case: Procedure and Function Call Errors }

variabel
    x, y, z: integer;
    result: integer;

prosedur proc1(a: integer; b: integer);
mulai
    x := a + b
selesai;

prosedur proc2(variabel p: integer);
mulai
    p := p + 1
selesai;

fungsi func1(n: integer): integer;
mulai
    func1 := n * 2
selesai;

mulai
    { Correct calls }
    proc1(5, 10);
    proc2(x);
    result := func1(20);

    { Error: undefined procedure }
    undefinedProc(x);

    { Error: undefined function }
    result := undefinedFunc(10);

    { Error: calling variable as procedure }
    x(5);

    { Note: Argument count/type checking may not be implemented yet }
    { but these are test cases for future enhancement }
    proc1(5);          { Wrong number of arguments - too few }
    proc1(5, 10, 15);  { Wrong number of arguments - too many }
    func1();           { Missing required argument }
selesai.

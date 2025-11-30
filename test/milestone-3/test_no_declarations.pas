program TestNoDeclarations;

{ Test Case: Program with no variable/constant/type declarations }

prosedur simpleProc;
mulai
    { Empty procedure }
selesai;

fungsi simpleFunc(): integer;
mulai
    simpleFunc := 42
selesai;

mulai
    { Main body with no local variables }
    simpleProc()
selesai.

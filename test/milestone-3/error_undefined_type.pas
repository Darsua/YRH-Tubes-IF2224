program TestUndefinedType;

{ Test Case: Using undefined types }

tipe
    ValidType = integer;

variabel
    x: ValidType;        { OK - ValidType is defined }
    y: UndefinedType;    { Error: UndefinedType not declared }
    z: AnotherBadType;   { Error: AnotherBadType not declared }

prosedur testProc(a: ValidType; b: BadParamType);
{ Error: BadParamType not declared }
variabel
    local: YetAnotherBad;  { Error: YetAnotherBad not declared }
mulai
    x := 10
selesai;

fungsi testFunc(p: integer): MysteryType;
{ Error: MysteryType not declared as return type }
variabel
    temp: integer;
mulai
    temp := p * 2;
    testFunc := temp
selesai;

mulai
    x := 5;
    testProc(x, x)
selesai.

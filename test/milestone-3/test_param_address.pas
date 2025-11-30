program TestParamAddress;

{ Test Case: Parameter and Variable Address Calculation }

variabel
    global1: integer;
    global2: real;
    global3: char;

prosedur proc1(a: integer; b: real);
variabel
    local1: integer;
    local2: char;
mulai
    local1 := a;
    a := 5
selesai;

prosedur proc2(variabel x: integer; y: real; variabel z: char);
variabel
    temp1: integer;
    temp2: real;
mulai
    x := 10;
    temp1 := x;
    temp2 := y
selesai;

fungsi func1(p1: integer; p2: integer): integer;
variabel
    result: integer;
    temp: integer;
mulai
    result := p1 + p2;
    temp := result * 2;
    func1 := temp
selesai;

fungsi func2(variabel a: integer; b: real; variabel c: char): integer;
variabel
    x: integer;
    y: integer;
    z: integer;
mulai
    x := a;
    y := 20;
    z := x + y;
    func2 := z
selesai;

mulai
    global1 := 100;
    global2 := 3.14;
    global3 := 'A';
    proc1(global1, global2);
    proc2(global1, global2, global3);
    global1 := func1(5, 10);
    global1 := func2(global1, global2, global3)
selesai.

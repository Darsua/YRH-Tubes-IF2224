program TestVarParameters;

variabel
    x, y: integer;
    hasil: integer;

{ Test procedure with VAR parameter }
prosedur swap_values(variabel a, b: integer);
variabel
    temp: integer;
mulai
    temp := a;
    a := b;
    b := temp
selesai;

{ Test procedure with normal parameters }
prosedur add_one(n: integer);
mulai
    n := n + 1
selesai;

{ Test function with mixed parameters }
fungsi compute(variabel p: integer; q: integer): integer;
mulai
    p := p * 2;
    compute := p + q
selesai;

mulai
    x := 10;
    y := 20;
    swap_values(x, y);
    writeln('x=', x, ' y=', y);
    
    add_one(x);
    writeln('x after add_one=', x);
    
    hasil := compute(x, y);
    writeln('hasil=', hasil, ' x=', x)
selesai.

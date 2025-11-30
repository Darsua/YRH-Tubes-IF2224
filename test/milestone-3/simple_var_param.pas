program TestVarParam;

variabel
    a, b: integer;
    temp: integer;

prosedur tukar(variabel p, q: integer);
variabel
    tmpvar: integer;
mulai
    tmpvar := p;
    p := q;
    q := tmpvar
selesai;

prosedur increment(n: integer);
mulai
    n := n + 1
selesai;

mulai
    a := 5;
    b := 10;
    tukar(a, b);
    increment(a)
selesai.

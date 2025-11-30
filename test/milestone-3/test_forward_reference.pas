program TestForwardReference;

{ Test Case: Forward references and mutual recursion }

variabel
    result: integer;

prosedur procA(n: integer);
variabel
    temp: integer;
mulai
    jika n > 0 maka
        temp := n - 1
selesai;

prosedur procB(m: integer);
variabel
    val: integer;
mulai
    jika m > 0 maka
        val := m * 2
selesai;

fungsi factorial(n: integer): integer;
variabel
    temp: integer;
mulai
    jika n <= 1 maka
        factorial := 1
    selain_itu
        mulai
            temp := n - 1;
            factorial := n * factorial(temp)
        selesai
selesai;

fungsi fibonacci(n: integer): integer;
variabel
    a, b: integer;
mulai
    jika n <= 1 maka
        fibonacci := n
    selain_itu
        mulai
            a := n - 1;
            b := n - 2;
            fibonacci := fibonacci(a) + fibonacci(b)
        selesai
selesai;

mulai
    result := factorial(5);
    result := fibonacci(7);
    procA(10);
    procB(20)
selesai.

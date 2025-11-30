program TestControlFlow;

{ Test Case: Control flow statements - if, while, for }

variabel
    x, y, z: integer;
    flag: boolean;
    i, j: integer;
    sum, product: integer;

mulai
    { Simple if statement }
    x := 10;
    jika x > 5 maka
        y := 20;

    { If with else }
    jika x < 0 maka
        y := -1
    selain_itu
        y := 1;

    { Nested if statements }
    jika x > 0 maka
        jika y > 0 maka
            z := x + y
        selain_itu
            z := x - y
    selain_itu
        z := 0;

    { If with complex condition }
    jika (x > 5) dan (y < 100) maka
        mulai
            z := x * y;
            x := z
        selesai;

    { If with boolean variable }
    flag := true;
    jika flag maka
        x := 100;

    { If with comparison }
    jika x = y maka
        z := 0
    selain_itu
        jika x > y maka
            z := 1
        selain_itu
            z := -1;

    { While loop }
    i := 1;
    sum := 0;
    selama i <= 10 lakukan
        mulai
            sum := sum + i;
            i := i + 1
        selesai;

    { While with complex condition }
    i := 10;
    selama (i > 0) dan (sum < 1000) lakukan
        mulai
            sum := sum + i;
            i := i - 1
        selesai;

    { Nested while loops }
    i := 1;
    product := 1;
    selama i <= 3 lakukan
        mulai
            j := 1;
            selama j <= 3 lakukan
                mulai
                    product := product * 2;
                    j := j + 1
                selesai;
            i := i + 1
        selesai;

    { For loop ascending }
    sum := 0;
    untuk i := 1 ke 10 lakukan
        sum := sum + i;

    { For loop with compound body }
    sum := 0;
    untuk i := 1 ke 5 lakukan
        mulai
            sum := sum + i;
            product := i * i
        selesai;

    { For loop descending }
    sum := 100;
    untuk i := 10 turun_ke 1 lakukan
        sum := sum - i;

    { Nested for loops }
    sum := 0;
    untuk i := 1 ke 5 lakukan
        untuk j := 1 ke 5 lakukan
            sum := sum + (i * j);

    { For loop with array-like access }
    untuk i := 0 ke 9 lakukan
        mulai
            x := i * 10;
            y := x + 5
        selesai;

    { Complex control flow combination }
    untuk i := 1 ke 10 lakukan
        mulai
            jika i mod 2 = 0 maka
                mulai
                    sum := sum + i;
                    j := i;
                    selama j > 0 lakukan
                        mulai
                            product := product * j;
                            j := j - 1
                        selesai
                selesai
            selain_itu
                sum := sum - i
        selesai;

    { While with if inside }
    i := 1;
    sum := 0;
    selama i <= 20 lakukan
        mulai
            jika i <= 10 maka
                sum := sum + i
            selain_itu
                sum := sum - i;
            i := i + 1
        selesai;

    { If with for inside }
    jika x > 0 maka
        mulai
            sum := 0;
            untuk i := 1 ke x lakukan
                sum := sum + i
        selesai
    selain_itu
        sum := -1;

    { Multiple levels of nesting }
    untuk i := 1 ke 5 lakukan
        mulai
            jika i mod 2 = 0 maka
                mulai
                    j := 1;
                    selama j <= i lakukan
                        mulai
                            jika j = i maka
                                product := i * j
                            selain_itu
                                product := product + 1;
                            j := j + 1
                        selesai
                selesai
        selesai
selesai.

program TestPascal;

const
    maxsize := 100;

var
    x, y, result: integer;
    flag: boolean;

procedure calculate;
begin
    x := 10;
    y := 20;
    result := x + y * 2;
    if result > 30 then
        flag := true
    else
        flag := false
end;

begin
    { This is a Pascal-S comment }
    call calculate;

    (* Another comment style *)
    while x < y do
    begin
        x := x + 1;
        if odd(x) then
            result := result - 1
    end;

    { String and character examples }
    writeln('Hello Pascal-S World!');
    writeln('A');

    { Relational operators }
    if x = y then result := 0;
    if x <> y then result := 1;
    if x <= y then result := 2;
    if x >= y then result := 3;

    { Range operator example }
    for i := 1..10 do
        result := result + i
end.

program ComprehensiveTest;

const
    MAX_SIZE = 100;
    MIN_VALUE = 0;
    DEFAULT_CHAR = 'X';

var
    counter, total, result: integer;
    flag, done: boolean;
    current_char: char;
    message, name: string;

procedure initialize;
begin
    { Initialize variables with different literal types }
    counter := 0;
    total := 1000;
    flag := true;
    done := false;
    current_char := 'A';
    message := 'Starting initialization';
    name := 'Pascal-S Program'
end;

procedure calculate;
var
    temp: integer;
begin
    { Arithmetic operations }
    temp := counter + total;
    temp := temp - MIN_VALUE;
    temp := temp * 2;
    temp := temp / 4;

    { Assignment operations }
    result := temp;
    counter := counter + 1
end;

procedure testRelationalOperators;
begin
    { All relational operators }
    if counter = 10 then result := 1;
    if counter <> 10 then result := 2;
    if counter < MAX_SIZE then result := 3;
    if counter <= MAX_SIZE then result := 4;
    if counter > MIN_VALUE then result := 5;
    if counter >= MIN_VALUE then result := 6
end;

procedure testControlFlow;
begin
    { While loop with relational operators }
    while counter < 10 do
    begin
        counter := counter + 1;
        if odd(counter) then
            result := result + counter
        else
            result := result - 1
    end;

    { Conditional statements }
    if result > 50 then
    begin
        flag := true;
        message := 'Result is high'
    end
    else
    begin
        flag := false;
        message := 'Result is low'
    end
end;

procedure testCharactersAndStrings;
begin
    { Character literals - single characters }
    current_char := 'Z';
    current_char := '9';
    current_char := ' ';
    current_char := '\n';
    current_char := '\t';
    current_char := '\'';
    current_char := '\\';

    { String literals - multiple characters }
    message := 'Hello Pascal-S World!';
    message := 'String with numbers: 12345';
    message := 'String with special chars: @#$%^&*()';
    message := ''; { Empty string }
    message := 'Escaped quotes: \'Hello\' and \'World\'';
    message := 'Multi-line\nstring with\ttabs';

    { Mixed usage }
    if current_char = 'A' then
        message := 'Character is A'
    else if current_char = 'Z' then
        message := 'Character is Z'
    else
        message := 'Character is something else'
end;

procedure testRangeOperator;
var
    i: integer;
begin
    { Range operator in for loops }
    for i := 1..10 do
        total := total + i;

    for i := 0..MAX_SIZE do
        if i = counter then
            done := true
end;

begin
    (* Main program starts here *)
    { This is a brace-style comment }

    { Call all test procedures }
    call initialize;
    call calculate;
    call testRelationalOperators;
    call testControlFlow;
    call testCharactersAndStrings;
    call testRangeOperator;

    { Final operations }
    if done then
        message := 'All tests completed successfully'
    else
        message := 'Some tests may have failed';

    { Demonstrate all punctuation }
    result := (counter + total) * 2;

    { Program termination }
    done := true

end.

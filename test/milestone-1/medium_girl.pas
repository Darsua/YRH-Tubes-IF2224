program LiteralTest;

const
    PI = 3.14;
    GREETING = 'Hello, Lexer!';
    EMPTY = '';

var
    ch: char;
    str: string;
    num: integer;

begin
    ch := 'X';
    str := GREETING;
    num := 42;
    writeln('Char:', ch);
    writeln('String:', str);
    writeln('Number:', num);
    writeln('Empty string:', EMPTY);
    writeln('Pi:', PI)
end.
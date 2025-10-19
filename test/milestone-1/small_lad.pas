program ControlFlowTest;

var
    i, sum: integer;
    status: boolean;

begin
    sum := 0;
    status := false;
    for i := 1 to 5 do
    begin
        sum := sum + i;
        if sum mod 2 = 0 then
            status := true
        else
            status := false
    end;
    while sum > 0 do
    begin
        sum := sum - 1
    end;
    writeln('Final sum:', sum);
    writeln('Status:', status)
end.
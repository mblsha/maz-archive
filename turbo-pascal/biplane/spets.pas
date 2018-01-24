program spets;
uses crt;

type    t = 0 .. 99;

var     f : file of t;
        tt : array[1 .. 2] of t;
        i : integer;

begin
     clrscr;
     assign (f, 'text.txt');
     rewrite (f);

     for i := 1 to 10 do begin
         tt[1] := random (100);
         write (f, tt[1]);
     end;

     close (f);
     reset (f);

     tt[1] := 0;
     for i := 1 to 10 do begin
         read (f, tt[2]);
         if (tt[2] < tt[1]) then begin
            writeln ('Faiil ne idet po vozrastayuscheii!!! -- ', tt[1], ' > ', tt[2]);
            break;
         end;

         if i > 1 then
            writeln (tt[1], ' < ', tt[2]);

         tt[1] := tt[2];
     end;

     if tt[2] = tt[1] then
        writeln ('Vse elementi faiila idut po vozrastayuscheii.');
end.
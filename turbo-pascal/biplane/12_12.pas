program prog_12_12;

{
   ЗАМЕЧАНИЕ: Для достижения оптимального результата следует
              изменить значения параметров функции delay();
              (в меньшую сторону для более быстрой анимации,
              в большую сторону, для более медленной)
}

uses graph, crt;

procedure load_sprite (filename : string; var ptr : pointer);
var x, y   : integer;
    sx, sy : integer;
    c      : integer;
    s      : string;
    fp     : text;
    size   : word;
begin
     SetVisualPage (0);
     SetActivePage (1);

     assign (fp, filename);
     reset (fp);

     readln (fp, s);
     read (fp, sx, sy);

     ClearDevice;

     for y := 0 to sy do begin
         for x := 0 to sx do begin
             read (fp, c);
             putpixel (x, y, c);
         end;
     end;

     size := imagesize (0, 0, sx, sy);
     getmem (ptr, size);
     getimage (0, 0, sx, sy, ptr^);

     close (fp);
end;


procedure do_barrel;
var p : array[1 .. 24] of pointer;
    x, i, frame, page : integer;
    counter : integer;
    s, s2 : string;
begin
     frame := 1;
     counter := 0;

     for i := 1 to 24 do begin
         s := 'pics\barrel\plane_';
         str (i, s2);
         s := concat (s, s2);
         s := concat (s, '.spr');

         load_sprite (s, p[i]);
     end;

     for x := 0 to 580 do begin
         ClearDevice;

         PutImage(x, 150, P[frame]^, xorPut);

         inc (counter);

         if counter > 10 then begin
            counter := 0;
            inc (frame);

            if frame > 24 then
               frame := 1;
         end;

         SetVisualPage (page);
         if page = 0 then
            page := 1
         else
            page := 0;
         SetActivePage (page);

         delay (250);
     end;

     for i := 1 to 24 do
         freemem (p[i], sizeof(p[i]^));
end;


procedure do_loop;
var p : array[1 .. 16] of pointer;
    x, y, i, frame, page : integer;
    counter : integer;
    s, s2 : string;
    ang, ang2 : real;
    r, init_y : integer;
begin
     frame := 1;
     counter := 0;

     init_y := 300;
     r := 25;

     for i := 1 to 16 do begin
         s := 'pics\loop\plane_';
         str (i, s2);
         s := concat (s, s2);
         s := concat (s, '.spr');

         load_sprite (s, p[i]);
     end;

     { сначала летим до центра... }
     for x := 0 to 320 do begin
         SetVisualPage (page);
         if page = 0 then
            page := 1
         else
            page := 0;
         SetActivePage (page);
         ClearDevice;

         PutImage(x-40, init_y-25, P[1]^, xorPut);

         delay (100);
     end;

     { типерь делаем круг... }
     for i := 1 to 3 do begin
        ang := 0;

        while ang <= 360 do begin
              ClearDevice;

              ang2 := (ang-90) * 0.017;
              x := round( cos(ang2)*r + 320);

              ang2 := (90-ang) * 0.017;
              y := round( (sin(ang2)-1)*r + init_y);

              case round(ang) of
                   001 .. 022: frame := 1;
                   022 .. 045: frame := 2;
                   045 .. 067: frame := 3;
                   067 .. 090: frame := 4;
                   090 .. 112: frame := 5;
                   112 .. 135: frame := 6;
                   135 .. 157: frame := 7;
                   157 .. 180: frame := 8;
                   180 .. 202: frame := 9;
                   202 .. 225: frame := 10;
                   225 .. 247: frame := 11;
                   247 .. 270: frame := 12;
                   270 .. 292: frame := 13;
                   292 .. 315: frame := 14;
                   315 .. 337: frame := 15;
                   337 .. 360: frame := 16;
              end;

              PutImage(x-40, y-25, P[frame]^, xorPut);

              ang := ang + (150 / r);

              SetVisualPage (page);
              if page = 0 then
                 page := 1
              else
                 page := 0;
              SetActivePage (page);

              delay (250);
        end;

        r := r + 50;
     end;

     { все, летим до правого края... }
     for x := 320 to 640 do begin
         SetVisualPage (page);
         if page = 0 then
            page := 1
         else
            page := 0;
         SetActivePage (page);
         ClearDevice;

         PutImage(x-40, init_y-25, P[1]^, xorPut);

         delay (75);
     end;

     for i := 1 to 16 do
         freemem (p[i], sizeof(p[i]^));
end;


procedure do_corkscrew;
var p : array[1 .. 24] of pointer;
    x, y, i, frame, page : integer;
    counter : integer;
    s, s2 : string;
    r : integer;
begin
     frame := 1;
     counter := 0;

     for i := 1 to 24 do begin
         s := 'pics\corksc~1\plane_';
         str (i, s2);
         s := concat (s, s2);
         s := concat (s, '.spr');

         load_sprite (s, p[i]);
     end;

     r := 750;

     for y := 0 to 300 do begin
         ClearDevice;

         x := round (sin(y/50)*r/2) + 320;
         r := r - 3;

         PutImage(x-40, y-25, P[frame]^, xorPut);

         inc (counter);

         if counter > 10 then begin
            counter := 0;
            inc (frame);

            if frame > 24 then
               frame := 1;
         end;

         SetVisualPage (page);
         if page = 0 then
            page := 1
         else
            page := 0;
         SetActivePage (page);

         delay (250);
     end;

     for i := 1 to 24 do
         freemem (p[i], sizeof(p[i]^));
end;

var
    Gd, Gm: Integer;
    P: array[1 .. 24] of Pointer;
    Size: Word;
    an : integer;
    i : integer;
    k : integer;

begin
  Gd := EGA;
  Gm := EGAHi;
  InitGraph(Gd, Gm, '');

  if GraphResult <> grOk then
    Halt(1);

  do_barrel;
  do_loop;
  do_corkscrew;

  CloseGraph;
end.

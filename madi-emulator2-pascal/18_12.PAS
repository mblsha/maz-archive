program prog_18_12;

uses
   Crt, Graph;

const
   pause = 1000;
   cool_effect = 0;

   madi_speed = 15;

   sun_speed     = 20;
   sun_radius    = 170;
   sun_rot_speed = 2;

   num_mans   = 10;
   man_speed  = 10;

   adv_speed  = 10;
   adv_delay  = 5;

var

   SCREEN_W, SCREEN_H : integer;
   Aspect : real;

type

   Node = record
      x, y : integer;
   end;

   Connection = record
      n   : array[1 .. 2] of integer;
      col : byte;
   end;

   Poly = record
      n   : array[1 .. 3] of integer;
      col : byte;
   end;

   Nodes       = array[1 .. 1] of Node;
   Connections = array[1 .. 1] of Connection;
   Polys       = array[1 .. 1] of Poly;

   NodesP       = ^Nodes;
   ConnectionsP = ^Connections;
   PolysP       = ^Polys;

procedure load_nodes (var n    : NodesP;
                      var size : integer;
                      filename : string);
var
   fp : text;
   i  : integer;

begin
   assign (fp, filename);
   reset (fp);

   readln (fp);

   readln (fp, size);
   getmem (n, sizeof(Node) * size);

   for i := 1 to size do with n^[i] do
      readln (fp, x, y);

   close (fp);
end;

procedure load_connections (var c    : ConnectionsP;
                            var size : integer;
                            filename : string);
var
   fp : text;
   t  : integer;
   i  : integer;

begin
   assign (fp, filename);
   reset (fp);

   readln (fp);
   readln (fp, t);

   for i := 1 to t do
      readln (fp);

   readln (fp, size);
   getmem (c, sizeof(Connection) * size);

   for i := 1 to size do with c^[i] do
      readln (fp, n[1], n[2], col);

   close (fp);
end;

procedure load_polys (var p    : PolysP;
                      var size : integer;
                      filename : string);
var
   fp : text;
   t  : integer;
   i  : integer;

begin
   assign (fp, filename);
   reset (fp);

   readln (fp);
   readln (fp, t);

   for i := 1 to t do
      readln (fp);

   readln (fp, t);

   for i := 1 to t do
      readln (fp, t);

   readln (fp, size);
   getmem (p, sizeof(Poly) * size);

   for i := 1 to size do with p^[i] do begin
      readln (fp, n[1], n[2], n[3], col);

      if cool_effect = 1 then
         col := random(16);
   end;

   close (fp);
end;


procedure normalize_nodes (var n    : NodesP;
                           size     : integer;
                           var w, h : integer;
                           zx, zy   : real);
var
   i : integer;
   up, down, left, right : real;
   Xasp, Yasp : word;

begin
   up    := 1000;
   down  := -1000;
   right := -1000;
   left  := 1000;

   for i := 1 to size do with n^[i] do begin
      if x < left  then left  := x;
      if x > right then right := x;
      if y > down  then down  := y;
      if y < up    then up    := y;
   end;

   GetAspectRatio (Xasp, Yasp);
   Aspect := Xasp / Yasp;

   if zy = 0 then
      zy := zx * Aspect;

   for i := 1 to size do with n^[i] do begin
      x := x - round((right + left)/2);
      y := y - round((down  + up)/2);

      x := round(x * zx);
      y := round(y * zy);
   end;

   w := round((right - left)/2);
   h := round((down  - up)/2);
end;

procedure draw_connections (var from_conn : ConnectionsP;
                            var to_conn   : ConnectionsP;
                            conn_size     : integer;
                            conn_size2    : integer;
                            var from_node : NodesP;
                            var to_node   : NodesP;
                            node_size     : integer;
                            frame         : integer;
                            ox, oy        : integer);
var
   i, j, max : integer;
   n : integer;
   Point : array[1 .. 2] of PointType;

begin
   if frame > 50 then
      max := conn_size2
   else
      max := conn_size;

   for i := 1 to max do begin
      for j := 1 to 2 do begin
         if frame > 50 then
            n := to_conn^[i].n[j]
         else
            n := from_conn^[i].n[j];

         Point[j].x := round((to_node^[n].x - from_node^[n].x)/100*frame) + from_node^[n].x + ox;
         Point[j].y := round((to_node^[n].y - from_node^[n].y)/100*frame) + from_node^[n].y + oy;
      end;


      if frame > 50 then
         setcolor (to_conn^[i].col)
      else
         setcolor (from_conn^[i].col);

      line (Point[1].x, Point[1].y, Point[2].x, Point[2].y);
   end;
end;

procedure draw_polys (var from_poly : PolysP;
                      var to_poly   : PolysP;
                      poly_size     : integer;
                      var from_node : NodesP;
                      var to_node   : NodesP;
                      node_size     : integer;
                      frame         : integer;
                      ox, oy        : integer);
var
   i, j : integer;
   n, col : integer;
   Triangle : array[1 .. 3] of PointType;

begin
   for i := 1 to poly_size do begin
      for j := 1 to 3 do begin
         if frame > 50 then
            n := to_poly^[i].n[j]
         else
            n := from_poly^[i].n[j];

         Triangle[j].x := round((to_node^[n].x - from_node^[n].x)/100*frame) + from_node^[n].x + ox;
         Triangle[j].y := round((to_node^[n].y - from_node^[n].y)/100*frame) + from_node^[n].y + oy;
      end;

      if frame > 50 then
         col := to_poly^[i].col
      else
         col := from_poly^[i].col;

      SetFillStyle (1, col);
      SetColor (col);

      FillPoly (3, Triangle);
   end;
end;


type

   MADI_TYPE = record
      n : array[1 .. 2] of NodesP;
      n_size : integer;

      c : ConnectionsP;
      c_size : integer;

      p : PolysP;
      p_size : integer;

      w, h : integer;

      frame       : integer;
      frame_speed : integer;
   end;

var

   madi : MADI_TYPE;

procedure init_madi;
var
   i : integer;

begin
   load_nodes       (madi.n[1], madi.n_size, 'gfx\madi1.vec');
   load_nodes       (madi.n[2], madi.n_size, 'gfx\madi2.vec');
   load_connections (madi.c,    madi.c_size, 'gfx\madi1.vec');
   load_polys       (madi.p,    madi.p_size, 'gfx\madi1.vec');

   for i := 1 to 2 do
      normalize_nodes (madi.n[i], madi.n_size, madi.w, madi.h, 0.65, 0);

   madi.frame := 0;
   madi.frame_speed := madi_speed;
end;

procedure update_madi;
begin
   madi.frame := madi.frame + madi.frame_speed;

   if (madi.frame > 100) or (madi.frame < 0) then
      madi.frame_speed := madi.frame_speed * -1;
end;

procedure draw_madi;
begin
   draw_polys (madi.p, madi.p, madi.p_size,
               madi.n[1], madi.n[2], madi.n_size,
               madi.frame, round(SCREEN_W/2), round(SCREEN_H-madi.h/2));
   draw_connections (madi.c, madi.c, madi.c_size, madi.c_size,
                     madi.n[1], madi.n[2], madi.n_size,
                     madi.frame, round(SCREEN_W/2), round(SCREEN_H-madi.h/2));
end;

type

   SUN_TYPE = record
      n : array[1 .. 3] of NodesP;
      n_size : integer;

      p : PolysP;
      p_size : integer;

      ang  : integer;
      x, y : real;
      w, h : integer;

      frame       : integer;
      frame_speed : integer;
      anim_frame  : integer;
   end;

var

   sun : SUN_TYPE;

procedure init_sun;
var
   i : integer;

begin
   load_nodes       (sun.n[1], sun.n_size, 'gfx\sun1.vec');
   load_nodes       (sun.n[2], sun.n_size, 'gfx\sun2.vec');
   load_nodes       (sun.n[3], sun.n_size, 'gfx\sun3.vec');
   load_polys       (sun.p,    sun.p_size, 'gfx\sun1.vec');

   for i := 1 to 3 do
      normalize_nodes (sun.n[i], sun.n_size, sun.w, sun.h, 0.4, 0);

   sun.ang := 0;

   sun.x := SCREEN_W / 2;
   sun.y := SCREEN_H / 2 - 100;

   sun.frame := 0;
   sun.frame_speed := sun_speed;
   sun.anim_frame := 1;
end;

procedure update_sun;
var
   angle : real;

begin
   sun.frame := sun.frame + sun.frame_speed;

   if sun.frame > (100 - sun.frame_speed) then begin
      sun.frame := 0;
      sun.anim_frame := sun.anim_frame + 1;

      if sun.anim_frame > 3 then
         sun.anim_frame := 1;
   end;

   angle := sun.ang * pi / 180;
   sun.x := SCREEN_W/2 + cos(angle)*sun_radius;
   sun.y := SCREEN_H/2 - sin(angle)*sun_radius*Aspect;

   sun.ang := sun.ang + sun_rot_speed;
   if sun.ang > 180 then
      sun.ang := 0;

end;

procedure draw_sun;
begin
   case sun.anim_frame of
      1: draw_polys (sun.p, sun.p, sun.p_size,
                     sun.n[1], sun.n[2], sun.n_size,
                     sun.frame, round(sun.x), round(sun.y));
      2: draw_polys (sun.p, sun.p, sun.p_size,
                     sun.n[2], sun.n[3], sun.n_size,
                     sun.frame, round(sun.x), round(sun.y));
      3: draw_polys (sun.p, sun.p, sun.p_size,
                     sun.n[3], sun.n[1], sun.n_size,
                     sun.frame, round(sun.x), round(sun.y));
   end;
end;

type

   MAN_MODEL_TYPE = record
      n : array[1 .. 6] of NodesP;
      n_size : integer;

      c : ConnectionsP;
      c_size : integer;

      p : PolysP;
      p_size : integer;

      w, h : integer;
   end;

   MAN_TYPE = record
      x, y : integer;

      frame : integer;
      frame_speed : integer;
      anim_frame : integer;

      direction : integer;

      move_speed : integer;
   end;

   MANS_TYPE = array[1 .. 1] of MAN_TYPE;

var

   man_model : MAN_MODEL_TYPE;
   mans      : ^MANS_TYPE;

procedure init_man (var m : MAN_TYPE);
begin
   case random (2) of
      0: m.direction := -1;
      1: m.direction := 1;
   end;

   m.frame := 0;
   m.anim_frame := 1;

   m.move_speed := round( (random (150) + 10)/10 );
   m.frame_speed := m.move_speed * 3;

   if m.direction > 0 then
      m.x := 0
   else
      m.x := SCREEN_W;

   m.y := round( SCREEN_H - man_model.h/5 );
end;

procedure move_man (var m : MAN_TYPE);
begin
   m.x := m.x + m.direction * m.move_speed;

   m.frame := m.frame + m.frame_speed;

   if m.frame > 100 then begin
      m.frame := 0;

      m.anim_frame := m.anim_frame + 1;
      if m.anim_frame > 3 then
         m.anim_frame := 1;
   end;

   if ((m.direction > 0) and (m.x > (SCREEN_W+man_model.w))) or
      ((m.direction < 0) and (m.x < (-man_model.w))) then
      init_man (m);
end;

procedure draw_man (var m : MAN_TYPE);
var
   o : integer;
   f, t : integer;

begin
   if m.direction > 0 then
      o := 3
   else
      o := 0;

   case m.anim_frame of
      1: begin
            f := 1;
            t := 2;
         end;
      2: begin
            f := 2;
            t := 3;
         end;
      3: begin
            f := 3;
            t := 1;
         end;
   end;

   draw_polys (man_model.p, man_model.p, man_model.p_size,
               man_model.n[f+o], man_model.n[t+o], man_model.n_size,
               m.frame, m.x, m.y);
   draw_connections (man_model.c, man_model.c, man_model.c_size, man_model.c_size,
               man_model.n[f+o], man_model.n[t+o], man_model.n_size,
               m.frame, m.x, m.y);
end;

procedure init_mans;
var
   i, j : integer;

begin
   load_nodes       (man_model.n[1], man_model.n_size, 'gfx\human1.vec');
   load_nodes       (man_model.n[2], man_model.n_size, 'gfx\human2.vec');
   load_nodes       (man_model.n[3], man_model.n_size, 'gfx\human3.vec');
   load_connections (man_model.c,    man_model.c_size, 'gfx\human1.vec');
   load_polys       (man_model.p,    man_model.p_size, 'gfx\human1.vec');

   for i := 1 to 3 do
      normalize_nodes (man_model.n[i], man_model.n_size,
                       man_model.w, man_model.h, 0.25, 0);

   for i := 1 to 3 do begin
      getmem (man_model.n[i+3], sizeof(Node) * man_model.n_size);
      for j := 1 to man_model.n_size do begin
         man_model.n[i+3]^[j].x := -1 * man_model.n[i]^[j].x;
         man_model.n[i+3]^[j].y :=  man_model.n[i]^[j].y;
      end;
   end;

   getmem (mans, sizeof(MAN_TYPE) * num_mans);
   for i := 1 to num_mans do begin
      init_man (mans^[i]);
   end;
end;

procedure update_mans;
var
   i : integer;

begin
   for i := 1 to num_mans do
      move_man (mans^[i]);
end;

procedure draw_mans;
var
   i : integer;

begin
   for i := 1 to num_mans do
      draw_man (mans^[i]);
end;

type

   Symbol = record
      n : NodesP;
      n_size : integer;

      c : ConnectionsP;
      c_size : integer;

      w, h : integer;
   end;

var

   charset : array[1 .. 255] of Symbol;

procedure load_symbol (var s : Symbol;
                       filename : string;
                       zoom : real);
begin
   load_nodes       (s.n, s.n_size, filename);
   load_connections (s.c, s.c_size, filename);
   normalize_nodes  (s.n, s.n_size, s.w, s.h, zoom, 0);
end;

procedure load_charset;
var
   i : integer;
   s : string;

begin
   { ãááª¨¥ ¡ãª®¢ª¨ }
   for i := 1 to 32 do begin
      str (i, s);
      load_symbol (charset[i+127], 'gfx\font\ru_'+s+'.vec', 0.5);
   end;

   { à®¡¥« }
   load_symbol (charset[32], 'gfx\font\space.vec', 0.5);
end;

procedure draw_string (fs, ts : string;
                       frame : integer;
                       ox, oy : integer);
var
   i : integer;
   c : array[1 .. 2] of integer;
   x, y : integer;

begin
   x := ox;
   y := oy;

   for i := 1 to Length (fs) do begin
      c[1] := Ord(fs[i]);
      c[2] := Ord(ts[i]);

      draw_connections (charset[c[1]].c, charset[c[2]].c, charset[c[1]].c_size, charset[c[2]].c_size,
                        charset[c[1]].n, charset[c[2]].n, charset[c[1]].n_size,
                        frame, x, y);

      x := x + 15;
   end;
end;

const

   NumAdv = 7;

type

   AdvType = array[1 .. NumAdv] of string;

const

   Adv : AdvType =
   (
      '                                         ',
      '      …ƒˆ ‘Š…‰ ‚ Œ€„ˆ €ˆ’“ˆ…’      ',
      '  ‡„…‘œ ’› ‹“—ˆ˜œ ‘’“„…—…‘Šˆ‰ ˆ‹…’   ',
      '        “—ˆ’œ‘Ÿ “„…˜œ € ’‹ˆ—        ',
      '   ˆ Š€‘›‰ „ˆ‹Œ ‹“—ˆ˜œ ’› ‹ˆ—    ',
      '                                         ',
      '          ‹€ƒ„€ˆŒ ‡€ ‚ˆŒ€ˆ…         '
   );

var

   AdvFrame : integer;
   AdvAnim  : integer;
   AdvDelay : integer;

procedure init_text;
begin
   AdvFrame := 0;
   AdvAnim  := 1;
end;

procedure update_text;
begin
   If AdvDelay <= 0 then
      AdvFrame := AdvFrame + adv_speed;

   if AdvDelay > 0 then
      AdvDelay := AdvDelay - 1;

   if AdvFrame > 100 then begin
      AdvFrame := 0;
      AdvDelay := adv_delay;

      AdvAnim := AdvAnim + 1;
      if AdvAnim > NumAdv then
         AdvAnim := 1;
   end;
end;

procedure draw_text;
var n : array[1 .. 2] of integer;

begin
   n[1] := AdvAnim;

   if AdvAnim = NumAdv then
      n[2] := 1
   else
      n[2] := n[1] + 1;

   draw_string (Adv[n[1]], Adv[n[2]], AdvFrame, 15,15);
end;

procedure init;
begin
   init_madi;
   init_sun;
   init_mans;

   load_charset;
   init_text;
end;

procedure update;
begin
   update_madi;
   update_sun;
   update_mans;

   update_text;
end;

procedure draw;
begin
   draw_sun;
   draw_madi;
   draw_mans;

   draw_text;
end;

var
   driver, mode : integer;
   page : integer;

begin
   driver := EGA;
   mode   := EGAhi;
   InitGraph (driver, mode, '');

   if GraphResult <> grOK then
      Halt (1);

   SCREEN_W := 640;
   SCREEN_H := 350;

   OutTextXY (round((SCREEN_W-16*8)/2), round((SCREEN_H-8)/2), 'Wait a second...');
   init;

   while not keypressed do begin
      ClearDevice;

      update;
      draw;

      SetVisualPage (page);
      if page = 0 then
         page := 1
      else
         page := 0;
      SetActivePage (page);

      delay (pause);
   end;

   CloseGraph;
end.
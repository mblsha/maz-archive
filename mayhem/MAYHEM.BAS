'----------------------------------------------------------------------------'
'                                                                            '
'                   €€   €  €€€€€ €€   € €€   € €€€€€ €€   €                 '
'                   €€€ €€ €€   €  €€ €  €€   € €€    €€€ €€                 '
'                   €€ € € €€€€€€   €€   €€€€€€ €€€€  €€ € €                 '
'                   €€   € €€   €   €€   €€   € €€    €€   €                 '
'                   €€   € €€   €   €€   €€   € €€€€€ €€   €                 '
'                                                                            '
'----------------------------------------------------------------------------''
' MAYHEM 1.5 - Alex Smith 1998                                               '
'                                                                            '
' MAYHEM Improved(1.5+ and higher...) - Mike Pichagin 1999-2001 (MAZsoft)    '
'                                                                            '
'----------------------------------------------------------------------------'
'$DYNAMIC
'$INCLUDE: 'Mayhem.bi'

'Initilization proccess
Init

Player(1).AI = AILOCAL
Player(2).AI = AILOCAL
Player(3).AI = AILOCAL
Player(4).AI = AILOCAL
         
DO
  'Load preferences
  LoadCFG

  'DrawMainMenu
  Intro

  REDIM Laser(LasersAllowed, Players) AS LaserType
  'REDIM Ship(Players) AS PlyrShip
  REDIM Debris(MaxDebris) AS DebrisType
  REDIM Dot(Bits, Players) AS Fragments

  'Now we play
  PlayGame
LOOP

FUNCTION AIFaceTarget (p AS INTEGER)

  IF Player(p).Target < 1 OR Ship(p).Exploded THEN
    AIFaceTarget = 0
    EXIT FUNCTION
  END IF

  t = Player(p).Target
  a = Ship(p).Angle

  dx = Ship(t).PosX - Ship(p).PosX
  dy = Ship(t).PosY - Ship(p).PosY

  IF dx THEN
    ang = CINT(ATN(dy / dx) * 57.29579)
  ELSEIF dy >= 0 THEN
    ang = 90
  ELSEIF dy < 0 THEN
    ang = -90
  END IF

  IF dx < 0 AND dy > 0 THEN
    ang = 180 + ang
  ELSEIF dx < 0 AND dy < 0 THEN
    ang = -180 + ang
  ELSEIF dx < 0 AND dy = 0 THEN
    ang = 180
  END IF

  IF ang < 0 THEN ta = 360 + ang ELSE ta = ang

  a = 90 - a
  IF a < 0 THEN a = 360 + a

  IF a > ta THEN
    LEFT = 360 - (a - ta)
    RIGHT = a - ta
  ELSE
    LEFT = ta - a
    RIGHT = 360 - (ta - a)
  END IF

  IF LEFT < RIGHT THEN
    diff = LEFT
  ELSE
    diff = RIGHT
  END IF

  IF diff >= 2 * TurnAngle THEN
    IF (LEFT < RIGHT) THEN
      AIFaceTarget = 1
    ELSE
      AIFaceTarget = -1
    END IF
  ELSE
    'LOCATE 1, 1: PRINT "AIFaceTarget!!!"
    AIFaceTarget = 0
  END IF


  '
  ' Some Super-Secret cool effect!
  '

  ' Hunter-Target Line
  'LINE (Ship(p).PosX + 13, Ship(p).PosY + 13)-(Ship(t).PosX + 13, Ship(t).PosY + 13), Ship(p).Col'14

  ' Hunter's current Anti-Facing
  'LINE (Ship(p).PosX + 13, Ship(p).PosY + 13)-(Ship(p).PosX + 13 + 15 * COS(ta * (3.14 / 180)), Ship(p).PosY + 13 + 15 * SIN(ta * (3.14 / 180))), 14'Ship(p).Col'14

  ' Hunter's wanted direction
  'LINE (Ship(p).PosX + 13, Ship(p).PosY + 13)-(Ship(p).PosX + 13 + 15 * COS(a * (3.14 / 180)), Ship(p).PosY + 13 + 15 * SIN(a * (3.14 / 180))), 15'Ship(p).Col'14

  ' delay
  'WAIT &H3DA, 8

  ' Delete, what we just draw
  'LINE (Ship(p).PosX + 13, Ship(p).PosY + 13)-(Ship(t).PosX + 13, Ship(t).PosY + 13), 0
  'LINE (Ship(p).PosX + 13, Ship(p).PosY + 13)-(Ship(p).PosX + 13 + 15 * COS(ta * (3.14 / 180)), Ship(p).PosY + 13 + 15 * SIN(ta * (3.14 / 180))), 0
  'LINE (Ship(p).PosX + 13, Ship(p).PosY + 13)-(Ship(p).PosX + 13 + 15 * COS(a * (3.14 / 180)), Ship(p).PosY + 13 + 15 * SIN(a * (3.14 / 180))), 0

END FUNCTION

SUB AIFindMission (p AS INTEGER)

  CONST MISSIONNONE = 0

  flag = MISSIONNONE
  DIM score(Players)
  DIM dist AS SINGLE
  t = -1

  FOR i = 0 TO Players
    IF (i <> p) AND Player(i).Lives >= 0 AND (Player(i).Dead <> i) AND (Player(i).Dmg < 100) THEN
      dx = Ship(i).PosX - Ship(p).PosX
      dy = Ship(i).PosY - Ship(p).PosY
      dist = dx ^ 2 + dy ^ 2

      score(i) = 0

      IF Ship(i).Fired <= 3 THEN score(i) = score(i) + 5
      IF Ship(i).Fired = 0 THEN score(i) = score(i) + 20
      IF Ship(i).Fired >= 15 THEN score(i) = score(i) - 5
      IF Player(i).ShotsLft <= 5 THEN score(i) = score(i) + 50
      IF dist < 3000 THEN score(i) = score(i) + 100
      IF dist < 10000 THEN score(i) = score(i) + 80
      IF dist < 20000 THEN score(i) = score(i) + 75
      IF dist < 30000 THEN score(i) = score(i) + 70
      IF dist < 40000 THEN score(i) = score(i) + 65
      IF dist < 50000 THEN score(i) = score(i) + 60
      IF dist < 60000 THEN score(i) = score(i) + 55
      IF dist < 70000 THEN score(i) = score(i) + 50
      IF dist < 80000 THEN score(i) = score(i) + 45
      IF dist < 90000 THEN score(i) = score(i) + 40
      IF dist < 1000000 THEN score(i) = score(i) + 35
      IF dist < 1500000 THEN score(i) = score(i) + 30
      IF dist < 2500000 THEN score(i) = score(i) + 25
      'IF dist < 3000000 THEN score(i) = score(i) + 20
      'IF dist < 3500000 THEN score(i) = score(i) + 15
      IF dist < 4000000 THEN score(i) = score(i) + 20
      IF dist < 7500000 THEN score(i) = score(i) + 15
      IF dist < 15000000 THEN score(i) = score(i) + 10

      'IF dist < 20000 THEN score(i) = score(i) + 25
      'IF dist < 40000 THEN score(i) = score(i) + 15
      'IF dist < 45000 THEN score(i) = score(i) + 10
      'IF dist < 70000 THEN score(i) = score(i) + 5
      'IF dist < 150000 THEN score(i) = score(i) + 1

      'IF dist > 45000 THEN score(i) = score(i) - 50

      IF t < 0 THEN
        t = i
      ELSEIF score(t) < score(i) THEN
        t = i
      END IF
    ELSE
      score(i) = -10000
    END IF
  NEXT

  IF t > 0 THEN
    Player(p).Target = t
  ELSE
    FOR i = 1 TO Players
      IF (p <> i) AND (Player(i).Lives >= 0) AND (Player(i).Dead <> i) AND (Player(i).Dmg < 100) THEN
        Player(p).Target = i
      END IF
    NEXT
  END IF

END SUB

SUB CentrePRINT (text$, Col AS INTEGER, BCol AS INTEGER, x AS INTEGER, y AS INTEGER)
DEFINT A-Z

  'yy = 1
  IF x THEN
    xx = CINT(x + .1 - (LEN(text$) / 2))
  ELSE
    xx = (82 - LEN(text$)) / 2
  END IF
  IF xx <= 0 THEN xx = 1
 
  IF y THEN
    LOCATE y, xx
  ELSE
    LOCATE , xx
  END IF

  COLOR Col, BCol

  PRINT text$

END SUB

DEFSNG A-Z
SUB CheckWalls (p AS INTEGER)
DEFINT A-Z
   
IF Ship(p).Refresh THEN EXIT SUB

xxx = Ship(p).PosX + 13
yyy = Ship(p).PosY + 13

IF xxx < 0 OR xxx > 640 OR yyy < 0 OR yyy > 350 THEN
  Player(p).Crash = Player(p).Crash + 1
  Player(p).Dead = p: InitExplosion p
END IF

DIM xx AS SINGLE, yy AS SINGLE, a AS SINGLE

xx = Ship(p).Addx / 2
yy = Ship(p).AddY / 2

IF Ship(p).Landed = 0 AND Ship(p).Angle = 0 THEN Depth = 3 ELSE Depth = 2
FOR x = 1 TO Points(p)
  FOR a = Depth TO 1 STEP -1
    IF y(x, p) < 27 THEN GOTO Crash
    IF y(x, p) > 339 AND Ship(p).Angle <> 0 THEN GOTO Crash
    Col = POINT(x(x, p) + (xx * a), y(x, p) + (yy * a))
    IF Col = 8 OR Col = 7 OR Col = RockCol THEN
      IF Damagable = 1 THEN
        FOR c = 1 TO DebrisNum
          IF CINT(Debris(c).x) = CINT(x(x, p) + (xx * a)) THEN
            IF CINT(Debris(c).y) = CINT(y(x, p) + (yy * a)) THEN
              GOTO NextPoint
            END IF
          END IF
        NEXT c
      END IF
      IF Ship(p).Angle > 0 THEN GOTO Crash
      'IF Ship(P).AddY < 0 THEN GOTO Crash
      IF Ship(p).Landed = 0 THEN
        IF y(x, p) >= 339 - Ship(p).AddY THEN
          Landing p
          EXIT SUB
        END IF
        FOR B = 0 TO MaxLines
          IF Lines(B).Landable = 1 THEN
            IF x(x, p) >= Lines(B).x1 AND x(x, p) <= Lines(B).x2 THEN
              IF y(x, p) + (yy * a) >= Lines(B).y1 - 5 AND y(x, p) + (yy * a) <= Lines(B).y2 + 5 THEN
                Landing p
                EXIT SUB
              END IF
            END IF
          END IF
          'IF Barrier(B).Landable = 1 THEN
          '  IF x(x, P) >= Barrier(B).x1 AND x(x, P) <= Barrier(B).x2 THEN
          '    IF y(x, P) + (yy * a) >= Barrier(B).y1 - 5 AND y(x, P) + (yy * a) <= Barrier(B).y2 + 5 THEN
          '      Landing
          '      EXIT SUB
          '    END IF
          '  END IF
          'END IF
        NEXT B
      END IF
      GOTO Crash
    END IF
  NEXT a
NextPoint:
NEXT x

EXIT SUB

Crash:
IF FunCol THEN
  Ship(p).Addx = -Ship(p).Addx * ColCoff
  Ship(p).AddY = -Ship(p).AddY * ColCoff

  IF Noise = 1 THEN
    PLAY "O1MBL52C"
  END IF
  PLAY Hit$

  IF MaxDamage THEN
    Player(p).Dmg = Player(p).Dmg + (MaxDamage * CollisionDamage)
  ELSE
    Player(p).Dmg = 100
  END IF
  Player(p).Dead = p
  PrintStats p, 1
  IF Player(p).Dmg >= MaxDamage THEN
    Ship(p).AddY = 0
    Ship(p).Addx = 0
    InitExplosion p
    Player(p).Crash = Player(p).Crash + 1
  ELSE Player(DeadPlayer).Dead = 0
  END IF
ELSE
  Player(p).Crash = Player(p).Crash + 1
  Player(p).Dead = p: InitExplosion p
END IF

END SUB

DEFSNG A-Z
SUB ClearShip (w AS INTEGER)
DEFINT A-Z

'w = P

IF Player(w).Lives >= 0 AND Ship(w).Exploded = 0 THEN
  FOR B = 1 TO Links(w)
    LINE (x(Link(B, 1, w), w), y(Link(B, 1, w), w))-(x(Link(B, 2, w), w), y(Link(B, 2, w), w)), 0
  NEXT B
END IF


END SUB

DEFSNG A-Z
SUB Continue
DEFINT A-Z

'ShowScreen
IF Ship(Plyr).Exploded > INT(170 - Compensation) / Compensation THEN Player(Plyr).Lives = Player(Plyr).Lives - 1: Ship(Plyr).Addx = 0: Ship(Plyr).AddY = 0
F = 0
FOR x = 1 TO Players
  IF Player(x).Lives < 0 THEN F = F + 1
NEXT x
PrintStats Plyr, 1
IF F = Players - 1 THEN GOTO GameOver
GOTO ContinueGame

GameOver:
d = MULTIKEY(-2)
CLS
SCREEN 9
InitStars

GameOver2:
DIM HitAccuracy AS SINGLE
i$ = ""
WHILE i$ = ""
  i$ = INKEY$
  FOR G = 1 TO Players
    HitAccuracy = 0
    COLOR Ship(G).Col: DOWN = 1 + (4 * G)
    LOCATE DOWN, 20: PRINT "Frags: "; Player(G).Frags
    LOCATE DOWN, 35: PRINT "Kills: "; Player(G).Kills
    LOCATE DOWN + 1, 21: PRINT "èÆØ†§†≠®©: "; Player(G).ShotsHit
    LOCATE DOWN + 1, 40: PRINT "ÇÎ·‚‡•´Æ¢: "; Player(G).ShotsTkn
    LOCATE DOWN + 2, 41: PRINT "ä‡„Ë•≠®©: "; Player(G).Crash
    IF Player(G).ShotsTkn = 0 THEN HitAccuracy = -.1
    IF HitAccuracy >= 0 THEN HitAccuracy = (Player(G).ShotsHit / Player(G).ShotsTkn) * 100
    LOCATE DOWN + 2, 22: PRINT "å•‚™Æ·‚Ï:"; CINT(HitAccuracy); "%"
  NEXT G
  COLOR 15: LOCATE 3, 30: PRINT "à£‡† é™Æ≠Á•≠†!"
  LOCATE 23, 25: PRINT "ç†¶¨®‚• èêéÅÖã §´Ô Ø‡Æ§Æ´¶•≠®Ô "
  StarField
WEND

IF UCASE$(i$) = " " THEN
            CLS
            'ERASE ShipX, ShipY
            FOR Reinit = 1 TO 4
                Player(Reinit).Lives = Life
                Player(Reinit).Dead = 0
            NEXT Reinit
            Restart = 1
            'QuitGame = 1
            GOTO ContinueGame
END IF
GOTO GameOver2

ContinueGame:
IF Restart = 0 THEN
  Ship(Plyr).Refresh = 1
  InitPlayer Plyr, 1
  RotateShip Plyr
  PrintStats Plyr, 1
END IF

SCREEN 9
END SUB

DEFSNG A-Z
SUB Delay (t AS INTEGER)
DEFINT A-Z
 
  FOR i = 1 TO t
    WAIT &H3DA, 8
  NEXT

END SUB

DEFSNG A-Z
SUB DoDebris
DEFINT A-Z

FlagSet = 0

'erase the old points
FOR a = 1 TO DebrisNum
   PSET (Debris(a).x, Debris(a).y), 0
NEXT a

'move them
FOR a = 1 TO DebrisNum

  Debris(a).AddY = Debris(a).AddY + Gravity * 4

  Debris(a).y = Debris(a).y + Debris(a).AddY
  Debris(a).x = Debris(a).x + Debris(a).Addx

  Col = POINT(Debris(a).x, Debris(a).y)
  IF Col = RockCol OR Col = 8 OR Debris(a).Life > 1000 OR Debris(a).x > 635 OR Debris(a).x < 5 OR Debris(a).y < 29 OR Debris(a).y > 345 THEN
    'Debris(a).AddY = 0
    flag = 1
    FlagSet = 1
  ELSE
    flag = 0
    Debris(a).Life = Debris(a).Life + 1
  END IF

  IF Col = 0 THEN Debris(a).Show = 1 ELSE Debris(a).Show = 0

  IF flag = 1 THEN
    'Debris(a).AddY = 0

    c = 0
    FOR B = 1 TO DebrisNum
      c = c + 1
      IF B = a THEN c = c + 1
      IF c > DebrisNum THEN EXIT FOR

      IF Debris(c).Life > -1 THEN
        Debris(B).x = Debris(c).x
        Debris(B).y = Debris(c).y
    
        Debris(B).AddY = Debris(c).AddY
        Debris(B).Addx = Debris(c).Addx
    
        Debris(B).Show = Debris(c).Show

        Debris(B).Life = Debris(c).Life
      END IF
    NEXT
    DebrisNum = DebrisNum - 1

    Debris(DebrisNum + 1).Life = -1
    TakeOff = TakeOff + 1
  ELSE
    IF Debris(a).Show = 1 THEN PSET (Debris(a).x, Debris(a).y), Debris(a).Col
  END IF
NEXT a

'IF DebrisNum < MaxDebris AND TakeOff THEN
'DebrisNum = DebrisNum + 1
'IF TakeOff THEN DebrisNum = DebrisNum - TakeOff + 1
'DebrisNum = DebrisNum - TakeOff
'END IF

IF FlagSet AND DebrisNum > 0 THEN DebrisNum = DebrisNum + 1
END SUB

DEFSNG A-Z
SUB DoFlare (Thrusts AS INTEGER)
DEFINT A-Z

 IF ThrustTime(Thrusts, Plyr) - TIMER > .1 THEN Col = 14 ELSE Col = 6

 ThrustXL(1, Plyr) = x(Engine(Plyr), Plyr)
 ThrustYU(1, Plyr) = y(Engine(Plyr), Plyr)
 ThrustXR(1, Plyr) = ThrustXL(1, Plyr) + (5 * Sine(Ship(Plyr).Angle))
 ThrustYD(1, Plyr) = ThrustYU(1, Plyr) + (5 * Cosine(Ship(Plyr).Angle))

 FlareBackCol1 = POINT(ThrustXR(1, Plyr), ThrustYD(1, Plyr))
 FlareBackCol2 = POINT(ThrustXL(1, Plyr) + (2 * Sine(Ship(Plyr).Angle)), ThrustYU(1, Plyr) + (2 * Cosine(Ship(Plyr).Angle)))
 IF FlareBackCol1 = 7 OR FlareBackCol1 = 8 OR FlareBackCol2 = 7 OR FlareBackCol2 = 8 THEN
   ThrustTime(Thrusts, Plyr) = 0
   Thrusts = 0
   LINE (ThrustXL(w, Plyr), ThrustYU(w, Plyr))-(ThrustXR(w, Plyr), ThrustYD(w, Plyr)), 0
 ELSE Thrusts = 1
 END IF
 IF Thrusts = 1 THEN LINE (ThrustXL(1, Plyr), ThrustYU(1, Plyr))-(ThrustXR(1, Plyr), ThrustYD(1, Plyr)), Col

END SUB

DEFSNG A-Z
SUB DoLaser
DEFINT A-Z

Hit$ = "O0MBL60BL30D"
IF Noise = 0 THEN Hit$ = "": Bang$ = ""
DIM flag(Ship(Plyr).Fired)
DIM a AS SINGLE, Accurate AS SINGLE, OldLaserX AS SINGLE, OldLaserY AS SINGLE

FOR p = 1 TO Ship(Plyr).Fired
 flag(p) = 0: Hit = 0: DontPrint = 0: DontPrint2 = 0
 OldLaserX = Laser(p, Plyr).x: OldLaserY = Laser(p, Plyr).y
 Colour = POINT(OldLaserX, OldLaserY)
 IF Colour = 8 THEN DontPrint2 = 1

 Laser(p, Plyr).x = Laser(p, Plyr).x - Laser(p, Plyr).Xspeed
 Laser(p, Plyr).y = Laser(p, Plyr).y - Laser(p, Plyr).Yspeed

 IF Laser(p, Plyr).x - Laser(p, Plyr).Xspeed <= 5 OR Laser(p, Plyr).x - Laser(p, Plyr).Xspeed >= 635 THEN flag(p) = 1
 IF Laser(p, Plyr).y - Laser(p, Plyr).Yspeed <= 29 OR Laser(p, Plyr).y - Laser(p, Plyr).Yspeed >= 339 THEN flag(p) = 1

 Accurate = LaserSpeed
 FOR a = 0 TO 1 STEP (1 / Accurate)
   Laser(p, Plyr).BackCol = POINT(Laser(p, Plyr).x - (a * (Laser(p, Plyr).Xspeed)), Laser(p, Plyr).y - (a * (Laser(p, Plyr).Yspeed)))

   'Check laser for hit rock
   IF Laser(p, Plyr).BackCol = 8 OR Laser(p, Plyr).BackCol = 7 OR Laser(p, Plyr).BackCol = RockCol THEN

     'check for hitting onto debris
     IF Damagable = 1 THEN
       FOR d = 1 TO DebrisNum
         IF CINT(Debris(d).x) = CINT(Laser(p, Plyr).x - (a * (Laser(p, Plyr).Xspeed))) THEN
           IF CINT(Debris(d).y) = CINT(Laser(p, Plyr).y - (a * (Laser(p, Plyr).Yspeed))) THEN GOTO Skip
         END IF
       NEXT d
     END IF
     DontPrint = 1

     'init new debris
     IF Damagable THEN
       x = Laser(p, Plyr).x - (a * Laser(p, Plyr).Xspeed)
       y = Laser(p, Plyr).y - (a * Laser(p, Plyr).Yspeed)

'LINE (0, 341)-(640, 350), RockCol, BF
'LINE (0, 29)-(5, 350), RockCol, BF

'LINE (635, 29)-(640, 350), RockCol, BF'ver

'LINE (0, 28)-(640, 28), RockCol, BF'hor

       IF x > 5 AND x < 635 AND y > 28 AND y < 341 THEN
         PSET (x, y), 0
         SCREEN 9, , 0, 1
         PSET (x, y), 0
         SCREEN 9, , 1, 1
       END IF

       Laser(p, Plyr).x = x
       Laser(p, Plyr).y = y
       IF DebrisNum < MaxDebris THEN
         InitDebris Laser(p, Plyr)'Laser(P, Plyr).x - (a * (Laser(P, Plyr).Xspeed)), Laser(P, Plyr).y - (a * (Laser(P, Plyr).Yspeed)), Laser(P,Plyr).BackCol
       END IF
     END IF
     flag(p) = 1: EXIT FOR

   'Hmmm... Maybe we hit some ship, check this out!
   ELSEIF Laser(p, Plyr).BackCol > 0 THEN

     'Checks for ships
     IF Laser(p, Plyr).BackCol < 7 OR Laser(p, Plyr).BackCol > 8 THEN
       FOR B = 1 TO Players

         'dont hit onto ship, who spawned it!
         IF B = Plyr OR Ship(B).Exploded > 0 THEN GOTO NextShipLoop

         'check color
         IF Laser(p, Plyr).BackCol = Ship(B).Col AND Laser(p, Plyr).y > 29 AND Ship(B).Refresh = 0 THEN
           flag(p) = 1
           DontPrint2 = 0: DeadPlayer = B: Hit = 1: EXIT FOR
         END IF
NextShipLoop:
       NEXT B
     END IF
   END IF
 NEXT a

Skip:
 IF Hit = 1 THEN
        PLAY Hit$

        'increase damage
        Player(DeadPlayer).Dmg = Player(DeadPlayer).Dmg + 1
        flag(p) = 1
        Player(DeadPlayer).Dead = DeadPlayer

        'increase hit counter
        Player(Plyr).ShotsHit = Player(Plyr).ShotsHit + 1

        'if he uses shield decrease damage
        IF Ship(DeadPlayer).Col = 15 THEN Player(DeadPlayer).Dmg = Player(DeadPlayer).Dmg - 1

        OldPlyr = Plyr
        Plyr = DeadPlayer
        PrintStats Plyr, 1

        'check for critical damage rate
        IF Player(DeadPlayer).Dmg >= MaxDamage THEN
          Ship(DeadPlayer).AddY = 0
          Ship(DeadPlayer).Addx = 0
          'Ship(DeadPlayer).Frags = Ship(DeadPlayer).Frags - 1
          InitExplosion DeadPlayer
          Player(OldPlyr).Kills = Player(OldPlyr).Kills + 1
          Player(OldPlyr).Frags = Player(OldPlyr).Frags + 1
        ELSE Player(DeadPlayer).Dead = 0
        END IF
        Plyr = OldPlyr
 END IF

 'erase laser
 IF DontPrint2 = 0 THEN
   FOR a = 1 TO ShotLength
    PRESET (OldLaserX + (a - 1) * (Laser(p, Plyr).Xspeed / LaserSpeed), OldLaserY + (a - 1) * (Laser(p, Plyr).Yspeed / LaserSpeed)), 0
   NEXT a
 END IF

 'draw laser
 IF flag(p) = 0 AND DontPrint = 0 THEN
   FOR B = 1 TO ShotLength
     PSET (Laser(p, Plyr).x + (B - 1) * (Laser(p, Plyr).Xspeed / LaserSpeed), Laser(p, Plyr).y + (B - 1) * (Laser(p, Plyr).Yspeed / LaserSpeed)), 14
   NEXT B
 END IF
NEXT p

Total = 0
p = 0
DO
 p = p + 1
 IF flag(p) = 1 THEN
   Laser(p, Plyr).Life = 0

   'move the lasers
   FOR Q = p + 1 TO Ship(Plyr).Fired - Total
     Laser(Q - 1, Plyr).x = Laser(Q, Plyr).x
     Laser(Q - 1, Plyr).y = Laser(Q, Plyr).y
     Laser(Q - 1, Plyr).Xspeed = Laser(Q, Plyr).Xspeed
     Laser(Q - 1, Plyr).Yspeed = Laser(Q, Plyr).Yspeed
     flag(Q - 1) = flag(Q): flag(Q) = 0
     IF Bounce = 1 THEN
       Laser(Q - 1, Plyr).Life = Laser(Q, Plyr).Life
       Laser(Q, Plyr).Life = 0
     END IF
   NEXT Q
   p = p - 1
   Total = Total + 1
 END IF
LOOP UNTIL p = Ship(Plyr).Fired - Total

Ship(Plyr).Fired = Ship(Plyr).Fired - Total

END SUB

DEFSNG A-Z
SUB DrawBSVimage (x AS INTEGER, y AS INTEGER, file$)
  'ÇÎ¢Æ§®‚ ≠† Ì™‡†≠ BSAVE-‰†©´ (¨†™·®¨„¨: Ë®‡®≠† - 640/2, ¢Î·Æ‚†, 350/2)

  DIM arr(14008)

  DEF SEG = VARSEG(arr(0))
    BLOAD file$, 0
  DEF SEG
  PUT (x, y), arr(0)

  ERASE arr

END SUB

SUB DrawLightning (x AS INTEGER, y AS INTEGER, l AS INTEGER, ang AS INTEGER)
DEFINT A-Z

ang = 360 - ang

xx = x: yy = y
'rad = ang * 3.141593 / 180: s = SIN(rad): c = COS(rad)

FOR i = 0 TO l
  'rang = CINT(RND * 90) - 45
  rang = CINT(RND * 120) - 60
  rang = ang + rang
  rrad = rang * 3.14159 / 180
  cc = COS(rrad): ss = SIN(rrad)
  xx = xx + cc'(c * (RND * 2 - 1))
  yy = yy + ss'(s * (RND * 2 - 1))

  SELECT CASE CINT(RND * 1)
    CASE 0: Col = 15
    CASE 1: Col = 7
  END SELECT

  IF POINT(xx, yy) THEN
    FOR B = 1 TO Players
      IF B <> Plyr AND Ship(B).Exploded = 0 AND POINT(xx, yy) = Ship(B).Col AND yy > 29 AND Ship(B).Col <> 15 THEN
      'ELSEIF POINT(xx, yy) = Ship(B).Col THEN
        IF Noise THEN PLAY "O0MBL60BL30D"
        Player(B).Dmg = MaxDamage
        Player(B).Dead = B: DeadPlayer = B
        OldPlyr = Plyr
        Plyr = B
        'PrintStats
        Ship(DeadPlayer).AddY = 0
        Ship(DeadPlayer).Addx = 0
        InitExplosion DeadPlayer
        Player(OldPlyr).Kills = Player(OldPlyr).Kills + 1
        Player(OldPlyr).Frags = Player(OldPlyr).Frags + 1
        Plyr = OldPlyr
      END IF
    NEXT B
  END IF

  PSET (xx, yy), Col
NEXT

END SUB

DEFSNG A-Z
SUB DrawMap (Col AS INTEGER)
DEFINT A-Z

LINE (0, 341)-(640, 350), Col, BF
LINE (0, 29)-(5, 350), Col, BF
LINE (635, 29)-(640, 350), Col, BF
LINE (0, 28)-(640, 28), Col, BF

FOR i = 0 TO MaxLines
  LINE (Lines(i).x1, Lines(i).y1)-(Lines(i).x2, Lines(i).y2), Col
NEXT

FOR i = 0 TO MaxCurves
  CIRCLE (Curves(i).x, Curves(i).y), Curves(i).Radius, Col, Curves(i).Start, Curves(i).Fin, Curves(i).Ratio
NEXT

'FOR x = 1 TO Barriers + Curves
'  IF x <= Barriers THEN
'    LINE (Barrier(x).x1, Barrier(x).y1)-(Barrier(x).x2, Barrier(x).y2), RockCol
'  ELSE
'    CIRCLE (Barrier(x).x1, Barrier(x).y1), Barrier(x).Radius, 7, Barrier(x).Start, Barrier(x).Fin, Barrier(x).Ratio
'  END IF
'NEXT x


END SUB

DEFSNG A-Z
SUB DrawShip
DEFINT A-Z

FOR Plyr = 1 TO Players
  ClearShip Plyr
  IF Ship(Plyr).Exploded = 0 AND Player(Plyr).Lives >= 0 AND Ship(Plyr).Refresh = 0 THEN
    FOR a = 1 TO Points(Plyr)
      x(a, Plyr) = x(a, Plyr) + Ship(Plyr).Addx
      y(a, Plyr) = y(a, Plyr) + Ship(Plyr).AddY
    NEXT a
    FOR B = 1 TO Links(Plyr)
      LINE (x(Link(B, 1, Plyr), Plyr), y(Link(B, 1, Plyr), Plyr))-(x(Link(B, 2, Plyr), Plyr), y(Link(B, 2, Plyr), Plyr)), Ship(Plyr).Col
    NEXT B

    IF DEBUG THEN
      PSET (Ship(Plyr).PosX + 13, Ship(Plyr).PosY + 13), 15
    END IF
  END IF
NEXT Plyr

END SUB

DEFSNG A-Z
SUB DrawTXTimage (x AS INTEGER, y AS INTEGER, file$)

  OPEN file$ FOR INPUT AS #1

  INPUT #1, txt$

  PSET (x, y), POINT(x, y)

  WHILE NOT EOF(1)
    INPUT #1, t$
    DRAW t$
  WEND

  CLOSE #1

END SUB

SUB DrawYCFimage (x AS INTEGER, y AS INTEGER, file$)

  DIM l AS STRING * 640

  OPEN file$ FOR INPUT AS #1
  INPUT #1, ycf$
  INPUT #1, xxx, yyy

  FOR xx = 0 TO xxx
    INPUT #1, l$
    FOR yy = 1 TO yyy
      Col = ASC(MID$(l$, yy, 1)) - 50
      PSET (xx + x, yy + y), Col
    NEXT
  NEXT

  CLOSE #1

END SUB

SUB Explosion
DEFINT A-Z

Ship(Plyr).Exploded = Ship(Plyr).Exploded + 1
IF Ship(Plyr).Exploded = ExplosionTime THEN Continue: EXIT SUB

FOR x = 1 TO Bits
    OldDotX = Dot(x, Plyr).x: OldDotY = Dot(x, Plyr).y: OldDotCol = Dot(x, Plyr).BackCol
    IF Dot(x, Plyr).x + Dot(x, Plyr).Addx <= 7 OR Dot(x, Plyr).x + Dot(x, Plyr).Addx > 630 THEN
      Dot(x, Plyr).x = Dot(x, Plyr).x - Dot(x, Plyr).Addx
      Dot(x, Plyr).Addx = -Dot(x, Plyr).Addx * .5
    END IF
    IF Dot(x, Plyr).y + Dot(x, Plyr).AddY < 30 OR Dot(x, Plyr).y + Dot(x, Plyr).AddY >= 340 THEN
      Dot(x, Plyr).y = Dot(x, Plyr).y - Dot(x, Plyr).AddY
      Dot(x, Plyr).AddY = -Dot(x, Plyr).AddY * .5
      Dot(x, Plyr).Addx = Dot(x, Plyr).Addx / 1.5
    END IF
    Dot(x, Plyr).AddY = Dot(x, Plyr).AddY + (Gravity * 4)
    Dot(x, Plyr).x = Dot(x, Plyr).x + (Compensation * Dot(x, Plyr).Addx)
    Dot(x, Plyr).y = Dot(x, Plyr).y + (Compensation * Dot(x, Plyr).AddY)
    Dot(x, Plyr).BackCol = POINT(Dot(x, Plyr).x, Dot(x, Plyr).y)
    IF Dot(x, Plyr).BackCol <> 8 THEN
      IF Dot(x, Plyr).BackCol <> 7 THEN
        IF Dot(x, Plyr).BackCol <> RockCol THEN Dot(x, Plyr).BackCol = 0
      END IF
    END IF
    IF Dot(x, Plyr).BackCol = 8 THEN
      FOR d = 1 TO DebrisNum
        IF CINT(Dot(x, Plyr).x) = CINT(Debris(d).x) THEN
          IF CINT(Dot(x, Plyr).y) = CINT(Debris(d).y) THEN Dot(x, Plyr).BackCol = 0: EXIT FOR
        END IF
      NEXT d
    END IF

    PRESET (OldDotX, OldDotY), OldDotCol
    IF INT(Ship(Plyr).Exploded) <= INT(((BitLife - 1) - Compensation) / Compensation) THEN
      IF Dot(x, Plyr).BackCol = 0 THEN PSET (Dot(x, Plyr).x, Dot(x, Plyr).y), Dot(x, Plyr).Col
    END IF
NEXT x

END SUB

DEFSNG A-Z
SUB FireShot (p AS INTEGER)
DEFINT A-Z

IF Player(p).Lives >= 0 AND Ship(p).Fired < LasersAllowed AND Ship(p).Col < 15 AND Player(p).ShotsLft > 0 AND Ship(p).Exploded = 0 THEN
    Player(p).ShotsLft = Player(p).ShotsLft - 1
    PrintStats p, 1
    InitLaser p
END IF

END SUB

SUB GetAction (p AS INTEGER)

  IF Ship(p).DelayControl <= 0 THEN
    IF Player(p).AI = AILOCAL THEN
      '
      ' Human Player
      '

      IF MULTIKEY(Ship(p).KEYUP) THEN
        Ship(p).DelayControl = CONTROLDELAY
        Ship(p).UP = 1
      END IF
      IF MULTIKEY(Ship(p).KEYLEFT) THEN
        Ship(p).DelayControl = CONTROLDELAY
        Ship(p).LEFT = 1
      END IF
      IF MULTIKEY(Ship(p).KEYRIGHT) THEN
        Ship(p).DelayControl = CONTROLDELAY
        Ship(p).RIGHT = 1
      END IF
      IF MULTIKEY(Ship(p).KEYDOWN) THEN
        Ship(p).DelayControl = CONTROLDELAY
        Ship(p).DOWN = 1
      END IF
      IF MULTIKEY(Ship(p).KEYSHIELD) THEN
        Ship(p).DelayControl = CONTROLDELAY
        Ship(p).Shield = 1
      END IF

    ELSEIF Player(p).AI = AICPU THEN
      '
      ' Computer Player
      '

      AIFindMission p
      turn = AIFaceTarget(p)

      IF turn THEN
        IF Ship(p).Landed THEN
          Ship(p).UP = 1
        END IF

        IF turn > 0 THEN
          Ship(p).LEFT = 1
        ELSEIF turn < 0 THEN
          Ship(p).RIGHT = 1
        END IF

        Ship(p).DOWN = 1
      ELSEIF Player(p).Target > 0 THEN
        Ship(p).UP = 1
        Ship(p).DOWN = 1
      END IF

      Ship(p).DelayControl = CONTROLDELAY
    END IF

  ELSE
    Ship(p).DelayControl = Ship(p).DelayControl - 1
  END IF

END SUB

DEFSNG A-Z
SUB GetMapData
DEFINT A-Z

OPEN "data\maymap.dat" FOR INPUT AS #1
INPUT #1, Maps

FOR mmm = 1 TO Map
 'general information
 INPUT #1, MaxLines, MaxCurves, MaxPaints, MaxLandspots

 IF (MaxLines > 0) THEN
   REDIM Lines(MaxLines) AS LineType
 ELSE
   ERASE Lines
 END IF
 IF (MaxCurves > 0) THEN
   REDIM Curves(MaxCurves) AS CurveType
 ELSE
   ERASE Curves
 END IF
 IF (MaxPaints > 0) THEN
   REDIM Paints(MaxPaints) AS PaintType
 ELSE
   ERASE Paints
 END IF
 IF (MaxLandspots > 0) THEN
   REDIM Landspots(MaxLandspots) AS LandspotType
 ELSE
   ERASE Landspots
 END IF

 'lines
 FOR i = 0 TO MaxLines
   Lines(i).Landable = 0
   INPUT #1, Lines(i).x1, Lines(i).y1, Lines(i).x2, Lines(i).y2
   IF Lines(i).y1 = Lines(i).y2 THEN Lines(i).Landable = 1
   IF Lines(i).x2 < Lines(i).x1 AND Lines(i).Landable THEN
     tmp = Lines(i).x1
     Lines(i).x1 = Lines(i).x2
     Lines(i).x2 = tmp
   END IF
 NEXT
 
 'curves
 FOR i = 0 TO MaxCurves
   INPUT #1, Curves(i).x, Curves(i).y, Curves(i).Radius, Curves(i).Start, Curves(i).Fin, Curves(i).Ratio
 NEXT
 
 'paints
 FOR i = 0 TO MaxPaints
   INPUT #1, Paints(i).x, Paints(i).y
 NEXT
 
 'landspots
 FOR i = 0 TO MaxLandspots
   INPUT #1, Landspots(i).x, Landspots(i).y
 NEXT

NEXT

CLOSE #1
                   
'Beginning:
'FOR u = 1 TO Maps
' INPUT #3, MapNum, Barriers, Curves, PaintPoints, Pricks, Landings
' FOR t = 1 TO Barriers + Curves
'   Barrier(t).Landable = 0
'   Barrier(t).Curved = 0
'   IF t <= Barriers THEN
'     INPUT #3, Barrier(t).x1, Barrier(t).y1, Barrier(t).x2, Barrier(t).y2
'     IF (Barrier(t).y1 - Barrier(t).y2) = 0 THEN Barrier(t).Landable = 1
'   ELSE
'     Barrier(t).Curved = 1
'     INPUT #3, Barrier(t).x1, Barrier(t).y1, Barrier(t).Radius, Barrier(t).Start, Barrier(t).Fin, Barrier(t).Ratio
'   END IF
' NEXT t
' FOR uu = 1 TO PaintPoints
'   INPUT #3, PaintX(uu), PaintY(uu)
' NEXT uu
' FOR l = 1 TO Landings
'   INPUT #3, ShipX(l), ShipY(l)
' NEXT', ShipX(2), ShipY(2), ShipX(3), ShipY(3), ShipX(4), ShipY(4)
' IF MapNum = Map THEN GOTO Finish
'NEXT u
'
'Finish:
'CLOSE #3

IF Map = 0 THEN
  MaxLandspots = 3: MaxLines = 0: MaxCurves = 0: MaxPaints = 0
  Landspots(0).x = 150: Landspots(0).y = 320
  Landspots(1).x = 500: Landspots(1).y = 320
  Landspots(2).x = 300: Landspots(2).y = 320
  Landspots(3).x = 400: Landspots(3).y = 320
  'ShipX(1) = 150: ShipY(1) = 320
  'ShipX(2) = 500: ShipY(2) = 320
  'ShipX(3) = 300: ShipY(3) = 320
  'ShipX(4) = 400: ShipY(4) = 320
  EXIT SUB
ELSEIF Map > 0 THEN
'  FOR xx = 1 TO 4
'    Ship(xx).PosX = Landspots(xx).x
'    Ship(xx).PosY = Landspots(xx).y
'    'Ship(xx).PosX = ShipX(xx)
'    'Ship(xx).PosY = ShipY(xx)
'  NEXT xx
END IF

IF Players > MaxLandspots + 1 THEN Players = MaxLandspots + 1

END SUB

DEFSNG A-Z
SUB GetShipData
DEFINT A-Z

ReadFile:
FOR w = 1 TO 4
  OPEN "data\ship.Dat" FOR INPUT AS #3
  INPUT #3, Ships
  FOR x = 1 TO Ships
    INPUT #3, ShipNum
    INPUT #3, Points(w), Links(w), ShipTurrets(w)
    FOR xx = 1 TO Points(w)
      INPUT #3, Size(xx, 1, w), Size(xx, 2, w)
    NEXT xx
    FOR y = 1 TO Links(w)
      INPUT #3, Link(y, 1, w), Link(y, 2, w)
    NEXT y
    FOR Z = 1 TO ShipTurrets(w)
      INPUT #3, ShipTurret(Z, w)
    NEXT Z
    INPUT #3, Engine(w)
    IF x = ShipChoice(w) THEN GOTO NextShip
  NEXT x
NextShip:
CLOSE #3
NEXT w

Finished:
CLOSE #3
END SUB

SUB Init

DIM rad AS DOUBLE
DIM cmd AS STRING

cmd = UCASE$(COMMAND$)

PRINT "ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ"
PRINT "MAYHEM v"; Version$; " by MAZsoft 1999-2000"
PRINT "ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ"
PRINT
PRINT "Initializing variables..."

IF INSTR(cmd, "DEBUG") THEN
  DEBUG = 1
END IF

FOR u = 1 TO 4
 ShipChoice(u) = u
 Ship(u).Col = u + 9
 Player(u).Lives = 9
NEXT u

FOR Angle = 0 TO 360
 rad = Angle * 3.141593 / 180
 Cosine(Angle) = COS(rad)
 Sine(Angle) = SIN(rad)
NEXT Angle

PRINT "Loading configuration from file..."

LoadCFG

PRINT "Loading game data..."

GetShipData

FirstTime = 1

SCREEN 9: RANDOMIZE TIMER

SLEEP 1

END SUB

DEFSNG A-Z
SUB InitDebris (l AS LaserType) 'x AS INTEGER, y AS INTEGER, col AS integer)
DEFINT A-Z

DebrisNum = DebrisNum + 1
aa = DebrisNum
Debris(DebrisNum).x = l.x: Debris(DebrisNum).y = l.y: Debris(DebrisNum).Life = 0
Debris(DebrisNum).Col = RND * 1 + 7'Col

Debris(DebrisNum).Addx = (l.Xspeed / 5)
Debris(DebrisNum).AddY = (l.Yspeed / 5)

x = l.x' + L.XSpeed'Debris(DebrisNum).x - Debris(DebrisNum).AddX
y = l.y' + L.YSpeed'Debris(DebrisNum).y - Debris(DebrisNum).AddY

'Col = POINT(x, y + 2)
'IF Col = 7 OR Col = 8 OR Col = RockCol THEN DebrisNum = DebrisNum - 1

IF x > 6 AND x < 634 AND y > 29 AND y < 340 THEN
  PSET (x, y), 0
  SCREEN 9, , 0, 1
  PSET (x, y), 0
  SCREEN 9, , 1, 1
END IF

END SUB

DEFSNG A-Z
SUB InitExplosion (p AS INTEGER)
DEFINT A-Z

Bang$ = "MBL32O0GGDDBA": IF Noise = 0 THEN Bang$ = ""
PLAY Bang$
IF Noise <> 0 THEN PLAY "MBL32O0GGDDBA"

Ship(p).Exploded = 1
Ship(p).Col = Ship(p).DefaultCol

IF Lightning THEN
  FOR i = 0 TO 10
    DrawLightning Ship(p).PosX + 13, Ship(p).PosY + 13, RND * 30 + 10, RND * 360
  NEXT
END IF

Landspots(Ship(p).LanPos - 1).Landed = 0
'Landed(Ship(P).LanPos) = 0

w = p
ShowScreen
p = w

IF Ship(p).Col > 8 THEN ChangeCol = -8 ELSE ChangeCol = 8
FOR x = 1 TO Bits
    Dot(x, p).Direction = INT(0 + 360 * RND)
    Dot(x, p).x = Ship(p).PosX + 13
    Dot(x, p).y = Ship(p).PosY + 13
    Dot(x, p).Speed = (.1 + .85 * RND)
    Dot(x, p).BackCol = 0
    Dot(x, p).Col = INT(1 + 3 * RND)
    IF Dot(x, p).Col = 1 THEN Dot(x, p).Col = Ship(p).Col + ChangeCol ELSE Dot(x, p).Col = Ship(p).Col
    Dot(x, p).Addx = (Dot(x, p).Speed * Cosine(Dot(x, p).Direction) + Ship(p).Addx)
    Dot(x, p).AddY = (Dot(x, p).Speed * Sine(Dot(x, p).Direction) + Ship(p).AddY)
NEXT x

END SUB

DEFSNG A-Z
SUB InitFlare (p AS INTEGER)
DEFINT A-Z
   
    IF Flare = 1 THEN
      FOR w = 1 TO Thrusts
       IF ThrustTime(w, p) - TIMER >= 0 AND Now = 1 THEN
         LINE (ThrustXL(w, p), ThrustYU(w, p))-(ThrustXR(w, p), ThrustYD(w, p)), 0: DoFlare Thrusts
       END IF
       IF ThrustTime(w, p) - TIMER < 0 AND ThrustTime(w, p) > 0 THEN ThrustTime(w, p) = 0: LINE (ThrustXL(w, p), ThrustYU(w, p))-(ThrustXR(w, p), ThrustYD(w, p)), 0: Thrusts = 0
      NEXT w
    END IF


END SUB

DEFSNG A-Z
SUB InitLaser (p AS INTEGER)
DEFINT A-Z

Bang$ = "O1MBL52C": Hit$ = "O0MBL60BL30D"

Player(p).ShotsTkn = Player(p).ShotsTkn + 1
Ship(p).Fired = Ship(p).Fired + 1
Turret(p) = Turret(p) + 1: IF Turret(p) > ShipTurrets(p) THEN Turret(p) = 1

Laser(Ship(p).Fired, p).x = x(ShipTurret(Turret(p), p), p) - ((ShotLength - 2) * Sine(Ship(p).Angle))
Laser(Ship(p).Fired, p).y = y(ShipTurret(Turret(p), p), p) - ((ShotLength - 2) * Cosine(Ship(p).Angle))

IF Noise = 1 THEN
  PLAY Bang$
END IF

Ppp = Ship(p).Fired

Laser(Ppp, p).Xspeed = LaserSpeed * Sine(Ship(p).Angle)
Laser(Ppp, p).Yspeed = LaserSpeed * Cosine(Ship(p).Angle)

IF Ship(p).Landed = 0 THEN
  Ship(p).Addx = Ship(p).Addx + (Laser(Ppp, p).Xspeed * BulletMass) / ShipMass
  Ship(p).AddY = Ship(p).AddY + (Laser(Ppp, p).Yspeed * BulletMass) / ShipMass
  'Ship(P).AddX = Ship(P).AddX + (Laser(P, P).Xspeed * 1) / 100
  'Ship(P).AddY = Ship(P).AddY + (Laser(P, P).Yspeed * 1) / 100
END IF

END SUB

DEFSNG A-Z
SUB InitPlayer (Plyr AS INTEGER, w AS INTEGER)
DEFINT A-Z
  
Player(Plyr).Dmg = 0
IF w THEN
do.landing:
  Count = Count + 1

  Ship(Plyr).LanPos = (RND * (MaxLandspots)) + 1
  IF Count > 100 THEN
    Ship(Plyr).LanPos = Plyr
    Landspots(Plyr - 1).Landed = 0
  END IF
IF Landspots(Ship(Plyr).LanPos - 1).Landed = 1 THEN GOTO do.landing
  Landspots(Ship(Plyr).LanPos - 1).Landed = 1
  Ship(Plyr).PosX = Landspots(Ship(Plyr).LanPos - 1).x
  Ship(Plyr).PosY = Landspots(Ship(Plyr).LanPos - 1).y
ELSE
  Ship(Plyr).LanPos = Plyr
  Landspots(Plyr - 1).Landed = 1
  Ship(Plyr).PosX = Landspots(Plyr - 1).x
  Ship(Plyr).PosY = Landspots(Plyr - 1).y
END IF
Ship(Plyr).Landed = 0'1
Ship(Plyr).Col = Ship(Plyr).DefaultCol
Ship(Plyr).Exploded = 0
Ship(Plyr).Addx = 0
Ship(Plyr).AddY = 0
Player(Plyr).ShotsLft = ShotLoad
Player(Plyr).FuelLft = 100
Ship(Plyr).Angle = 0
Player(Plyr).Dead = 0

END SUB

DEFSNG A-Z
SUB InitStars
DEFINT A-Z

REDIM StarX(Stars) AS SINGLE, StarY(Stars) AS SINGLE, StarZ(Stars) AS SINGLE, StarCol(Stars)

FOR a = 1 TO Stars
  StarX(a) = (-80 * RND) + 40
  StarY(a) = (-80 * RND) + 40
  StarZ(a) = INT(350 * RND) + 50
  StarCol(a) = 8
NEXT a

END SUB

DEFSNG A-Z
SUB Intro
DEFINT A-Z

DIM Word$(17)
Word$(1) = "            Ü®ß≠®:"
Word$(2) = "     ã®¨®‚ ´†ß•‡†:"
Word$(3) = "    Player 1 Ê¢•‚:"
Word$(5) = "    Player 2 Ê¢•‚:"
Word$(7) = "    Player 3 Ê¢•‚:"
Word$(9) = "    Player 4 Ê¢•‚:"
Word$(11) = "             á¢„™:"
Word$(12) = "       É‡†¢®‚†Ê®Ô:"
Word$(13) = "      äÆ¨Ø•≠·†Ê®Ô:"
Word$(14) = "  èÆ´Æ¶. ™Æ‡†°´•©:"
Word$(15) = "          ÇÎÂ´ÆØÎ:"
Word$(16) = "        ë®´† ·™†´:" '
Word$(17) = "         ÇÆß¢‡†‚ ¢ ÉãÄÇçéÖ  "
FOR t = 4 TO 10 STEP 2  '
  Word$(t) = "Ç®§ äÆ‡†°´Ô:"
NEXT t

CLS

Map = -1
Plyrs = 2
Players = 4
'DebrisNum = 0
  
FOR d = 1 TO 4
  Ship(d).PosX = 510
  Ship(d).PosY = 115 + (d * 18)
  Ship(d).Angle = 0
  Ship(d).Addx = 0
  Ship(d).AddY = 0
  Ship(d).Exploded = 0
  Player(d).Dead = 0
  Plyr = d: RotateShip d
  Player(d).Dmg = 0
NEXT d

'Turns = 16
'Life = 9
Row = 1
IF Turns = 0 THEN Turns = 16
TurnAngle = 360 / Turns

'Word$ = "MAYHEM v" + Version$ + " °Î´† Ø‡®§„¨†≠† ® ‡†ß‡†°Æ‚†≠† Alex'Æ¨ Smith'Æ¨ - Ø®Ë®‚• •¨„ ≠† wedge@btinternet.com ¢·Ô™®• ¢ÆØ‡Æ·Î... è•‡•¢•§•≠Æ, „´„ÁË•≠Æ, ™Æ¨Ø®´®‡Æ¢†≠Æ, ·‚†´Æ §Æ·‚„Ø≠Æ ™ ®·ØÆ´ÏßÆ¢†≠®Ó °´†£Æ§†‡Ô è®È†£®≠„ å®Â†®´„ ®ß ™Æ¨Ø†≠®®"
'Word$ = Word$ + " MAZsoft. "
Word$ = "MAYHEM v" + Version$ + " é‡®£®≠†´Ï≠†Ô ®§•Ô -- Alex Smith(wedge@btinternet.com) èÆ ¢ÆØ‡Æ·†¨ §Æ‡†°Æ‚™®, °†£Æ¢, Æ°‡†È†‚Ï·Ô ™ MAZsoft(mailfrom@mail.ru) "

xx = 58: yy = 2
Col = 12
Wide = 17'20

Word$ = SPACE$(Wide) + Word$
Length = LEN(Word$)

OPEN "data\maymap.dat" FOR INPUT AS #3
INPUT #3, Maps
CLOSE #3

Speed = 1000: Sizes = 25

InitStars

IF FirstTime THEN
  SCREEN 9, , 1, 0
  'LINE (160, 175 / 2)-(160 + 320, 175 / 2 + 175), 15, B
  LINE (159, 175 / 2 - 1)-(160 + 320, 175 / 2 + 175 + 1), 15, B
  PAINT (5, 5), 15
  DrawBSVimage 160, 175 / 2, "data\maz.maz"
  LINE (160, 175 / 2 - 5)-(160 + 320, 175 / 2 + 5), 15, BF
  LINE (160, 175 / 2 + 170)-(160 + 320, 175 / 2 + 180), 15, BF
  SCREEN 9, , 1, 0
  PCOPY 1, 0
END IF

Introduction:
 
SCREEN 9, , 1, 0
CLS : GOSUB DrawMayhemLogo
SCREEN 9, , 0, 0

IF FirstTime THEN
  inp$ = INPUT$(1)

  SCREEN 9, , 1, 0
  CLS
  DrawBSVimage 64, 123, "data\maylogo.maz"
  SCREEN 9, , 0, 0
  PCOPY 1, 0

  inp$ = INPUT$(1)

  SCREEN 9, , 1, 0
  CLS : GOSUB DrawMayhemLogo
  SCREEN 9, , 0, 0
  'CLS
  PCOPY 1, 0
  FirstTime = 0
ELSE
  PCOPY 1, 0
END IF

'DrawTXTimage 160, 20, "data\mayhem.txt"
'DrawTXTimage 10, 260, "data\maz.txt"
'DrawYCFimage 0, 0, "data\mayhem.ycf"
'DrawTXTimage 400, 90, "data\version.txt"
'DrawTXTimage 510, 90, "data\ver.txt"

inp$ = INPUT$(1)

LOCATE 2, 54: PRINT "…ÕÕ[                 ]ÕÕª"
LOCATE 3, 54: PRINT "∫                       ∫"
LOCATE 4, 54: PRINT "∫                       ∫"
LOCATE 5, 54: PRINT "∫                       ∫"
LOCATE 6, 54: PRINT "∫                       ∫"
LOCATE 7, 54: PRINT "∫                       ∫"
LOCATE 8, 54: PRINT "∫                       ∫"
LOCATE 9, 54: PRINT "∫                       ∫"
LOCATE 10, 54: PRINT "»ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº"

DO
  action = 2
  c = c + 1: IF c > 100 THEN c = 1
  IF c = 1 THEN B = B + 1: IF B > Length THEN B = 1
  G$ = INKEY$
   SELECT CASE UCASE$(G$)
        'CASE "S", "õ": CLS : Setup: CLS : GOTO Introduction
        CASE CHR$(0) + "H": Row = Row - 1: IF Row < 1 THEN Row = 7
        CASE CHR$(0) + "P": Row = Row + 1: IF Row > 7 THEN Row = 1
        CASE CHR$(0) + "K": ' Left
          SELECT CASE Row
            CASE 2:
              IF Plyrs > 2 THEN Plyrs = Plyrs - 1
            CASE 3:
              IF Map > -1 THEN Map = Map - 1
          END SELECT
        CASE CHR$(0) + "M": ' Right
          SELECT CASE Row
            CASE 2:
              IF Plyrs < MaxPlayers THEN Plyrs = Plyrs + 1
            CASE 3:
              IF Map < Maps THEN Map = Map + 1
   END SELECT
        CASE " ", CHR$(13): SELECT CASE Row
                        CASE 1:
                                IF Map = -1 THEN Map = INT(Maps * RND) + 1
                                Ship(1).PosX = 0
                                Players = Plyrs
                                EXIT SUB
                        'CASE 2: Plyrs = Plyrs + 1
                        '        IF Plyrs > 4 THEN Plyrs = 1
                        'CASE 3: Map = Map + 1
                        '        IF Map > Maps THEN Map = -1
                        CASE 4: GOSUB SetupGame
                        CASE 5: GOTO OptionScreen
                        CASE 6: GOTO Controls
                        CASE 7: CALL Quit
                       END SELECT
   END SELECT
PrintScreen:

   '-9,+26
   IF Row = 1 THEN COLOR 14 ELSE COLOR 7
   LOCATE 3, 61: PRINT "çÄóÄíú àÉêì"
   IF Row = 2 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT " àÉêéäà : "; Plyrs
   IF Row = 3 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 60: IF Map >= 0 THEN PRINT "  äÄêíÄ : "; Map; "    " ELSE PRINT "äÄêíÄ : ë´„Á†©"
   IF Row = 4 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 60: PRINT "çÄëíêéâäÄ àÉêõ"
   IF Row = 5 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT "   éèñàà "
   IF Row = 6 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT " ìèêÄÇãÖçàÖ"
   IF Row = 7 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT "   ÇõïéÑ"

   ' €ª  €ª€€€€ª€ª  €ª€ª  €ª€€€€€ª€ª  €ª
   ' €€ª€€∫€…Õ€∫ €ª€…º€∫  €∫€…ÕÕÕº€€ª€€∫
   ' €…€∫€∫€€€€∫  €…º €€€€€∫€€€ª  €…€∫€∫
   ' €∫»º€∫€…Õ€∫  €∫  €…ÕÕ€∫€…Õº  €∫»º€∫
   ' €∫  €∫€∫ €∫  €∫  €∫  €∫€€€€€ª€∫  €∫
   ' »º  »º»º »º  »º  »º  »º»ÕÕÕÕº»º  »º
 
   'LOCATE 6, 35: COLOR 14: PRINT "M A Y H E M"
   'COLOR 7: LOCATE 20, 27: PRINT "Num Lock §Æ´¶•≠ °Î‚Ï ¢™´ÓÁ•≠."
   'LOCATE , 20: PRINT "Recommended to run in DOS before Win95 boots"
   'LOCATE 8, 35: COLOR 7: PRINT "Ç•‡·®Ô: "; Version$
   'LOCATE 10, 23: PRINT "àëèéãúáìâíÖ ëíêÖãäà à èêéÅÖã Ñãü ÇõÅéêÄ..."
   'GOSUB DoStars
   IF c = 1 THEN GOSUB Scroll
LOOP
GOTO Fin:


OptionScreen:
CLS
DrawBSVimage 187, 10, "data\options.maz"
Roww = 17
DO
 UP = 0
 FOR a = 1 TO 17
  IF Roww = a THEN COLOR 14 ELSE COLOR 7
  LOCATE a + 7 - UP, 25
  IF a = 4 OR a = 6 OR a = 8 OR a = 10 THEN LOCATE (a / 2) + 9, 53: UP = UP + 1
  PRINT Word$(a)
 NEXT a

IF action > 0 THEN

 IF action = 1 AND Roww = 1 THEN
    Life = Life + 1
    IF Life > 15 THEN Life = 1
 ELSEIF action = 2 AND Roww = 1 THEN
    Life = Life - 1
    IF Life < 0 THEN Life = 15
 END IF
 FOR x = 1 TO 4
    Player(x).Lives = Life
 NEXT x

 IF action = 1 AND Roww = 2 THEN
   LasersAllowed = LasersAllowed + 1
 ELSEIF action = 1 AND Roww = 2 THEN
   LasersAllowed = LasersAllowed - 1
 END IF
 IF LasersAllowed > 20 THEN LasersAllowed = 1
 IF LasersAllowed = 0 THEN LasersAllowed = 20

 FOR a = 1 TO 4
   IF action = 1 AND Roww = 1 + (2 * a) THEN
     Ship(a).Col = Ship(a).Col + 1
     IF Ship(a).Col = 7 OR Ship(a).Col = 8 THEN Ship(a).Col = 9
     FOR B = 1 TO 4
       IF B <> a THEN
         IF Ship(a).Col = Ship(B).Col THEN Ship(a).Col = Ship(a).Col + 1
       END IF
     NEXT B
     IF Ship(a).Col > 13 THEN Ship(a).Col = 2
   ELSEIF action = 2 AND Roww = 1 + (2 * a) THEN
     Ship(a).Col = Ship(a).Col - 1
     IF Ship(a).Col = 7 OR Ship(a).Col = 8 THEN Ship(a).Col = 9
     FOR B = 1 TO 4
       IF B <> a THEN
         IF Ship(a).Col = Ship(B).Col THEN Ship(a).Col = Ship(a).Col - 1
       END IF
     NEXT B
     IF Ship(a).Col < 2 THEN Ship(a).Col = 13
   END IF
 NEXT a

 FOR a = 1 TO 4
   'IF action = 1 THEN
   '  IF Roww = 2 + (2 * a) THEN
   '    ShipChoice(a) = ShipChoice(a) + 1
   '    IF ShipChoice(a) > Ships THEN ShipChoice(a) = 1
   '    LINE (513, 135)-(535, 214), 0, BF
   '    GetShipData
   '    Plyr = a: RotateShip: DrawShip
   '  END IF
   'END IF
 'NEXT a
   IF action = 1 THEN
    IF Roww = 2 + (2 * a) THEN
     ShipChoice(a) = ShipChoice(a) + 1
     IF ShipChoice(a) > Ships THEN ShipChoice(a) = 1
     LINE (513, 135)-(535, 214), 0, BF
     GetShipData
     'Plyr = a
     RotateShip a
     DrawShip
    END IF
   ELSEIF action = 2 THEN
    IF Roww = 2 + (2 * a) THEN
     ShipChoice(a) = ShipChoice(a) - 1
    IF ShipChoice(a) <= 0 THEN ShipChoice(a) = Ships
     LINE (513, 135)-(535, 214), 0, BF
     GetShipData
     'Plyr = a:
     RotateShip a
     DrawShip
    END IF
   END IF
 NEXT a

 IF Roww = 11 AND action = 1 THEN
   Noise = Noise + 1
 ELSEIF Roww = 11 AND action = 2 THEN
   Noise = Noise - 1
 END IF
 IF Noise < 0 THEN Noise = 1
 IF Noise > 1 THEN Noise = 0
 IF Noise = 1 THEN UseSound$ = "On " ELSE UseSound$ = "Off"

 IF action = 1 AND Roww = 12 THEN
   Gravity = Gravity + .001
 ELSEIF action = 2 AND Roww = 12 THEN
   Gravity = Gravity - .001
 END IF
 IF Gravity < 0 THEN Gravity = .021
 IF Gravity > .021 THEN Gravity = 0

 IF action = 1 AND Roww = 13 THEN
   Compensation = Compensation + 1
 ELSEIF action = 2 AND Roww = 13 THEN
   Compensation = Compensation - 1
 END IF
 IF Compensation < 1 THEN Compensation = 10
 IF Compensation > 10 THEN Compensation = 1

 IF action = 1 AND Roww = 14 THEN
   Turns = Turns + 1
 ELSEIF action = 2 AND Roww = 14 THEN
   Turns = Turns - 1
 END IF
 IF Turns < 4 THEN Turns = 360
 IF Turns > 360 THEN Turns = 4
 TurnAngle = 360 / Turns

 IF Roww = 15 AND action = 1 THEN Flare = Flare + 1: IF Flare > 1 THEN Flare = 0
 IF Flare = 0 THEN Flare$ = "Off" ELSE Flare$ = "On"

 IF Roww = 16 AND action = 1 THEN Damagable = Damagable + 1: IF Damagable > 1 THEN Damagable = 0
 IF Damagable = 0 THEN RockType$ = "Indestructable" ELSE RockType$ = "Soft          "

END IF

 COLOR 7
 LOCATE 8, 43: PRINT Life
 LOCATE 9, 43: PRINT LasersAllowed
 'LINE (343, 146)-(385, 154), Ship(1).Col, BF
 'LINE (343, 162)-(385, 170), Ship(2).Col, BF
 'LINE (343, 178)-(385, 186), Ship(3).Col, BF
 'LINE (343, 194)-(385, 202), Ship(4).Col, BF

 LINE (343, 130)-(385, 138), Ship(1).Col, BF
 LINE (343, 144)-(385, 152), Ship(2).Col, BF
 LINE (343, 158)-(385, 167), Ship(3).Col, BF
 LINE (343, 173)-(385, 180), Ship(4).Col, BF

 LOCATE 14, 44: PRINT UseSound$
 LOCATE 15, 43: PRINT Gravity * 1000; "          "
 LOCATE 16, 43: PRINT Compensation
 LOCATE 17, 43: PRINT Turns
 LOCATE 18, 44: PRINT Flare$; " "
 LOCATE 19, 44: PRINT RockType$
 'LOCATE 22, 13: COLOR 7: PRINT "Increasing compensation keeps the frame rate high, but slows"
 'LOCATE 23, 11: PRINT "the frame rate for explosions, engine flares and moving barriers"

 'LOCATE 22, 10: COLOR 7: PRINT "ì¢•´®Á•≠®• ™Æ¨Ø•≠·†Ê®® „¢•´®Á®¢†•‚ Á†·‚Æ‚„ ™†§‡Æ¢, ≠Æ ‚Æ‡¨Æß®‚"
 'LOCATE 23, 11: PRINT "‡®·Æ¢†≠®• ¢ß‡Î¢Æ¢, ¢ÎÂ´ÆØ¢ §¢®£†‚•´•© ® Ø•‡•§¢®¶•≠®• °†‡Ï•‡Æ¢"

 action = 0
 DrawShip
 h$ = INKEY$

 SELECT CASE UCASE$(h$)
    CASE CHR$(0) + "K": action = 2' Left
    CASE CHR$(0) + "M": action = 1' Right
    'CASE " ", CHR$(13): action = 1
    CASE CHR$(13), " ": IF Roww = 17 THEN CLS : GOTO Introduction
    CASE CHR$(0) + "H": Roww = Roww - 1
                   IF Roww < 1 THEN Roww = 17
    CASE CHR$(0) + "P": Roww = Roww + 1
                   IF Roww > 17 THEN Roww = 1
    CASE "S", "s": SaveCFG
 END SELECT

 'GOSUB DoStars
LOOP


Controls:
CLS
DO

' COLOR 14: LOCATE 5, 34: PRINT "C O N T R O L S"
' COLOR 8: LOCATE 7, 25:           PRINT "Player 1   Player 2   Player 3   Player 4"
' COLOR 7: LOCATE 9, 15:  PRINT "THRUST     8           W           I           T"
' LOCATE 11, 15:          PRINT "LEFT       4           A           J           F"
' LOCATE 13, 15:          PRINT "RIGHT      6           D           L           H"
' LOCATE 15, 15:          PRINT "WEAPON     5           S           K           G"
' LOCATE 17, 15:          PRINT "SHIELD     2           X           ,           B"
' LOCATE 20, 27:          PRINT "Press Q to Quit, P to Pause"
' LOCATE 22, 30:          PRINT "Press A Key To Return"


 COLOR 14: LOCATE 5, 34: PRINT "ì è ê Ä Ç ã Ö ç à Ö"
 COLOR 8: LOCATE 7, 25: PRINT "Player 1   Player 2   Player 3   Player 4"
 COLOR 7
 LOCATE 9, 15:  PRINT "ì·™Æ‡•≠®•   8           W           I           T"
 LOCATE 11, 15: PRINT "Ç´•¢Æ       4           A           J           F"
 LOCATE 13, 15: PRINT "ÇØ‡†¢Æ      6           D           L           H"
 LOCATE 15, 15: PRINT "é‡„¶®•      5           S           K           G"
 LOCATE 17, 15: PRINT "ô®‚         2           X           ,           B"
 LOCATE 20, 27: PRINT "ç†¶¨®‚• Q §´Ô ¢ÎÂÆ§†, P §´Ô Ø†„ßÎ"
 LOCATE 22, 28: PRINT "ç†¶¨®‚• ™´†¢®Ë„ §´Ô ¢Æß¢‡†È•≠®Ô"


 'GOSUB DoStars
 IF INKEY$ <> "" THEN CLS : GOTO Introduction
LOOP

DoStars:
 IF ColUp = 1 AND rgb < 62 THEN rgb = rgb + 1 ELSE ColUp = -1
 IF ColUp = -1 AND rgb > 5 THEN rgb = rgb - 1 ELSE ColUp = 1
'PALETTE 1, (65536 * rgb) + (256 * rgb) + rgb

 StarField

RETURN

Scroll:
Delay 200
COLOR Col: LOCATE yy, xx: PRINT MID$(Word$, B, Wide)
IF Length - B < Wide THEN
   LOCATE yy, xx + (Length - B): PRINT LEFT$(Word$, Wide - (Length - B))
END IF
RETURN

DrawMayhemLogo:
  DrawBSVimage 0, 0, "data\mayhem.mz1"
  DrawBSVimage 0, 175, "data\mayhem.mz2"
  DrawBSVimage 320, 0, "data\mayhem.mz3"
  DrawBSVimage 320, 175, "data\mayhem.mz4"

  DrawTXTimage 480, 330, "data\version.txt"
  DrawTXTimage 590, 330, "data\ver.txt"
RETURN

SetupGame:
DO
  action = 2
  c = c + 1: IF c > 100 THEN c = 1
  IF c = 1 THEN B = B + 1: IF B > Length THEN B = 1
  G$ = INKEY$
   SELECT CASE UCASE$(G$)
        'CASE "S", "õ": CLS : Setup: CLS : GOTO Introduction
        CASE CHR$(0) + "H": Row = Row - 1: IF Row < 1 THEN Row = 7
        CASE CHR$(0) + "P": Row = Row + 1: IF Row > 7 THEN Row = 1
        CASE CHR$(0) + "K": ' Left
          SELECT CASE Row
            CASE 1:
              GameType = GameType - 1
              IF GameType < 0 THEN GameType = 2
              GOSUB UpdateGameType
            'CASE 2:
            '  IF Plyrs > 2 THEN Plyrs = Plyrs - 1
            'CASE 3:
            '  IF Map > -1 THEN Map = Map - 1
          END SELECT
        CASE CHR$(0) + "M": ' Right
          SELECT CASE Row
            CASE 1:
              GameType = GameType + 1
              IF GameType > 0 THEN GameType = 0
              GOSUB UpdateGameType
            'CASE 2:
            '  IF Plyrs < MaxPlayers THEN Plyrs = Plyrs + 1
            'CASE 3:
            '  IF Map < Maps THEN Map = Map + 1
   END SELECT
        CASE " ", CHR$(13): SELECT CASE Row
                        CASE 1:
                                'IF Map = -1 THEN Map = INT(Maps * RND) + 1
                                'Ship(1).PosX = 0
                                'Players = Plyrs
                                'EXIT SUB
                        'CASE 2: Plyrs = Plyrs + 1
                        '        IF Plyrs > 4 THEN Plyrs = 1
                        'CASE 3: Map = Map + 1
                        '        IF Map > Maps THEN Map = -1
                        'CASE 4: GOTO SetupGame
                        'CASE 5: GOTO OptionScreen
                        'CASE 6: GOTO Controls
                        'CASE 7: CALL Quit
                        CASE 7: EXIT DO
                       END SELECT
   END SELECT
PrintScreen3:

   '-9,+26
   IF Row = 1 THEN COLOR 14 ELSE COLOR 7
   LOCATE 3, 61: PRINT "íàè àÉêõ : "; GameT$
   IF Row = 2 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT " àÉêéäà : "; Plyrs
   IF Row = 3 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 60: IF Map >= 0 THEN PRINT "  äÄêíÄ : "; Map; "    " ELSE PRINT "äÄêíÄ : ë´„Á†©"
   IF Row = 4 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 60: PRINT "çÄëíêéâäÄ àÉêõ"
   IF Row = 5 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT "   éèñàà "
   IF Row = 6 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT " ìèêÄÇãÖçàÖ"
   IF Row = 7 THEN COLOR 14 ELSE COLOR 7
   LOCATE , 61: PRINT "   çÄáÄÑ"

   IF c = 1 THEN GOSUB Scroll
LOOP
RETURN

UpdateGameType:
  SELECT CASE GameType
    CASE 0
      GameT$ = "DeathMatch"
    CASE 1
      GameT$ = "Lives Comp"
    CASE 2
      GameT$ = "Team  Play"
  END SELECT
RETURN

Fin:
END SUB

DEFSNG A-Z
SUB Landing (p AS INTEGER)
DEFINT A-Z

IF Ship(p).AddY < .1 THEN GOTO Repair
ClearShip p
Ship(p).PosX = Ship(p).PosX - (Ship(p).AddY)
Ship(p).AddY = Ship(p).AddY / -2
Ship(p).Addx = Ship(p).Addx / 2
EXIT SUB

Repair:
Player(p).Dmg = 0
Player(p).FuelLft = 100
Player(p).ShotsLft = ShotLoad
Ship(p).Landed = 1
PrintStats p, 1
Ship(p).AddY = 0
Ship(p).Addx = 0

END SUB

DEFSNG A-Z
SUB LoadCFG
DEFINT A-Z

  OPEN "mayhem.cfg" FOR INPUT AS #1

  WHILE NOT EOF(1)
    LINE INPUT #1, name$

    IF (NOT MID$(name$, 1, 1) = "#") AND (NOT MID$(name$, 1, 1) = " " AND (name$ <> "")) THEN
       
      V = VAL(MID$(name$, INSTR(name$, "=") + 1, LEN(name$) - INSTR(name$, "=")))
      name$ = LTRIM$(RTRIM$(MID$(name$, 1, INSTR(name$, "=") - 1)))

      'PRINT name$, v
      'inp$ = INPUT$(1)

      SELECT CASE UCASE$(name$)
        CASE "LIVES": Life = V
        CASE "LASALL": LasersAllowed = V:
        CASE "AMMO": ShotLoad = V
        CASE "COMPENS": Compensation = V
        CASE "FUNCOLL": FunCol = V
        CASE "FLARE": Flare = V
        CASE "ROCKCOL": RockCol = V
        CASE "ROCKDAM": Damagable = V
        CASE "FUNCOFF": ColCoff = V
        CASE "MAXDAM": MaxDamage = V
        CASE "BITS": Bits = V:
        CASE "BITLIFE": BitLife = V
        CASE "SHLDON": Shield = V
        CASE "SNDON": Noise = V
        CASE "POSTMAP": PostProccessMap = V
        CASE "TURNS": Turns = V
        CASE "STARS": Stars = V
        CASE "LIGHTNING": Lightning = V
        CASE "DQBENABLED": DQBenabled = V
        CASE "SNDENABLED": SNDenabled = V
        CASE "VSYNC": VSync = V
        CASE "GRAVITY": Gravity = V
        CASE "SMASS":   ShipMass = V
        CASE "BMASS":   BulletMass = V
        CASE "DEBRIS": MaxDebris = V:
        CASE "CDELAY": CONTROLDELAY = V
        CASE "COLDAM": CollisionDamage = V
       END SELECT

      IF MID$(UCASE$(name$), 1, 2) = "PL" THEN
        Plyr = VAL(MID$(name$, 3, 1))
        SELECT CASE UCASE$(MID$(name$, 4, LEN(name$) - 3))
          CASE "COL"
            Ship(Plyr).Col = V
          CASE "SHIP"
            ShipChoice(Plyr) = V
          CASE "TYPE"
            Player(Plyr).AI = V

          '
          ' Controls
          '
          CASE "KEYTHRUST"
            Ship(Plyr).KEYUP = V
          CASE "KEYFIRE"
            Ship(Plyr).KEYDOWN = V
          CASE "KEYLEFT"
            Ship(Plyr).KEYLEFT = V
          CASE "KEYRIGHT"
            Ship(Plyr).KEYRIGHT = V
          CASE "KEYSHIELD"
            Ship(Plyr).KEYSHIELD = V
        END SELECT
      END IF
    END IF
  WEND

  CLOSE #1

END SUB

DEFSNG A-Z
SUB MoveShip (p AS INTEGER)
DEFINT A-Z

 IF Ship(p).UP THEN
   Ship(p).UP = 0

   Thrust p
 END IF

 IF Ship(p).LEFT THEN
   Ship(p).LEFT = 0

   IF Player(Plyr).Lives >= 0 AND Ship(Plyr).Landed = 0 THEN TurnShip 1, p
 END IF

 IF Ship(p).RIGHT THEN
   Ship(p).RIGHT = 0

   IF Player(Plyr).Lives >= 0 AND Ship(Plyr).Landed = 0 THEN TurnShip -1, p
 END IF

 IF Ship(p).DOWN THEN
   Ship(p).DOWN = 0

   FireShot p
 END IF

 IF Ship(p).Shield THEN
   Ship(p).Shield = 0

   IF Shield THEN
     IF Player(p).Lives >= 0 AND Ship(p).Col < 15 THEN
       ShieldTime(p) = TIMER + 1.5
       Ship(p).Col = 15
       PrintStats Plyr, 1
     END IF
   END IF
 END IF

 IF Ship(p).Landed = 0 THEN
   Ship(p).PosX = Ship(p).PosX + Ship(p).Addx
   Ship(p).AddY = Ship(p).AddY + Gravity
   Ship(p).PosY = Ship(p).PosY + Ship(p).AddY
 END IF

END SUB

DEFSNG A-Z
FUNCTION MULTIKEY% (t AS INTEGER)
DEFINT A-Z

STATIC kbcontrol%(), kbmatrix%(), FirstTime, StatusFlag

IF MKFirstTime = 0 THEN          'Initalize
 REDIM kbcontrol%(128)
 REDIM kbmatrix%(128)
 code$ = ""
 code$ = code$ + "E91D00E93C00000000000000000000000000000000000000000000000000"
 code$ = code$ + "00001E31C08ED8BE24000E07BF1400FCA5A58CC38EC0BF2400B85600FAAB"
 code$ = code$ + "89D8ABFB1FCB1E31C08EC0BF2400BE14000E1FFCFAA5A5FB1FCBFB9C5053"
 code$ = code$ + "51521E560657E460B401A8807404B400247FD0E088C3B700B0002E031E12"
 code$ = code$ + "002E8E1E100086E08907E4610C82E661247FE661B020E6205F075E1F5A59"
 code$ = code$ + "5B589DCF"
 DEF SEG = VARSEG(kbcontrol%(0))
 FOR i% = 0 TO 155                     ' Load ASM
     d% = VAL("&h" + MID$(code$, i% * 2 + 1, 2))
     POKE VARPTR(kbcontrol%(0)) + i%, d%
 NEXT i%
 i& = 16       ' I think this stuff connects the interrupt with kbmatrix%()
 n& = VARSEG(kbmatrix%(0)): l& = n& AND 255: h& = ((n& AND &HFF00) \ 256): POKE i&, l&: POKE i& + 1, h&: i& = i& + 2
 n& = VARPTR(kbmatrix%(0)): l& = n& AND 255: h& = ((n& AND &HFF00) \ 256): POKE i&, l&: POKE i& + 1, h&: i& = i& + 2
 DEF SEG
 MKFirstTime = 1
END IF

SELECT CASE t
 CASE -1
  IF StatusFlag = 0 THEN
   DEF SEG = VARSEG(kbcontrol%(0))
   CALL ABSOLUTE(0)                     ' Run interrupt
   DEF SEG
   StatusFlag = 1
  END IF
 CASE -2
  IF StatusFlag = 1 THEN
   DEF SEG = VARSEG(kbcontrol%(0))      ' Turn off interrupt
   CALL ABSOLUTE(3)
   DEF SEG
   StatusFlag = 0
   'MKFirstTime = 0
  END IF
 CASE 1 TO 128
  MULTIKEY = kbmatrix%(t)               ' Return status
 CASE ELSE
  MULTIKEY = 0                          ' User Stupidity Error
END SELECT

END FUNCTION

DEFSNG A-Z
SUB one
END SUB

SUB PlayGame
DEFINT A-Z
 
PrepareForGame

QuitGame = 0
WHILE QuitGame = 0

   IF VSync THEN WAIT &H3DA, 8

   DrawShip

   IF Restart = 1 THEN Restart = 0: EXIT SUB

   Now = Now + 1
 
   IF Now > Compensation THEN Now = 1
 
   'u$ = INKEY$

   IF FrameTime = 0 THEN FrameTime = TIMER + 1: TimeCount = 0
   IF TIMER - FrameTime > 0 THEN LOCATE 2, 38: COLOR 14: PRINT USING "fps:###"; TimeCount: FrameTime = 0
   TimeCount = TimeCount + 1
 
   IF Damagable = 1 AND DebrisNum > 0 THEN
     DoDebris
   END IF

   FOR Plyr = 1 TO Players

     GetAction Plyr
   
     IF Ship(Plyr).Refresh = 1 AND Player(Plyr).Lives >= 0 THEN RefreshPlayer Plyr
   
     IF Ship(Plyr).Exploded = 0 AND Player(Plyr).Lives >= 0 THEN
       CheckWalls Plyr
       MoveShip Plyr
     ELSEIF Ship(Plyr).Exploded > 0 AND Now = Compensation THEN Explosion
   
     END IF
   
     IF ShieldTime(Plyr) - TIMER <= 0 AND ShieldTime(Plyr) > 0 THEN
        ShieldTime(Plyr) = 0
        Ship(Plyr).Col = Ship(Plyr).DefaultCol
        PrintStats Plyr, 1
     END IF

     InitFlare Plyr
     IF Ship(Plyr).Fired > 0 THEN DoLaser
 
   NEXT Plyr

 IF MULTIKEY(1) THEN                                          'QUIT
   LOCATE 1, 1
   PRINT "                                                                                "
   LOCATE 1, 1
   SELECT CASE INT(RND * 3)
     CASE 0: mes$ = "ÇÎ ¢ ·¢Æ•¨ „¨•"
     CASE 1: mes$ = "ç•„¶•´® ¢Î ÂÆ‚®‚• ·§•´†‚Ï Ì‚Æ"
     CASE 2: mes$ = "îÆ‡¨†‚®‡Æ¢†‚Ï ¶•·‚™®© §®·™"
     CASE 3: mes$ = "ÑÆ·‚†‚ÆÁ≠Æ ‰‡†£Æ¢ ≠† ·•£Æ§≠Ô"
   END SELECT
   mes$ = mes$ + "(Ñ†-Y/ç•‚-N) "
   PRINT mes$

   WHILE (MULTIKEY(1))
   WEND

   IF MULTIKEY(21) THEN
     FOR Reinit = 1 TO Players
       Player(Reinit).Lives = 9
       Player(Reinit).Dead = 0
       Ship(Reinit).Col = Ship(Reinit).DefaultCol
     NEXT Reinit
     QuitGame = 1              'Quit current game
     d = MULTIKEY(-2)
   ELSE
     LOCATE 1, 1: PRINT "                                                                                "
     FOR i = 1 TO Players
       PrintStats i, 0
     NEXT
   END IF
 END IF

 IF MULTIKEY(25) THEN                                         'PAUSE
    FOR i = 1 TO Players
      PrintStats i, 15
    NEXT

    COLOR 14: LOCATE 13, 30: PRINT "P A U S E D - Press A Key"
    FOR i = 0 TO 2500
      WaitAnyKey
    NEXT
    ShowScreen
 END IF

WEND

ERASE Lines
ERASE Curves
ERASE Paints
ERASE Landspots

'EXIT SUB

END SUB

SUB PrepareForGame
 
GetMapData

FOR l = 1 TO MaxLandspots
  Landspots(l).Landed = 0
  'Landed(l) = 0
NEXT

FOR yy = 1 TO Players'4
  Ship(yy).DefaultCol = Ship(yy).Col
  Player(yy).ShotsTkn = 0
  Player(yy).ShotsHit = 0
  Player(yy).Kills = 0
  Player(yy).Frags = 0
  Player(yy).Crash = 0
  Plyr = yy
  InitPlayer yy, 0
  RotateShip Plyr
  Ship(yy).Refresh = 1

  Ship(yy).UP = 0
  Ship(yy).LEFT = 0
  Ship(yy).RIGHT = 0
  Ship(yy).DOWN = 0
  Ship(yy).Shield = 0
NEXT yy

SCREEN 9
CLS
PRINT "èÆ§£Æ‚†¢´®¢†•‚·Ô ™†‡‚†..."
SCREEN 9, , 1, 0: CLS

ExplosionTime = INT((BitLife + 5) / Compensation)

DrawMap (RockCol)

FOR i = 0 TO MaxPaints
  PAINT (Paints(i).x, Paints(i).y), RockCol
NEXT

IF PostProccessMap THEN
  FOR x = 0 TO 640
    FOR y = 30 TO 350
      IF CINT(RND * 1) THEN
        IF POINT(x, y) THEN PSET (x, y), 8
      END IF
    NEXT
  NEXT
END IF

DrawMap (8)

COLOR 14: LOCATE 1, 40: PRINT Map

SCREEN 9, , 1, 1
'PRINT "ÇÎ¢Æ§®‚·Ô ≠† Ì™‡†≠..."
'FOR i = 0 TO 100: NEXT
'CLS

FOR c = 1 TO Players
  'Plyr = c
  IF Player(c).Dead > 0 AND Ship(c).Exploded >= (BitLife - 5 - Compensation) / Compensation THEN
    InitPlayer c, 1
    RotateShip c
    EXIT FOR
  END IF
  PrintStats c, 0
NEXT c

PCOPY 1, 0

MKFirstTime = 0
d = MULTIKEY(-1)

END SUB

DEFSNG A-Z
SUB PrintStats (n AS INTEGER, R AS INTEGER)
DEFINT A-Z

  'SELECT CASE Plyr
  SELECT CASE n
    CASE 1: Across = 1: DOWN = 1
    CASE 2: Across = 45: DOWN = 1
    CASE 3: Across = 1: DOWN = 2
    CASE 4: Across = 45: DOWN = 2
  END SELECT
   
  COLOR Ship(n).Col
  IF R = 1 THEN
    IF Player(n).Lives >= 0 THEN
      LOCATE DOWN, Across + 6: PRINT USING "##"; Player(n).Lives
      LOCATE DOWN, Across + 13: PRINT USING "###"; Player(n).Dmg / MaxDamage * 100
      LOCATE DOWN, Across + 23: PRINT USING "###"; Player(n).ShotsLft
      LOCATE DOWN, Across + 31: PRINT USING "###"; Player(n).FuelLft
    ELSEIF Player(n).Lives < 0 THEN
      LOCATE DOWN, Across: COLOR Ship(n).Col: PRINT "    ******* Player Dead *******     "
    END IF
  ELSEIF R = 0 THEN
      LOCATE DOWN, Across: PRINT "Lives:__ Dmg:___% Ammo:___Fuel:___%"
      PrintStats n, 1

      'LOCATE Down, Across: COLOR Ship(n).Col: PRINT "Lives:"; Player(n).Lives
      'LOCATE Down, Across + 9: PRINT USING "Dmg:###%"; Player(n).Dmg / MaxDamage * 100
      'LOCATE Down, Across + 18: PRINT "Ammo:"; Player(n).ShotsLft
      'LOCATE Down, Across + 27: PRINT USING "Fuel:###%"; Player(n).FuelLft
  ELSE
      LOCATE DOWN, Across: PRINT "  Frags:                            "
      LOCATE DOWN, Across + 8: PRINT USING "###"; Player(n).Frags
  END IF

END SUB

DEFSNG A-Z
SUB Quit

SCREEN 0


SaveCFG

'PRINT "         €€ª  €ª €€€€€ª€€ª  €ª€€ª  €ª€€€€€ €€ª  €ª"
'PRINT "         €€€ª€€∫€€   €∫ €€ª€…º€€∫  €∫€€    €€€ª€€∫"
'PRINT "         €€∫€ €∫€€€€€€∫  €€…º €€€€€€∫€€€€  €€∫€ €∫"
'PRINT "         €€∫  €∫€€…ÕÕ€∫  €€∫  €€   €∫€€    €€∫  €∫"
'PRINT "         €€∫  €∫€€∫  €∫  €€∫  €€∫  €∫€€€€€ €€∫  €∫"
'PRINT "         »Õº               º                      "

'PRINT "                        €ª  €ª€€€€ª€ª  €ª€ª  €ª€€€€€ª€ª  €ª"
'PRINT "                        €€ª€€∫€…Õ€∫ €ª€…º€∫  €∫€…ÕÕÕº€€ª€€∫"
'PRINT "                        €…€∫€∫€€€€∫  €…º €€€€€∫€€€ª  €…€∫€∫"
'PRINT "                        €∫»º€∫€…Õ€∫  €∫  €…ÕÕ€∫€…Õº  €∫»º€∫"
'PRINT "                        €∫  €∫€∫ €∫  €∫  €∫  €∫€€€€€ª€∫  €∫"
'PRINT "                        »º  »º»º »º  »º  »º  »º»ÕÕÕÕº»º  »º"

'                   €€   €  €€€€€ €€   € €€   € €€€€€ €€   €                 '
'                   €€€ €€ €€   €  €€ €  €€   € €€    €€€ €€                 '
'                   €€ € € €€€€€€   €€   €€€€€€ €€€€  €€ € €                 '
'                   €€   € €€   €   €€   €€   € €€    €€   €                 '
'                   €€   € €€   €   €€   €€   € €€€€€ €€   €                 '

CentrePRINT "ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ", 12, 0, 0, 2
CentrePRINT "€€   €  €€€€€ €€   € €€   € €€€€€ €€   €", 7, 0, 0, 4
CentrePRINT "€€€ €€ €€   €  €€ €  €€   € €€    €€€ €€", 7, 0, 0, 0
CentrePRINT "€€ € € €€€€€€   €€   €€€€€€ €€€€  €€ € €", 7, 0, 0, 0
CentrePRINT "€€   € €€   €   €€   €€   € €€    €€   €", 7, 0, 0, 0
CentrePRINT "€€   € €€   €   €€   €€   € €€€€€ €€   €", 7, 0, 0, 0
CentrePRINT "ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ", 12, 0, 0, 10
CentrePRINT "…ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕª", 7, 0, 0, 12
CentrePRINT "∫                                                           ∫", 7, 0, 0, 0
CentrePRINT "∫                                                           ∫", 7, 0, 0, 0
CentrePRINT "∫                                                           ∫", 7, 0, 0, 0
CentrePRINT "»ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº", 7, 0, 0, 0
CentrePRINT "Ä Ç í é ê õ:", 14, 0, 0, 13
CentrePRINT "éëçéÇçÄü àÑÖü: Alex Smith (wedge@btinternet.com)", 15, 0, 0, 0
CentrePRINT "ÑéêÄÅéíäÄ à åçéÉé óÖÉé ÖôÖ: MAZsoft (mailfrom@mail.ru)", 15, 0, 0, 0
CentrePRINT "∞±≤ ÑÆ ë¢®§†≠®Ô! ≤±∞", 11, 1, 0, 23

END
END SUB

SUB RefreshPlayer (who AS INTEGER)
DEFINT A-Z

max = 9

Toner = SGN(8 - Ship(who).Col) * 8
IF Ship(who).RefreshSleep = 0 THEN
  Ship(who).RefreshTime = Ship(who).RefreshTime + 1
  IF Lightning THEN
    FOR i = 0 TO 2
      DrawLightning Landspots(Ship(who).LanPos - 1).x + 13, Landspots(Ship(who).LanPos - 1).y + 13, RND * 20 + 10, RND * 360
      'DrawLightning ShipX(Ship(who).LanPos) + 13, ShipY(Ship(who).LanPos) + 13, RND * 20 + 10, RND * 360
    NEXT
  END IF
  'ELSE
    CIRCLE (Landspots(Ship(who).LanPos - 1).x + 13, Landspots(Ship(who).LanPos - 1).y + 13), Ship(who).RefreshTime - 1, 0
    CIRCLE (Landspots(Ship(who).LanPos - 1).x + 13, Landspots(Ship(who).LanPos - 1).y + 13), max + 1 - Ship(who).RefreshTime, 0
    'CIRCLE (ShipX(Ship(who).LanPos) + 13, ShipY(Ship(who).LanPos) + 13), Ship(who).RefreshTime - 1, 0
    'CIRCLE (ShipX(Ship(who).LanPos) + 13, ShipY(Ship(who).LanPos) + 13), max + 1 - Ship(who).RefreshTime, 0
    IF Ship(who).RefreshTime < max THEN
      CIRCLE (Landspots(Ship(who).LanPos - 1).x + 13, Landspots(Ship(who).LanPos - 1).y + 13), Ship(who).RefreshTime, Ship(who).Col
      CIRCLE (Landspots(Ship(who).LanPos - 1).x + 13, Landspots(Ship(who).LanPos - 1).y + 13), max - Ship(who).RefreshTime, Ship(who).Col + Toner
      'CIRCLE (ShipX(Ship(who).LanPos) + 13, ShipY(Ship(who).LanPos) + 13), Ship(who).RefreshTime, Ship(who).Col
      'CIRCLE (ShipX(Ship(who).LanPos) + 13, ShipY(Ship(who).LanPos) + 13), max - Ship(who).RefreshTime, Ship(who).Col + Toner
    END IF
  'END IF
  IF Ship(who).RefreshTime = max THEN

    ShowScreen            '!!!!

    Ship(who).Refresh = 0
    Ship(who).RefreshTime = 0
    Ship(who).RefreshSleep = 0
  END IF
END IF

IF Ship(who).RefreshTime < max THEN
   Ship(who).RefreshSleep = Ship(who).RefreshSleep + 1
   IF Ship(who).RefreshSleep = 4 THEN Ship(who).RefreshSleep = 0
END IF

END SUB

DEFSNG A-Z
SUB RotateShip (p AS INTEGER)
DEFINT A-Z

'w = P
FOR a = 1 TO Points(p)
  x(a, p) = (Size(a, 2, p) * Sine(Ship(p).Angle)) + (Size(a, 1, p) * Cosine(Ship(p).Angle)) + 13 + Ship(p).PosX
  y(a, p) = (Size(a, 2, p) * Cosine(Ship(p).Angle)) - (Size(a, 1, p) * Sine(Ship(p).Angle)) + 13 + Ship(p).PosY
NEXT a

END SUB

DEFSNG A-Z
SUB SaveCFG
DEFINT A-Z
 
  DIM name$(150)', v(100)
  c = 0

  OPEN "mayhem.cfg" FOR INPUT AS #1

  WHILE NOT EOF(1)
    LINE INPUT #1, name$(c)
    c = c + 1
  WEND
  cf = c
  CLOSE #1

  OPEN "mayhem.cfg" FOR OUTPUT AS #1

    c = 0
    WHILE c < cf
      IF (NOT MID$(name$(c), 1, 1) = "#") AND (NOT MID$(name$(c), 1, 1) = " ") AND (name$ <> "") THEN
      
        V = VAL(MID$(name$, INSTR(name$, "=") + 1, LEN(name$) - INSTR(name$, "=")))
        name$(c) = LTRIM$(RTRIM$(MID$(name$(c), 1, INSTR(name$(c), "=") - 1)))

        'PRINT name$, v
        'x$ = INPUT$(1)

        SELECT CASE UCASE$(name$(c))
          CASE "LIVES":   V = Life
          CASE "LASALL":  V = LasersAllowed
          CASE "AMMO":    V = ShotLoad
          CASE "COMPENS": V = Compensation
          CASE "FUNCOLL": V = FunCol
          CASE "FLARE":   V = Flare
          CASE "ROCKCOL": V = RockCol
          CASE "ROCKDAM": V = Damagable
          CASE "FUNCOFF": V = ColCoff
          CASE "MAXDAM":  V = MaxDamage
          CASE "BITS":    V = Bits
          CASE "BITLIFE": V = BitLife
          CASE "SHLDON":  V = Shield
          CASE "LIGHTNING": V = Lightning
          CASE "DQBENABLED": V = DQBenabled
          CASE "SNDENABLED": V = SNDenabled
          CASE "VSYNC":   V = VSync
          CASE "GRAVITY": V = Gravity
          CASE "SMASS":   V = ShipMass
          CASE "BMASS":   V = BulletMass
          CASE "SNDON":
          V = Noise
          CASE "TURNS":
          V = Turns
          CASE "STARS":
          V = Stars
          CASE "POSTMAP": V = PostProccessMap
        END SELECT

        IF MID$(UCASE$(name$(c)), 1, 2) = "PL" THEN
          Plyr = VAL(MID$(name$(c), 3, 1))
          SELECT CASE UCASE$(MID$(name$(c), 4, LEN(name$(c)) - 3))
            CASE "COL"
              V = Ship(Plyr).Col
            CASE "SHIP"
              V = ShipChoice(Plyr)
            CASE "TYPE"
              V = Player(Plyr).AI
          END SELECT
        END IF

        name$(c) = name$(c) + "=" + LTRIM$(RTRIM$(STR$(V)))
      END IF

      PRINT #1, name$(c)
      c = c + 1

    WEND
  CLOSE #1

END SUB

DEFSNG A-Z
SUB Setup
DEFINT A-Z
 
  PRINT "/\/\/\/\/\/\/\/\/\/\/\/\/ ÇõëíêÖãõ \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\"
  INPUT "äÆ´-¢Æ ¢Î·‚‡•´Æ¢      ( 5 ) = "; LasersAllowed
  INPUT "ë™Æ‡Æ·‚Ï ¢Î·‚‡•´†     ( 4 ) = "; LaserSpeed
  INPUT "Ñ´®≠† ¢Î·‚‡•´†        ( 2 ) = "; ShotLenght
  INPUT "äÆ´-¢Æ ¢Î·‚‡•´Æ¢      (50 ) = "; ShotLoad
  PRINT
  PRINT "/\/\/\/\/\/\/\/\/\/\/\/\/   àÉêÄ   \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\"
  INPUT "äÆ¨Ø•≠·†Ê®Ô           ( 5 ) = "; Compensation
  'INPUT "ÇÎÂ´ÆØÎ §¢®£†‚•´Ô     ( 1 ) = "; Flare
  INPUT "ñ¢•‚ ·™†´             ( 7 ) = "; RockCol
  'INPUT "èÆ¢‡•¶§. ·™†´         ( 1 ) = "; Damagable
  INPUT "Ç•·•´Î• ·‚Æ´™≠Æ¢•≠®Ô  ( 0 ) = "; FunCol
  INPUT "äÆÌ‰. ·‚Æ´™≠Æ¢•≠®©    (.7 ) = "; ColCoff
  INPUT "Çß‡Î¢ ¢·•Â ®£‡Æ™Æ¢    ( 0 ) = "; FunExplosion

  PRINT
  PRINT "/\/\/\/\/\/\/\/\/\/\/\/\/  àÉêéäà  \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\"
  INPUT "äÆ´-¢Æ ®£‡Æ™Æ¢        ( 4 ) = "; Players
  INPUT "å†™·. ØÆ¢‡•¶§•≠®Ô     ( 3 ) = "; MaxDamage
  INPUT "Ç™´. ô®‚              ( 1 ) = "; Shield

  PRINT
  PRINT "/\/\/\/\/\/\/\/\/\/\/\/\/   ????   \/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\"
  INPUT "Ç‡•¨Ô ¢Æ··‚.(¢ ‚†™‚†Â)(500) = "; BitLife
  INPUT "íÆÁ™® ØÆ·´• ·¨•‡‚®    (15 ) = "; Bits
  INPUT "LasersLastTime -- ??? (200) = "; LasersLastTime

  IF LasersAllowed = 0 THEN LasersAllowed = 5
  IF LaserSpeed = 0 THEN LaserSpeed = 4
  IF ShotLenght = 0 THEN ShotLenght = 2
  IF ShotLoad = 0 THEN ShotLoad = 50

  IF Compensation = 0 THEN Compensation = 5
  IF RockCol = 0 THEN RockCol = 7

  IF Players = 0 THEN Players = 4
  IF MaxDamage = 0 THEN MaxDamage = 1
  IF ColCoff = 0 THEN ColCoff = .7

  IF BitLife = 0 THEN BitLife = 500
  IF Bits = 0 THEN Bits = 15
  IF LasersLastTime = 0 THEN LasersLastTime = 200

END SUB

DEFSNG A-Z
SUB ShowScreen
DEFINT A-Z

PCOPY 0, 1

FOR i = 1 TO Players
  PrintStats i, 1
NEXT

END SUB

DEFSNG A-Z
SUB StarField
DEFINT A-Z

Speed = 1000: Sizes = 25: MiddleX = 320: MiddleY = 240

FOR a = 1 TO Stars

  FOR d = 1 TO CINT(Sizes / StarZ(a)) + 1
    FOR e = 1 TO CINT(Sizes / StarZ(a)) + 1
      xx = ((StarX(a) / StarZ(a)) * Speed) + MiddleX - 1 + d
      yy = ((StarY(a) / StarZ(a)) * Speed) + MiddleY - 1 + e
      IF POINT(xx, yy) <> 14 THEN PRESET (xx, yy), 0
    NEXT e
  NEXT d

  IF StarZ(a) < 120 THEN StarCol(a) = 7
  IF StarZ(a) < 40 THEN StarCol(a) = 15

  StarZ(a) = StarZ(a) - 1
  x = ((StarX(a) / StarZ(a)) * Speed) + MiddleX
  y = ((StarY(a) / StarZ(a)) * Speed) + MiddleY
  IF StarZ(a) < 2 THEN GOSUB SpawnNewStar
  IF y < -2 OR x < -2 OR x > 641 OR y > 481 THEN GOSUB SpawnNewStar
  FOR d = 1 TO CINT(Sizes / StarZ(a)) + 1
    FOR e = 1 TO CINT(Sizes / StarZ(a)) + 1
      IF POINT(x - 1 + d, y - 1 + e) <> 14 THEN PSET (x - 1 + d, y - 1 + e), StarCol(a)
    NEXT e
  NEXT d
NEXT a

EXIT SUB

SpawnNewStar:
  StarX(a) = (-80 * RND) + 40
  StarY(a) = (-80 * RND) + 40
  StarCol(a) = 8
  StarZ(a) = INT(RND * 350) + 50
RETURN


END SUB

DEFSNG A-Z
FUNCTION StartGame%
DEFINT A-Z

Row = 1: Plyr = 1

COLOR 7
LOCATE 2, 54: PRINT "…ÕÕ[                 ]ÕÕª"
LOCATE 3, 54: PRINT "∫                       ∫"
LOCATE 4, 54: PRINT "∫                       ∫"
LOCATE 5, 54: PRINT "∫                       ∫"
LOCATE 6, 54: PRINT "∫                       ∫"
LOCATE 7, 54: PRINT "∫                       ∫"
LOCATE 8, 54: PRINT "∫                       ∫"
LOCATE 9, 54: PRINT "»ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº"

'COLOR 12
'LOCATE 2, 59: PRINT "çÄëíêéâäÄ  àÉêõ"
CentrePRINT "çÄëíêéâäÄ  àÉêõ", 12, 0, 66, 2

' CONST G.LIFE = 0                    'Life competition
'CONST G.DEATH = 1                    'Deathmatch with frag limit
' CONST G.TEAM = 3                    'Teamplay with frag limit
'CONST G.FRAGS = 0
' CONST G.TIME = 1
'TYPE GameType
'        Mode AS INTEGER
'     EndCond AS INTEGER
'    Quantity AS INTEGER
'END TYPE

DO
  action = 2
  c = c + 1: IF c > 7 THEN c = 1
  IF c = 1 THEN B = B + 1: IF B > Length THEN B = 1
  G$ = INKEY$
   SELECT CASE UCASE$(G$)
        'CASE "S", "õ": CLS : Setup: CLS : GOTO Introduction
        CASE CHR$(0) + "H": Row = Row - 1: IF Row < 1 THEN Row = 6
        CASE CHR$(0) + "P": Row = Row + 1: IF Row > 6 THEN Row = 1
        CASE CHR$(0) + "K": ' Left
          SELECT CASE Row
            CASE 2: IF Game.Mode < G.TEAM THEN Game.Mode = Game.Mode + 1
            CASE 3: IF Game.EndCond < G.TIME THEN Game.EndCond = Game.EndCond + 1
            CASE 4: IF Game.Quantity < 30 THEN Game.Quantity = Game.Quantity + 1
            CASE 5: IF Plyr < Players THEN Plyr = Plyr + 1
          END SELECT
        CASE CHR$(0) + "M": ' Right
          SELECT CASE Row
            CASE 2: IF Game.Mode > 0 THEN Game.Mode = Game.Mode - 1
            CASE 3: IF Game.EndCond > 0 THEN Game.EndCond = Game.EndCond - 1
            CASE 4: IF Game.Quantity > 1 THEN Game.Quantity = Game.Quantity - 1
            CASE 5: IF Plyr > 0 THEN Plyr = Plyr - 1
   END SELECT
        CASE " ", CHR$(13): SELECT CASE Row
                        CASE 1: StartGame = 1: EXIT FUNCTION
                        CASE 5: GOSUB ConfigurePlayer
                        CASE 6: StartGame = 0: EXIT FUNCTION
                       END SELECT
   END SELECT
PrintScreen2:

   IF Row = 1 THEN c = 14 ELSE c = 7
   CentrePRINT "GO!!!", c, 0, 66, 3
   IF Row = 2 THEN c = 14 ELSE c = 7
   CentrePRINT "íàè àÉêõ: " + gt$, c, 0, 66, 4
   IF Row = 3 THEN c = 14 ELSE c = 7
   CentrePRINT "ìëã.áÄÇÖêò: " + cond$, c, 0, 66, 5
   IF Row = 4 THEN c = 14 ELSE c = 7
   CentrePRINT "äéã-Çé: " + LTRIM$(RTRIM$(STR$(Game.Quantity))), c, 0, 66, 6
   IF Row = 5 THEN c = 14 ELSE c = 7
   CentrePRINT "çÄëíê. àÉêéäÄ: " + LTRIM$(RTRIM$(STR$(Plyr))), c, 0, 66, 7
   IF Row = 6 THEN c = 14 ELSE c = 7
   CentrePRINT "çÄáÄÑ", c, 0, 66, 9

   '-9,+26
   'IF Row = 1 THEN COLOR 14 ELSE COLOR 7
   'LOCATE 3, 61: PRINT "   GO!!!"
   'IF Row = 2 THEN COLOR 14 ELSE COLOR 7
   'LOCATE , 61: PRINT " àÉêéäà : "; Plyrs
   'IF Row = 3 THEN COLOR 14 ELSE COLOR 7
   'LOCATE , 61: IF Map >= 0 THEN PRINT "  äÄêíÄ : "; Map; "    " ELSE PRINT "äÄêíÄ : ë´„Á†©"
   'IF Row = 4 THEN COLOR 14 ELSE COLOR 7
   'LOCATE , 61: PRINT "   éèñàà "
   'IF Row = 5 THEN COLOR 14 ELSE COLOR 7
   'LOCATE , 61: PRINT " ìèêÄÇãÖçàÖ"
   'IF Row = 6 THEN COLOR 14 ELSE COLOR 7
   'LOCATE , 61: PRINT "   çÄáÄÑ"
  
   ' €ª  €ª€€€€ª€ª  €ª€ª  €ª€€€€€ª€ª  €ª
   ' €€ª€€∫€…Õ€∫ €ª€…º€∫  €∫€…ÕÕÕº€€ª€€∫
   ' €…€∫€∫€€€€∫  €…º €€€€€∫€€€ª  €…€∫€∫
   ' €∫»º€∫€…Õ€∫  €∫  €…ÕÕ€∫€…Õº  €∫»º€∫
   ' €∫  €∫€∫ €∫  €∫  €∫  €∫€€€€€ª€∫  €∫
   ' »º  »º»º »º  »º  »º  »º»ÕÕÕÕº»º  »º

   'LOCATE 6, 35: COLOR 14: PRINT "M A Y H E M"
   'COLOR 7: LOCATE 20, 27: PRINT "Num Lock §Æ´¶•≠ °Î‚Ï ¢™´ÓÁ•≠."
   'LOCATE , 20: PRINT "Recommended to run in DOS before Win95 boots"
   'LOCATE 8, 35: COLOR 7: PRINT "Ç•‡·®Ô: "; Version$
   'LOCATE 10, 23: PRINT "àëèéãúáìâíÖ ëíêÖãäà à èêéÅÖã Ñãü ÇõÅéêÄ..."
   'GOSUB DoStars
   'IF c = 1 THEN GOSUB Scroll
LOOP

ConfigurePlayer:

RETURN

END FUNCTION

DEFSNG A-Z
SUB Thrust (p AS INTEGER)
DEFINT A-Z

IF Ship(p).Exploded = 0 AND Ship(p).Refresh = 0 AND Player(p).Lives >= 0 AND Player(p).FuelLft > 0 AND Ship(p).Col < 15 THEN
       Ship(p).Landed = 0
       Landspots(Ship(p).LanPos - 1).Landed = 0
       'Landed(Ship(P).LanPos) = 0
       Player(p).FuelLft = Player(p).FuelLft - 1
       Ship(p).Addx = Ship(p).Addx + (-Sine(Ship(p).Angle) / 4)
       Ship(p).AddY = Ship(p).AddY + (-Cosine(Ship(p).Angle) / 4)
       IF Flare = 1 AND Thrusts = 0 THEN Thrusts = Thrusts + 1: ThrustTime(Thrusts, p) = TIMER + .2: DoFlare Thrusts
       PrintStats p, 1
END IF

END SUB

DEFSNG A-Z
SUB TurnShip (Sign AS INTEGER, p AS INTEGER)
DEFINT A-Z

ClearShip p

Ship(p).Angle = Ship(p).Angle + TurnAngle * Sign

IF Ship(p).Angle >= 360 THEN Ship(p).Angle = 0
IF Ship(p).Angle < 0 THEN Ship(p).Angle = 360 - TurnAngle

RotateShip p
'IF Ship(P).Exploded = 0 THEN CheckWalls

END SUB

SUB WaitAnyKey

  DO
    FOR i = 1 TO 128
      IF MULTIKEY(i) THEN EXIT SUB
    NEXT
  LOOP

END SUB


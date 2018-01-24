'
' лл   л  ллллл лл   л лл   л ллллл лл   л
' ллл лл лл   л  лл л  лл   л лл    ллл лл
' лл л л лллллл   лл   лллллл лллл  лл л л
' лл   л лл   л   лл   лл   л лл    лл   л Include file (c)MAZsoft 1999-2000
' лл   л лл   л   лл   лл   л ллллл лл   л
'
'$DYNAMIC
DEFINT A-Z

'$INCLUDE: 'MayTypes.bi'

'!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!SUBROUTINES!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
DECLARE SUB Quit ()
DECLARE SUB GetAction (P AS INTEGER)
DECLARE SUB AIFindMission (P AS INTEGER)
DECLARE FUNCTION AIFaceTarget% (P AS INTEGER)
DECLARE SUB WaitAnyKey ()
DECLARE SUB Setup ()
DECLARE SUB CentrePRINT (text$, Col AS INTEGER, BCol AS INTEGER, x AS INTEGER, y AS INTEGER)
DECLARE SUB CheckWalls (P AS INTEGER)
DECLARE SUB ClearShip (w AS INTEGER)
DECLARE SUB Continue ()
DECLARE SUB Delay (t AS INTEGER)
DECLARE SUB DoDebris ()
DECLARE SUB DoFlare (Thrusts AS INTEGER)
DECLARE SUB DoLaser ()
DECLARE SUB DrawBSVimage (x AS INTEGER, y AS INTEGER, file$)
DECLARE SUB DrawLightning (x AS INTEGER, y AS INTEGER, L AS INTEGER, ang AS INTEGER)
DECLARE SUB DrawMap (Col AS INTEGER)
DECLARE SUB DrawShip ()
DECLARE SUB DrawTXTimage (x AS INTEGER, y AS INTEGER, file$)
DECLARE SUB Quit ()
DECLARE SUB Explosion ()
DECLARE SUB FireShot (P AS INTEGER)
DECLARE SUB GetMapData ()
DECLARE SUB GetShipData ()
DECLARE SUB Init ()
DECLARE SUB InitDebris (L AS LaserType)
DECLARE SUB InitExplosion (P AS INTEGER)
DECLARE SUB InitFlare (P AS INTEGER)
DECLARE SUB InitLaser (P AS INTEGER)
DECLARE SUB InitPlayer (Plyr AS INTEGER, w AS INTEGER)
DECLARE SUB InitStars ()
DECLARE SUB Intro ()
DECLARE SUB Landing (P AS INTEGER)
DECLARE SUB LoadCFG ()
DECLARE SUB MoveShip (P AS INTEGER)
DECLARE FUNCTION MULTIKEY% (t AS INTEGER)
DECLARE SUB PlayGame ()
DECLARE SUB PrepareForGame ()
DECLARE SUB PrintStats (n AS INTEGER, R AS INTEGER)
DECLARE SUB RefreshPlayer (who AS INTEGER)
DECLARE SUB RotateShip (P AS INTEGER)
DECLARE SUB SaveCFG ()
DECLARE SUB ShowScreen ()
DECLARE SUB StarField ()
DECLARE FUNCTION StartGame% ()
DECLARE SUB Thrust (P AS INTEGER)
DECLARE SUB TurnShip (Sign AS INTEGER, P AS INTEGER)

COMMON SHARED Restart, Gravity AS SINGLE, Map, Players, LaserLastTime, Plyr, MaxDamage
COMMON SHARED Compensation AS SINGLE, Now AS SINGLE, Bits, LasersAllowed, ShotLength, Flare, Thrusts
COMMON SHARED TurnAngle AS SINGLE, BitLife, Barriers, Pricks AS LONG, PaintPoints, DebrisNum
COMMON SHARED Ships, FlareBackCol, Links, Noise, LaserSpeed AS SINGLE, Damagable
COMMON SHARED Stars, ExplosionTime AS SINGLE, RockCol, ShotLoad, MaxDebris, Shield
'COMMON SHARED Stars, ExplosionTime AS SINGLE, RockCol, ShotLoad, Curves, MaxDebris, Shield

DIM SHARED ColCoff AS SINGLE, CollisionDamage AS SINGLE

COMMON SHARED FunCol, FunExplosion, Landings, PostProccessMap, Life, Lightning
COMMON SHARED DQBenabled, SNDenabled, VSync, ShipMass, BulletMass, DEBUG, MKFirstTime, CONTROLDELAY, MaxPlayers

CONST Version$ = "1.96"

CONTROLDELAY = 5
MaxPlayers = 4

 CONST GMLIFE = 0                    'Life competition
CONST GMDEATH = 1                    'Deathmatch with frag limit
 CONST GMTEAM = 3                    'Teamplay with frag limit

CONST GMFRAGS = 0
 CONST GMTIME = 1
CONST GMFRIME = 2                    'Frags & Time

CONST AILOCAL = 0
  CONST AICPU = 1

DIM SHARED FRAGLIMIT, TIMELIMIT

Lightning = 1

VSync = 1

ShipMass = 1000: BulletMass = 1

DEBUG = 0

Gravity = .003                       'Gravity force
LaserSpeed = 4                       'speed of lasers move
BitLife = 500
Landings = 4                         'Number of Ships' landing spots
Flare = 1                            'ogon from the end of the ship
Bits = 50
LaserLastTime = 200
LasersAllowed = 5                    'how many lasers at one time
Players = 4                          '# of players
Compensation = 1                     'compensation speed
TurnAngle = 22.5
ShotLength = 2                       'lenght of laser line
MaxDamage = 3                        'how many hits to blow up the ship
Stars = 100                          '# of stars on title screen
RockCol = 7
ShotLoad = 50                        '# of shots in your ship
MaxDebris = 20
Damagable = 1                        'rock type
Shield = 1
FunCol = 0: ColCoff = .7: FunExplosion = 0
PostProccessMap = 1


IF COMMAND$ <> "" THEN
  Setup
END IF

RANDOMIZE TIMER


CONST MLines = 500, MCurves = 500, MPaints = 100, MLandspots = 100
DIM SHARED Lines(1) AS LineType
DIM SHARED Curves(1) AS CurveType
DIM SHARED Paints(1) AS PaintType
DIM SHARED Landspots(1) AS LandspotType

DIM SHARED MaxLines, MaxCurves, MaxPaints, MaxLandspots

DIM SHARED Game AS GameType
DIM SHARED Ship(Players) AS PlyrShip
DIM SHARED Dot(1, Players) AS Fragments
DIM SHARED Player(Players) AS PlayerType
DIM SHARED Debris(1) AS DebrisType
DIM SHARED Laser(1, Players) AS LaserType

DIM SHARED FirstTime, GameType, Turns

DIM SHARED ThrustTime(20, Players) AS SINGLE, ThrustXL(20, Players), ThrustXR(20, Players), ThrustYU(20, Players), ThrustYD(20, Players)
DIM SHARED Cosine(360) AS SINGLE, Sine(360) AS SINGLE
DIM SHARED Turret(Players)
DIM SHARED x(100, Players) AS SINGLE, y(100, Players)  AS SINGLE
DIM SHARED Points(Players), Links(Players), Engine(Players), ShipTurrets(Players), ShipChoice(Players)
DIM SHARED Link(50, 2, Players), Size(50, 2, Players)   AS SINGLE, ShipTurret(6, Players)
DIM SHARED StarX(Stars) AS SINGLE, StarY(Stars) AS SINGLE, StarZ(Stars) AS SINGLE, StarCol(Stars)
DIM SHARED ShieldTime(Players) AS SINGLE

DIM SHARED FrameTime AS SINGLE, QuitGame

IF Players >= 1 THEN
  Ship(1).KEYUP = 72
  Ship(1).KEYLEFT = 75
  Ship(1).KEYRIGHT = 77
  Ship(1).KEYDOWN = 76
  Ship(1).KEYSHIELD = 80
END IF

IF Players >= 2 THEN
  Ship(2).KEYUP = 17
  Ship(2).KEYLEFT = 30
  Ship(2).KEYRIGHT = 32
  Ship(2).KEYDOWN = 31
  Ship(2).KEYSHIELD = 45
END IF

IF Players >= 3 THEN
  Ship(3).KEYUP = 23
  Ship(3).KEYLEFT = 36
  Ship(3).KEYRIGHT = 38
  Ship(3).KEYDOWN = 37
  Ship(3).KEYSHIELD = 27
END IF

IF Players >= 4 THEN
  Ship(4).KEYUP = 20
  Ship(4).KEYLEFT = 33
  Ship(4).KEYRIGHT = 35
  Ship(4).KEYDOWN = 34
  Ship(4).KEYSHIELD = 48
END IF

IF Players >= 5 THEN
  Ship(5).KEYUP = 26
  Ship(5).KEYLEFT = 39
  Ship(5).KEYRIGHT = 43
  Ship(5).KEYDOWN = 40
  Ship(5).KEYSHIELD = 53
END IF


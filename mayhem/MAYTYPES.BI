TYPE LineType
        x1 AS INTEGER
        y1 AS INTEGER
        x2 AS INTEGER
        y2 AS INTEGER
  Landable AS INTEGER
END TYPE

TYPE CurveType
        x AS INTEGER
        y AS INTEGER
    Start AS SINGLE
      Fin AS SINGLE
    Ratio AS SINGLE
   Radius AS SINGLE
END TYPE

TYPE PaintType
        x AS INTEGER
        y AS INTEGER
END TYPE

TYPE LandspotType
        x AS INTEGER
        y AS INTEGER
   Landed AS INTEGER
END TYPE

TYPE GameType
        Mode AS INTEGER
     EndCond AS INTEGER
    Quantity AS INTEGER
END TYPE

TYPE PlyrShip
      LanPos AS INTEGER
        PosX AS SINGLE
        PosY AS SINGLE
         Col AS INTEGER
  DefaultCol AS INTEGER
      Landed AS INTEGER
    Exploded AS INTEGER
       Fired AS INTEGER
        AddX AS SINGLE
        AddY AS SINGLE
       Angle AS SINGLE
     Refresh AS INTEGER
 RefreshTime AS INTEGER
RefreshSleep AS INTEGER

          UP AS INTEGER
        LEFT AS INTEGER
       RIGHT AS INTEGER
        DOWN AS INTEGER
      SHIELD AS INTEGER

       KEYUP AS INTEGER
     KEYLEFT AS INTEGER
    KEYRIGHT AS INTEGER
     KEYDOWN AS INTEGER
   KEYSHIELD AS INTEGER

DelayControl AS INTEGER
END TYPE

TYPE Fragments
           x AS SINGLE
           y AS SINGLE
       Speed AS SINGLE
   Direction AS SINGLE
         Col AS INTEGER
        AddX AS SINGLE
        AddY AS SINGLE
     BackCol AS INTEGER
END TYPE

TYPE LaserType
          x AS SINGLE
          y AS SINGLE
     Xspeed AS SINGLE
     Yspeed AS SINGLE
       Life AS INTEGER
    BackCol AS INTEGER
END TYPE

TYPE PlayerType
         AI AS INTEGER
     Target AS INTEGER
        Dmg AS INTEGER
      Lives AS INTEGER
      Frags AS INTEGER
      Kills AS INTEGER
   ShotsTkn AS INTEGER
   ShotsHit AS INTEGER
   ShotsLft AS INTEGER
    FuelLft AS INTEGER
       Dead AS INTEGER
      Crash AS INTEGER
END TYPE

TYPE DebrisType
          x AS SINGLE
          y AS SINGLE
       AddX AS SINGLE
       AddY AS SINGLE
       Show AS INTEGER
       Life AS INTEGER
        Col AS INTEGER
END TYPE


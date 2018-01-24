//********** JHeCDL -Johan Henrikssons CollisionDetectionLibrary ***********
//See jhecdl.txt for details or read above functions for quick usage
//Release 3
// Updated by MAZsoft (10.02.01): Circle - Line collision detection

#include <math.h>
#include <allegro.h>
#include <conio.h>
#include <pc.h>

#define COLLIDED        TRUE
#define NO_COLLISION    FALSE

int circ_cd             (int x1,int y1,int x2,int y2,int r1,int r2);   //Circle collisiondetection
int bb_cd               (int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4);  //Boundingbox collisiondetection
int sphere_cd           (int x1,int y1,int z1,int x2,int y2,int z2,int r1,int r2); //Sphere collisiondetection
int pn_cd               (int x1,int y1,int x2,int y2,int x3,int y3,int x4,int y4,BITMAP *sprite1,BITMAP *sprite2); //Pixelnear collisiondetection
int pointbb_cd          (int px,int py,int x1,int y1,int x2,int y2); //Point boundingbox CD
int pointpn_cd          (int px,int py,int x1,int y1,int x2,int y2, BITMAP *sprite); //Point pixelperfect CD
int pointci_cd          (int x1,int y1,int x2,int y2,int r1);
int pointsp_cd          (int x1,int y1,int z1,int x2,int y2,int z2,int r1);
int cylinder_cd         (int x1,int y1,int height1,int z1,int r1,  int x2,int y2,int height2,int z2,int r2);
int circline_cd         (int x1,int y1,int x2,int y2,int x,int y,int r);

int circle_cd(int x1,int y1,int x2,int y2,int r1,int r2)
{
  if(x1+r1 > x2-r2)
    if(x1-r1 < x2+r2)
      if(y1+r1 > y2-r2)
        if(y1-r1 < y2+r2)
          if((r1+r2)*(r1+r2) > (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))
            return(COLLIDED);

  return(NO_COLLISION);
}

int pointci_cd(int x1,int y1,int x2,int y2,int r1)
{
  if(x1+r1 > x2)
    if(x1-r1 < x2)
      if(y1+r1 > y2)
        if(y1-r1 < y2)
          if(r1*r1 > (x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))
            return(COLLIDED);

  return(NO_COLLISION);
}


int sphere_cd(int x1,int y1,int z1,int x2,int y2,int z2,int r1,int r2)
{
  if(x1+r1>x2-r2)
    if(x1-r1<x2+r2)
      if(y1+r1>y2-r2)
        if(y1-r1<y2+r2)
          if(z1+r1>z2-r2)
            if(z1-r1<z2+r2)
              if((r1+r2)*(r1+r2) > ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+(z1-z2)*(z1-z2)    )
                return(COLLIDED);

  return(NO_COLLISION);
}


int pointsp_cd(int x1,int y1,int z1,int x2,int y2,int z2,int r1)
 {
 if(x1+r1>x2)
  if(x1-r1<x2)
   if(y1+r1>y2)
    if(y1-r1<y2)
     if(z1+r1>z2)
      if(z1-r1<z2)
       if(r1*r1 > ((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2))+(z1-z2)*(z1-z2))
        return(COLLIDED);
 return(NO_COLLISION);
 }


int bb_cd(int x1,int y1,int x2,int y2,  int x3,int y3,int x4,int y4)
 {
 if(x2+x1-1<x3)
  {return(NO_COLLISION);}
 else
  {
  if(x3+x4-1<x1)
   {return(NO_COLLISION);}
  else
   {
   if(y2+y1-1<y3)
    {return(NO_COLLISION);}
   else
    {
    if(y4+y3-1<y1)
     {return(NO_COLLISION);}
    else
     {
     return(COLLIDED);
     }
    }
   }
  }
 }


//Point boundingbox CD
int pointbb_cd(int px,int py,  int x1,int y1,int x2,int y2)
 {
 if(px>x1 && px<x1+x2 && py>y1 && py<y1+y2)
  return(COLLIDED);
 else
  return(NO_COLLISION);
 }

//Point pixelnear CD
int pointpn_cd(int px,int py, int x1,int y1,int x2,int y2, BITMAP *sprite)
 {
 if(pointbb_cd(px,py,x1,y1,x2,y2)==COLLIDED)
  {
  if(bitmap_color_depth(sprite)==8)
   if(sprite->          line[py-y1][px-x1]!=MASK_COLOR_8)
    return(COLLIDED);
  if(bitmap_color_depth(sprite)==15)
   if(((short *)sprite->line[py-y1])[px-x1]!=(short)MASK_COLOR_15)
    return(COLLIDED);
  if(bitmap_color_depth(sprite)==16)
   if(((short *)sprite->line[py-y1])[px-x1]!=(short)MASK_COLOR_16)
    return(COLLIDED);
  if(bitmap_color_depth(sprite)==32)
   if(((long *)sprite-> line[py-y1])[px-x1]!=(long)MASK_COLOR_32)
    return(COLLIDED);
  return(NO_COLLISION);
  }

 return(NO_COLLISION);
 }


int cylinder_cd(int x1,int y1,int height1,int z1,int r1,  int x2,int y2,int height2,int z2,int r2)
 {
 int hh1, hh2;
 hh1=height1/2;
 hh2=height2/2;
 if(z1+hh1>z2-hh2 && z1-hh1<z2+hh2)
  if(circle_cd(x1,y1,x2,y2,r1,r2)==COLLIDED)
   return(COLLIDED);
 return(NO_COLLISION);
 }


int pn_cd(int x1,int y1,int x2,int y2,   int x3,int y3,int x4,int y4,BITMAP *sprite1,BITMAP *sprite2)
 {
 int min_x,max_x,
     min_y,max_y,
     itte,itte2,
     collision,
     for_x,for_y;
 if(bb_cd(x1,y1,x2,y2,x3,y3,x4,y4)==NO_COLLISION)
  return(NO_COLLISION);
 if(x1+x2>x3+x4)
  {
  //First tile is to the right
  min_x=x3-x1;
  if(min_x<0)
   min_x=0;
  max_x=x3+x4-x1-1;
  }
 else
  {
  //First tile is to the left
  min_x=x3-x1;
  if(min_x<0)
   min_x=0;
  max_x=x2-1;
  }

 if(y1+y2<y3+y4)
  {
  //First tile is above the second one
  min_y=y3-y1;
  if(min_y<0)
   min_y=0;
  max_y=y2-1;
  }
 else
  {
  //First tile is under the second one
  min_y=y3-y1;
  if(min_y<0)
   min_y=0;
  max_y=(y3+y4)-y1-1;
  }

 for_x=x1-x3;
 for_y=y1-y3;
 collision=NO_COLLISION;


 if(bitmap_color_depth(sprite1)==8)
  {
  itte2=min_y-1;
  do
   {
   itte2++;
   itte=min_x-1;
   if(itte2!=max_y)
    do
     {
     itte++;
     if(sprite1->line[itte2][itte] != MASK_COLOR_8)
      if(sprite2->line[itte2+for_y][itte+for_x] != MASK_COLOR_8)
       collision=COLLIDED;
     } while(itte<max_x && collision!=COLLIDED);
   } while(itte2<max_y && collision!=COLLIDED);
  }

 if(bitmap_color_depth(sprite1)==15)
  {
  itte2=min_y-1;
  do
   {
   itte2++;
   itte=min_x-1;
   if(itte2!=max_y)
    do
     {
     itte++;
     if(((short *)sprite1->line[itte2])[itte] != (short)MASK_COLOR_15)
      if(((short *)sprite2->line[itte2+for_y])[itte+for_x] != (short)MASK_COLOR_15)
       collision=1;
     } while(itte<max_x && collision!=COLLIDED);
   } while(itte2<max_y && collision!=COLLIDED);
  }

 if(bitmap_color_depth(sprite1)==16)
  {
  itte2=min_y-1;
  do
   {
   itte2++;
   itte=min_x-1;
   if(itte2!=max_y)
    do
     {
     itte++;
     if(((short *)sprite1->line[itte2])[itte] != (short)MASK_COLOR_16)
      if(((short *)sprite2->line[itte2+for_y])[itte+for_x] != (short)MASK_COLOR_16)
       collision=1;
     } while(itte<max_x && collision!=COLLIDED);
   } while(itte2<max_y && collision!=COLLIDED);
  }

 if(bitmap_color_depth(sprite1)==32)
  {
  itte2=min_y-1;
  do
   {
   itte2++;
   itte=min_x-1;
   if(itte2!=max_y)
    do
     {
     itte++;
     if(((long *)sprite1->line[itte2])[itte] != (long)MASK_COLOR_32)
      if(((long *)sprite2->line[itte2+for_y])[itte+for_x] != (long)MASK_COLOR_32)
       collision=COLLIDED;
     } while(itte<max_x && collision!=COLLIDED);
   } while(itte2<max_y && collision!=COLLIDED);
  }

 return(collision);
 }
#ifndef SQR
#define SQR(x)          ((x)*(x))
#endif
int circline_cd(int x1,int y1,int x2,int y2,int x,int y,int r)
{
  float a, b, c, p, l;

  a = sqrt(SQR(x-x1)  + SQR(y-y1));
  b = sqrt(SQR(x-x2)  + SQR(y-y2));
  c = sqrt(SQR(x1-x2) + SQR(y1-y2));

  p = (a+b+c)/2;

  //l = sqrt((4*p*(p-a)*(p-b)*(p-c))/SQR(c));
  l = (4*p*(p-a)*(p-b)*(p-c))/SQR(c);

  if(l <= SQR(r))
  {
    if(((SQR(a) - l) <= SQR(c + r)) && ((SQR(b) - l) <= SQR(c + r)))
      return COLLIDED;
  }

  return NO_COLLISION;
}


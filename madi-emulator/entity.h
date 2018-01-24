#ifndef __ENTITY_H__
#define __ENTITY_H__

class Entity
{
private:
public:
   //Entity ();
   //xvirtual ~Entity ();

   virtual void  draw           (BITMAP *bmp) = 0;
   virtual void  move           () = 0;
   virtual bool  done           () = 0;
};

class Student : public Entity
{
private:
   float x, speed;
   int direction; // +1 -- right, -1 -- left
   DATAFILE *gfx;
   int max_gfx; // number of frames in animation
   int frame;
   float fc; // frame changer
   bool remove;

public:
   Student ();
   ~Student ();

   void draw (BITMAP *bmp);
   void move ();
   bool done ();
};

class Sun : public Entity
{
private:
   int angle, r;
   fixed ra;

public:
   Sun ();
   ~Sun ();

   void draw (BITMAP *bmp);
   void move ();
   bool done ();
};

class Scroller : public Entity
{
private:
   int y, h;

public:
   Scroller ();
   ~Scroller ();

   void draw (BITMAP *bmp);
   void move ();
   bool done ();   
};

#endif

#ifndef _LIST_H_
#define _LIST_H_

#include "entity.h"

class Node
{
public:
   Entity *item;
   Node   *next, *prev;
};

class List
{
private:
   Node *first, *last;
   int count;
public:
   List ();
   ~List ();
   void add               (Entity *item );
   void remove            (Node *node );
   void remove_all        ();
   void draw_all          (BITMAP *bmp);
   void move_all          ();
   void remove_dead_nodes ();
   void unlink_entity     (Entity *item );
};

#endif

/* END */

#include "madi.h"

List::List ()
{
   first = NULL;
   last  = NULL;
   count = 0;
}

List::~List ()
{
   remove_all ();
}

void List::add ( Entity *item )
{
   if (first == NULL)
   {
      first = new Node;
      first->item = item;
      first->next = NULL;
      first->prev = NULL;
      last = first;
   }
   else
   {
      Node *p = last;
      Node *insert = new Node;
      insert->item = item;
      insert->next = NULL;
      insert->prev = p;
      p->next = insert;
      last = p->next;
   }
   count++;
}

void List::remove ( Node *p )
{
   if (first == p) first = p->next;
   if (last  == p) last  = p->prev;
   if (p->prev) p->prev->next = p->next;
   if (p->next) p->next->prev = p->prev;
   delete(p);
   count--;
}

void List::remove_all ()
{
   Node *p = first;
   Node *x;
   while (p)
   {
      x = p->next;
      delete(p->item);
      remove(p);
      p = x;
   }
   count = 0;
}

void List::draw_all (BITMAP *bmp)
{
   Node *p = first;
   while (p)
   {
      p->item->draw (bmp);
      p = p->next;
   }
}

void List::move_all ()
{
   Node *p = first;
   while (p)
   {
      p->item->move();
      p = p->next;
   }
}

void List::remove_dead_nodes ()
{
   Node *p = first;
   Node *x;
   while (p)
   {
      x = p->next;
      if (p->item->done())
      {
         delete(p->item);
         remove(p);
      }
      p = x;
   }
}

void List::unlink_entity (Entity *item )
{
   Node *p = first;
   Node *x;
   while (p)
   {
      x = p->next;
      if (p->item == item) remove(p);
         p = x;
   }
}


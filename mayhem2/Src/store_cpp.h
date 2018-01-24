#ifndef __STORE_H__
#define __STORE_H__

class Store
{
public:
   Store ();
   ~Store ();

   void     add       (char *file, char *prefix, char *desc = NULL);
   void     *dat      (char *key);
/*
   BITMAP   *dat      (char *key) { return (BITMAP *)dat (key); }
   FONT     *dat      (char *key) { return (FONT   *)dat (key); }
   SAMPLE   *dat      (char *key) { return (SAMPLE *)dat (key); }
*/
};

extern Store store;

#endif
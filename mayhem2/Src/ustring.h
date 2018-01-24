/*  __  __
 * |  ||  |
 * |  ||  |                  String-like C++ class, to easily use Allegro Unicode Functions
 * |  ||  |
 * |  ||  |                                     (c) MAZsoft 2002
 * |  \/  | S T R I N G
 *  \____/
 *
 */

#ifndef __USTRING_H__
#define __USTRING_H__

#include "allegro.h"

// TODO: Don't forget to remove this after Allegro 4.1 comes out ;)
int ustrnicmp(AL_CONST char *s1, AL_CONST char *s2, int n);

//#include "codepage.h"

#define DEF_UTYPE                       U_UTF8  /* because "codepages are evil" :-) */
#define MAX_USTR_CONVERT_BUFFER         8192

class ustring
{
private:
   struct srep
   {
   private:
      void resize_helper (int newsize)
      {
         if (size)
         {
            delete [] s;
            s = NULL;
         }
         
         size = newsize;
         if (size)
            s = new char [size];
      }

      // to prevent copying
      srep (srep &);
      srep& operator= (srep &);
      
   public:
      char *s;
      int  size;
      int  n;

      srep (const char *c)
      {
         n = 1;
         size = 0;

         assign (DEF_UTYPE, c, -1);
      }

      ~srep ()
      {
         if (!n && size)
         {
            delete [] s;
            size = 0;
         }
      }

      void assign (int type, const char *c, int len)
      {
         char buf[MAX_USTR_CONVERT_BUFFER];
         
         resize_helper (uconvert_size (c, type, U_CURRENT));
         if (len > 0)
            ustrncpy (s, uconvert (c, type, buf, U_CURRENT, sizeof(buf)), len);
         else
            ustrcpy (s, uconvert (c, type, buf, U_CURRENT, sizeof(buf)));
      }

      void append (int type, const char *c)
      {
         char *newc;
         char buf[MAX_USTR_CONVERT_BUFFER];
                                                            // two trailing zeros
         newc = new char [size + uconvert_size (c, type, U_CURRENT) - 1];

         ustrcpy (newc, s);
         ustrcat (newc, uconvert (c, type, buf, U_CURRENT, sizeof(buf)));

         resize_helper (size + uconvert_size (c, type, U_CURRENT) - 1);

         ustrcpy (s, newc);
      }

      int index (int i)
      {      
         int c, pos = 0;

         while ( (c = ugetc(s + pos)) )
         {
            if (--i < 0)
               return c;
               
            pos  += ucwidth (c);            
         }

         return c;
      }
   };

   srep *rep;

public:
   ustring ()
   {
      rep = new srep ("");
   }
   
   ustring (const char *c)
   {
      rep = new srep (c);
   }

   ustring (const ustring &ustr)
   {
      ustr.rep->n++;

      rep = ustr.rep;
   }
   
   ~ustring ()
   {
      rep->n--;

      if (!rep->n)
         delete rep;
   }

   //===============================================================================
   // string assigning
   
   void assign (int type, const char *c, int len = -1)
   {
      if (rep->n == 1)
      {
         rep->assign (type, c, len);
      }
      else
      {
         rep->n--;
         rep = new srep ("");
      
         rep->assign (type, c, len);
      }
   }

   ustring& operator=  (const char *c)
   {
      assign (DEF_UTYPE, c);

      return *this;
   }
   
   ustring& operator=  (const ustring &ustr)
   {
      ustr.rep->n++;

      if (--rep->n == 0)
      {
         delete rep;
      }

      rep = ustr.rep;

      return *this;
   }

   //===============================================================================
   // string appending
   
   void append (int type, const char *c)
   {
      char *olds;

      if (rep->n == 1)
      {
         rep->append (type, c);
      }
      else
      {
         olds = rep->s;
   
         rep->n--;
         rep = new srep ("");
      
         rep->assign (U_CURRENT, olds, -1);
         rep->append (type, c);
      }
   }
   
   ustring& operator+= (const char *c)
   {
      append (DEF_UTYPE, c);

      return *this;
   }
   
   ustring& operator+= (const ustring &ustr)
   {
      append (U_CURRENT, ustr.rep->s);

      return *this;
   }

   //===============================================================================

   int operator[] (int i)
   {
      return ugetat (rep->s, i); //rep->index (i);
   }

   //===============================================================================
   // string comparing
   
   int compare (const ustring &other, int len = -1) const
   {
      if (len > 0)
         return ustrncmp (rep->s, other.rep->s, len);
      else
         return ustrcmp  (rep->s, other.rep->s);
   }
   
   int case_compare (const ustring &other, int len = -1) const
   {
      if (len > 0)
         return ustrnicmp ((const char *)rep->s, (const char *)other.rep->s, len);
      else
         return ustricmp  (rep->s, other.rep->s);
   }
   
   int compare (const char *other, int len = -1) const
   {
      if (len > 0)
         return ustrncmp (rep->s, other, len);
      else
         return ustrcmp  (rep->s, other);
   }
   
   int case_compare (const char *other, int len = -1) const
   {
      if (len > 0)
         return ustrnicmp ((const char *)rep->s, (const char *)other, len );
      else
         return ustricmp  (rep->s, other);
   }

   //===============================================================================
   // case changing routines
   
   void to_upper ()
   {
      char buf[MAX_USTR_CONVERT_BUFFER];

      ustrcpy (buf, rep->s);
      assign (U_CURRENT, buf);

      ustrcpy (buf, ustrupr (rep->s));
      assign (U_CURRENT, buf);
   }
   
   void to_lower ()
   {
      char buf[MAX_USTR_CONVERT_BUFFER];
   
      ustrcpy (buf, rep->s);
      assign (U_CURRENT, buf);

      ustrcpy (buf, ustrlwr (rep->s));
      assign (U_CURRENT, buf);
   }

   //===============================================================================

   int get_int ()
   {
      char *bp;
      
      return (int)ustrtol (rep->s, &bp, 0);
   }

   int get_uint ()
   {
      char *bp;
      
      return ustrtol (rep->s, &bp, 0);
   }

   float get_float ()
   {
      char *bp;
      
      return (float)ustrtod (rep->s, &bp);
   }
   
   char *get_s ()
   {
      return rep->s;
   }
   
   //===============================================================================

   operator int () const
   {
      char *bp;
      
      return (int)ustrtol (rep->s, &bp, 0);
   }

   operator unsigned () const
   {
      char *bp;
      
      return ustrtol (rep->s, &bp, 0);
   }

   operator float () const
   {
      char *bp;
      
      return (float)ustrtod (rep->s, &bp);
   }

   operator char* () const
   {
      return rep->s;
   }
   
   //===============================================================================
   
   int size ()
   {
      return rep->size;
   }

   int length ()
   {
      return ustrlen(rep->s);
   }
   
   int n ()
   {
      return rep->n;
   }
   
   //===============================================================================
   
   void render (BITMAP *bmp, FONT *f, int x, int y, int col)
   {
      textout (bmp, f, rep->s, x, y, col);
   }

   void print ()
   {
      allegro_message ( "%s", rep->s );
   }

   /*void print (ostream &output) const
   {
      output << get_s ();
   }
   
   operator << (ostream )*/
};

//===============================================================================

inline ustring operator+  (const ustring &ustr1, const ustring &ustr2)
{
   ustring s (ustr1);
   return s += ustr2;
}

inline ustring operator+  (const ustring &ustr1, const char *c)
{
   ustring s (ustr1);
   return s += c;
}

//===============================================================================

inline bool operator== (const ustring &s1, const ustring &s2)
{
   return (s1.compare (s2) == 0);
}

inline bool operator!= (const ustring &s1, const ustring &s2)
{
   return (s1.compare (s2) != 0);
}

inline bool operator>  (const ustring &s1, const ustring &s2)
{
   return (s1.compare (s2) > 0);
}

inline bool operator>=  (const ustring &s1, const ustring &s2)
{
   return (s1.compare (s2) >= 0);
}

inline bool operator<  (const ustring &s1, const ustring &s2)
{
   return (s1.compare (s2) < 0);
}

inline bool operator<=  (const ustring &s1, const ustring &s2)
{
   return (s1.compare (s2) <= 0);
}

//===============================================================================

inline bool operator== (const ustring &s1, const char *s2)
{
   return (s1.compare (s2) == 0);
}

inline bool operator!= (const ustring &s1, const char *s2)
{
   return (s1.compare (s2) != 0);
}

inline bool operator>  (const ustring &s1, const char *s2)
{
   return (s1.compare (s2) > 0);
}

inline bool operator>=  (const ustring &s1, const char *s2)
{
   return (s1.compare (s2) >= 0);
}

inline bool operator<  (const ustring &s1, const char *s2)
{
   return (s1.compare (s2) < 0);
}

inline bool operator<=  (const ustring &s1, const char *s2)
{
   return (s1.compare (s2) <= 0);
}

//===============================================================================

#endif // __USTRING_H__


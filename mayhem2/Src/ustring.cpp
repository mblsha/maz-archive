#include "ustring.h"

/* ustrnicmp:
 *  Unicode-aware version of the DJGPP strnicmp() function.
 */
int ustrnicmp(AL_CONST char *s1, AL_CONST char *s2, int n)
{
   int c1, c2;

   if (n <= 0)
      return 0;

   for (;;) {
      c1 = utolower(ugetxc(&s1));
      c2 = utolower(ugetxc(&s2));

      if (c1 != c2)
         return c1 - c2;

      if ((!c1) || (--n <= 0))
         return 0;
   }
}


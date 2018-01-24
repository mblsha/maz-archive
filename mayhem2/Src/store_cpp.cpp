#include "def.h"
#include "store_cpp.h"
#include "store/store.h"

Store store;

Store::Store ()
{
   store_init (2500);
}

Store::~Store ()
{
   store_shutdown ();
}

// duplicates one in common.h
void sys_error (char *fmt, ...);

static DATAFILE *load_data (AL_CONST char *file)
{
   DATAFILE *d;

   d = load_datafile (file);

   if (d)
      convert_images_in_datafile (d);

   return d;
}

void Store::add (char *file, char *prefix, char *desc)
{
   if (store_load_ex (file, prefix, load_data) == -1)
   {
      if (desc)
         sys_error ("\"%s\" datafile doesn't exist!\nPlease reinstall", file);
      else
         console.printf (con_error, "Cannot open \"%s\" datafile!\n", file);

      return;
   }

   if (desc)
      console.printf ("%s successfully loaded.\n", desc);
}

void *Store::dat (char *key)
{
   return store_dat (key);
}


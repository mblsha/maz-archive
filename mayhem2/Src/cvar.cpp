#include "def.h"
#include "console.h"
#include <string.h>

cvar  *cvar_vars;
char  *cvar_null_string = "";

boolean cvar::do_no_set    = false;
boolean cvar::use_callback = false;

//cvar *cvars = NULL;
#define cvars first
static cvar *first = NULL;
static cvar *last  = NULL;

int cvar_defflags;

cvar::cvar (const cvar &var)
{
   sys_error ("Use of cvar copy constructor");
}

cvar &cvar::operator= (const cvar &var)
{
   sys_error ("Use of cvar = operator");
   return *this;
}

cvar::cvar (const char *var_name, const char *def_str, int flags)
{
   ustring name = var_name;
   ustring def  = def_str;

   init_self (name, def, flags, NULL);
}

cvar::cvar (const char *var_name, const char *def_str, int flags, void (*callback)(cvar &))
{
   ustring name = var_name;
   ustring def  = def_str;

   init_self (name, def, flags, callback);
}

static int count;

void cvar::init_self (ustring &var_name, ustring &def, int var_flags, void (*callback)(cvar &))
{
   cvar *var, *dummy;
//   ustring var_name = _var_name;

   callback_proc = callback;
   string = "";
   value  = 0.0f;
   flags  = 0;
   latched_string = "";

   if ( def.length () )
      default_string = def;
   else
      default_string = "";

   var = find_cvar (var_name, &dummy);

   if ( var_name.length () )
   {
      name = var_name;

      if (first == NULL)
      {
         next  = NULL;
         prev  = NULL;
         first = this;

         last  = first;
      }
      else
      {
         next       = first;
         prev       = NULL;
         first      = this;
      }
   }
   else
      name = "";

   if (var)
   {
      force_set (var->string);

      if (var->flags & CVAR_AUTO)
         delete var;
      else
         var->~cvar();
   }
   else if ( def.length () )
   {
      force_set (def);
   }

   sort ();

   flags = var_flags | CVAR_ISDEFAULT;

   count++;
}

cvar::~cvar ()
{
   if (name.length ())
   {
      cvar *var, *dummy;

      var = find_cvar (name, &dummy);

      if (var == this)
      {
         if (first == var) first = var->next;
         if (last  == var) last  = var->prev;

         if ( var->prev )
            var->prev->next = var->next;

         if (var->next)
            var->next->prev = var->prev;
      }
   }

   sort ();
}

//
// TODO: ADD SV_Broadcast_Printf HERE !!!
//
void cvar::force_set (ustring &val)
{
   if ((flags & CVAR_LATCH) && (game_state != game_playing))
   {
      flags |= CVAR_MODIFIED;
      latched_string = val;
   }
   else if (flags & CVAR_SERVERINFO)
   {
/*      if (netgame && consoleplayer != Net_Arbitrator)
      {
         console_printf ("Only player %d can change %s\n", Net_Arbitrator+1, name);
         return;
      }
      D_SendServerInfoChange (this, val);*/
   }
   else
   {
      flags |= CVAR_MODIFIED;
      string = val;
      value  = val.get_float ();

/*      if (flags & CVAR_USERINFO)
         D_UserInfoChanged (this);
  */
      if (use_callback)
      {
         callback ();
      }
   }

   flags &= ~CVAR_ISDEFAULT;
}

void cvar::force_set (float val)
{
   char string[32];

   usprintf (string, "%g", val);
   force_set (string);
}

void cvar::force_set (const char *val)
{
   ustring str;

   str.assign (U_CURRENT, val);

   force_set (str);
}

void cvar::set (ustring &val)
{
   if (!(flags & CVAR_NOSET) || !do_no_set)
      force_set (val);
}

void cvar::set (const char *val)
{
   if (!(flags & CVAR_NOSET) || !do_no_set)
      force_set (val);
}

void cvar::set (float val)
{
   if (!(flags & CVAR_NOSET) || !do_no_set)
      force_set (val);
}

void cvar::set_default (const char *val)
{
   //delete default_string;

   default_string = val;

   if (flags & CVAR_ISDEFAULT)
   {
      set (val);
      flags |= CVAR_ISDEFAULT;
   }
}

void cvar::enable_no_set ()
{
   do_no_set = true;
}

void cvar::enable_callbacks ()
{
   cvar *var;

   for (var = cvars; var; var = var->next)
   {
      var->use_callback = true;
      var->callback ();
   }
}

//#include "templates.cpp"

void cvar::sort ()
{
   cvar *var, *vprev, *vnext, *vnext2;
   boolean sorted = false;

   while (!sorted)
   {
      sorted = true;

      for (var = cvars; var; var = var->next)
      {
         vprev = var->prev;
         vnext = var->next;

         if ( vnext )
            vnext2 = vnext->next;
         else
            vnext2 = NULL;

         if ( vnext )
         {
            if ( var->name > vnext->name )
            {
               //     1     -     2   < - >   3     -     4
               //   vprev        var        vnext       vnext2

               //     1     -     3   < - >   2     -     4
               //   vprev       vnext        var        vnext2

               //     3     -     2     -     4
               //   first        var        vnext2

               //temp  = vnext->next;

               if (vprev)
               {
                  // linking 1-3
                  vprev->next = vnext;
                  vnext->prev = vprev;

                  // linking 3-2
                  vnext->next = var;
                  var->prev   = vnext;
               }
               else
               {
                  first = vnext;
                  first->next = var;
                  first->prev = NULL;

                  var->prev = first;
               }

               // linking 2-4
               var->next    = vnext2;
               if ( vnext2 )
                  vnext2->prev = var;

               sorted = false;
            }
         }
      }
   }

   // set the last cvar
   if (cvars)
      for (var = cvars; var->next; var = var->next)
         ;

   last = var;

   if (last) // just another bugfix ;)
      last->next = NULL;
}

cvar *cvar_set (ustring &var_name, ustring &val)
{
   cvar *var, *dummy;

   if ( (var = find_cvar (var_name, &dummy)) )
      var->set (val);

   if(!var)
   {
      // there is an error in C code if this happens
      console.printf ("cvar_set: variable %s not found\n", var_name.get_s ());
      return NULL;
   }

   return var;
}

cvar *cvar_force_set (ustring &var_name, ustring &val)
{
   cvar *var, *dummy;

   if ( (var = find_cvar (var_name, &dummy)) )
      var->force_set (val);

   return var;
}

cvar *cvar_set (const char *var_name, const char *value)
{
   ustring name = var_name;
   ustring val  = value;

   return cvar_set (name, val);
}

cvar *cvar_force_set (const char *var_name, const char *value)
{
   ustring name = var_name;
   ustring val  = value;

   return cvar_force_set (name, val);
}

cvar *cvar_set (const char *var_name, ustring &value)
{
   ustring name = var_name;

   return cvar_set (name, value);
}

cvar *cvar_force_set (const char *var_name, ustring &value)
{
   ustring name = var_name;

   return cvar_force_set (name, value);
}


//=====================================================================================

void cvar_init()
{
   //cvars = NULL;
}

cvar *cvar_find_var(ustring &var_name)
{
   cvar  *var;

   for ( var = cvars; var; var = var->next )
   {
      if ( !var->name.case_compare (var_name) )
         return var;
   }

   return NULL;
}

cvar *cvar_find_var(char *var_name)
{
   ustring name = var_name;

   return cvar_find_var (name);
}

float cvar_variable_value (ustring &var_name)
{
   cvar  *var, *dummy;

   var = find_cvar(var_name, &dummy);

   if(!var)
      return 0;

   return atof(var->string.get_s ());
}

ustring empty_ustring = "\0\0\0\0";

ustring &cvar_variable_string(ustring &var_name)
{
   cvar *var, *dummy;

   var = find_cvar (var_name, &dummy);

   if (!var)
      return empty_ustring;

   return var->string;
}

float cvar_variable_value (const char *var_name)
{
   ustring name = var_name;

   return cvar_variable_value (name);
}

ustring &cvar_variable_string (const char *var_name)
{
   ustring name = var_name;

   return cvar_variable_string (name);
}

/*
TODO: complete "__system" variables only if "developer" is set
*/
char *cvar_complete_variable(char *partial)
{
   cvar     *cvar;
   int      len, len2;
   char     comp_name[128];

   len = ustrlen (partial);

   // check for first word length
   for (len2 = 0; len2 < len; len2++)
      if ( ugetat (partial, len2) <= ' ' )
         break;

   if (!len)
      return empty_string;

   //comp_name = (char *)malloc(len2+1);
   //strncpy(comp_name, partial, len2);
   //comp_name[len2] = '\0';
   ustrzncpy (comp_name, 128, partial, len2);

   // check for next cvar
   for (cvar = cvars; cvar; cvar = cvar->next)
   {
      if(cvar->next)
      {
         // if first word completely matches cvar name,
         // so we will return next cvar name
         if ( !cvar->name.case_compare (comp_name) )
         {
            cvar = cvar->next;

            //free(comp_name);

            if(cvar)
               return cvar->name;
         }
      }
   }
   //free(comp_name);

   // check for cvar full name
   for (cvar = cvars; cvar; cvar = cvar->next)
      if ( !cvar->name.case_compare (partial, len) )
         return cvar->name;

   return empty_string;
}

void cvar_enable_callbacks ()
{
   cvar *var;

   cvars->enable_no_set ();

   for (var = cvars; var; var = var->next)
      if (var->callback_proc)
         var->enable_callbacks ();
}

//extern char **_argv_;
extern ustring _argv_[MAX_ARGS];
extern int     _argc_;

boolean cvar_command (void)
{
   cvar  *v;

   // check variables
   v = cvar_find_var ( _argv_[0] );

   if (!v)
      return false;

   // perform a variable print or set
   if ( _argc_ == 1 )
   {
      console.printf ("\"%s\" is \"%s\"\n", v->name.get_s (), v->string.get_s ());
      return true;
   }

   v->set ( _argv_[1] );

   return true;
}


/*
============
Cvar_WriteVariables

Writes lines containing "set variable value" for all variables
with the archive flag set to true.
============
*/

void cvar_write_variables (char * file)
{
   cvar  *var;
   FILE  *fd;

   fd = fopen(file, "a");

   for (var = cvars; var; var = var->next)
   {
      if (var->flags & CVAR_ARCHIVE)
      {
         fprintf (fd, "set \"%s\" \"%s\"\n", var->name.get_s (), var->string.get_s ());
      }
   }
   fclose(fd);

/*   cvar   *var;

   for (var = cvars; var; var = var->next)
      if (var->flags & CVAR_ARCHIVE)
         set_config_string("cvars", var->name, var->string);
*/
}

// obsolete
void cvar_read_variables ()
{
   cvar  *var;
   char  val[256];

   for(var = cvars; var; var = var->next)
   {
//      if (var->flags & CVAR_ARCHIVE)
//      {
         strcpy (val, get_config_string("cvars", var->name.get_s (), var->string.get_s ()));

         var->set (val);
//      }
   }
}

//=====================================================================================
/*
static int STACK_ARGS sort_cvars (const void *a, const void *b)
{
   return stricmp (((*(cvar **)a))->name, ((*(cvar **)b))->name);
}

void cvar_sort (t_array<cvar *> &array, int filter)
{
   cvar *var = cvars;

   while (var)
   {
      if (var->flags & filter)
         array.push (var);

      var = var->next;
   }

   if (array.size () > 0)
   {
      qsort (&array[0], array.size (), sizeof(cvar *), sort_cvars);
   }
}
*/

static struct backup_s
{
   ustring  name;
   ustring  string;
} cvar_backups[MAX_DEMOCVARS];

static int num_backed_up = 0;

void cvar_backup (void)
{
   struct backup_s *backup = cvar_backups;
   cvar   *cvar = cvars;

   while (cvar)
   {
      if (((cvar->flags & CVAR_SERVERINFO) || (cvar->flags & CVAR_DEMOSAVE))
       && !(cvar->flags & CVAR_LATCH))
      {
         if (backup == &cvar_backups[MAX_DEMOCVARS])
            console.printf (con_error, "cvar_backup: Too many cvars to save (%d)", MAX_DEMOCVARS);

         backup->name   = cvar->name;
         backup->string = cvar->string;
         backup++;
      }
      cvar = cvar->next;
   }

   num_backed_up = backup - cvar_backups;
}

void cvar_restore (void)
{
   struct backup_s *backup = cvar_backups;
   int i;

   for (i = num_backed_up; i; i--, backup++)
   {
      cvar_set (backup->name, backup->string);

      //delete[] backup->name;
      //delete[] backup->string;
      backup->name = backup->string = NULL;
   }

   num_backed_up = 0;
}

cvar *find_cvar (ustring &var_name, cvar **prev)
{
   cvar *var;

   if (var_name == "" || !cvars)
      return NULL;

   //var = cvars;
   *prev = NULL;

   for (var = cvars; var; var = var->next)
   //while (var->name)
   {
      if ( !var->name.case_compare (var_name) )
         break;

      *prev = var;
      //var = var->next;
   }

   return var;
}

cvar *find_cvar (const char *var_name, cvar **prev)
{
   ustring name = var_name;

   return find_cvar (name, prev);
}

void cvar_unlatch (void)
{
   cvar *var;

   var = cvars;
   while (var)
   {
      if (var->flags & (CVAR_MODIFIED | CVAR_LATCH))
      {
         unsigned oldflags = var->flags & ~CVAR_MODIFIED;

         var->flags &= ~(CVAR_LATCH | CVAR_SERVERINFO);

         if ( var->latched_string.length () )
         {
            var->set (var->latched_string);
            //delete[] var->latched_string;
            var->latched_string = "";
         }

         var->flags = oldflags;
      }
      var = var->next;
   }
}

void cvar_set_to_defaults (void)
{
   cvar *cvar = cvars;

   while (cvar)
   {
      // Only default save-able cvars
      if (cvar->flags & CVAR_ARCHIVE)
         cvar->set (cvar->default_string);

      cvar = cvar->next;
   }
}

void cvar_archive (void *f)
{
   cvar *cvar = cvars;

   while (cvar)
   {
      if (cvar->flags & CVAR_ARCHIVE)
      {
         fprintf ((FILE *)f, "set %s \"%s\"\n", cvar->name.get_s (), cvar->string.get_s ());
      }

      cvar = cvar->next;
   }
}

BEGIN_COMMAND (set, "Sets cvar value, can create new cvars")
{
   if (argc != 3)
   {
      console.printf (con_description, "usage: set <variable> <value>\n");
   }
   else
   {
      cvar *var, *prev;

      var = find_cvar (argv[1], &prev);

      if (!var)
      {
         var = new cvar (argv[1].get_s (), NULL, CVAR_AUTO | CVAR_UNSETTABLE | cvar_defflags);

         cvars->sort ();
      }

      var->set (argv[2]);

      if (var->flags & CVAR_NOSET)
      {
         console.printf (con_warning, "%s is write protected.\n", argv[1].get_s ());
      }
      else if (var->flags & CVAR_LATCH)
      {
         console.printf ("%s will be changed for next game.\n", argv[1].get_s ());
      }
   }
}
END_COMMAND (set)

BEGIN_COMMAND (get, "Displays value of cvar")
{
   cvar *var, *prev;

   if (argc >= 2)
   {
      if ( (var = find_cvar (argv[1], &prev)) )
      {
         console.printf ("\"%s\" is \"%s\"\n", var->name.get_s (), var->string.get_s ());
      }
      else
      {
         console.printf ("\"%s\" is unset\n", argv[1].get_s ());
      }
   }
   else
   {
      console.printf (con_description, "get <cvarname>: displays cvarname value\n");
   }
}
END_COMMAND (get)

BEGIN_COMMAND (toggle, "Toggle cvar between 1 and 0")
{
   cvar *var, *prev;

   if (argc > 1)
   {
      if ( (var = find_cvar (argv[1], &prev)) )
      {
         var->set ((float)(!var->value));
         console.printf ("\"%s\" is \"%s\"\n", var->name.get_s (), var->string.get_s ());
      }
   }
}
END_COMMAND (toggle)

BEGIN_COMMAND (inc, "Increments cvar by given value")
{
   cvar *var, *prev;

   if (argc > 2)
   {
      if ( (var = find_cvar (argv[1], &prev)) )
      {
         var->set ( (float)(var->value + atof (argv[2].get_s ())) );
      }
      else
      {
         console.printf (con_error, "%s cvar not found\n", argv[1].get_s ());
      }
   }
   else
   {
      console.printf (con_description, "%s <cvarname> <value>: increments cvarname by value\n", argv[0].get_s ());
   }
}
END_COMMAND (inc)

BEGIN_COMMAND (cvarlist, "Lists all cvars")
{
   cvar *var = cvars;
   int count = 0;

   while (var)
   {
      unsigned flags = var->flags;

      count++;
      console.printf ("%c%c%c%c %-20s \"%s\"\n",
                      flags & CVAR_ARCHIVE    ? 'A' : ' ',
                      flags & CVAR_USERINFO   ? 'U' : ' ',
                      flags & CVAR_SERVERINFO ? 'S' : ' ',
                      flags & CVAR_NOSET      ? '-' :
                      flags & CVAR_LATCH      ? 'L' :
                      flags & CVAR_UNSETTABLE ? '*' : ' ',
                      var->name.get_s (),
                      var->string.get_s () );

      var = var->next;
   }

   console.printf ("%d cvars\n", count);
}
END_COMMAND (cvarlist)


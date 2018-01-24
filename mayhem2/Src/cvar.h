#ifndef __CVAR_H__
#define __CVAR_H__

#include "common.h"
//#include "tarray.h"

// cvar->flags values
#define CVAR_ARCHIVE    1           // set to cause it to be saved to vars.rc
#define CVAR_USERINFO   2           // added to userinfo  when changed
#define CVAR_SERVERINFO 4           // added to serverinfo when changed
#define CVAR_NOSET      8           // don't allow change from console at all,
                                    // but can be set from the command line
#define CVAR_LATCH      16          // save changes until server restart
#define CVAR_UNSETTABLE 32          // can unset this var from console
#define CVAR_DEMOSAVE   64          // save the value of this cvar in a demo
#define CVAR_MODIFIED   128         // set each time the cvar is changed
#define CVAR_ISDEFAULT  256         // is cvar unchanged since creation?
#define CVAR_AUTO       512         // allocated, needs to be freed when destroyed

class cvar
{
public:
   cvar (const char *name, const char *def, int flags);
   cvar (const char *name, const char *def, int flags, void (*callback)(cvar &));
   virtual ~cvar ();

   ustring                 name;
   ustring                 string;
   float                   value;
   unsigned int            flags;

   ustring latched_string;
   ustring default_string;

   cvar *prev;
   cvar *next;

   void (*callback_proc)(cvar &);

   inline void callback () { if (callback_proc) callback_proc (*this); }

   void set_default        (ustring    &value);
   void set_default        (const char *value);
   
   void set                (ustring    &value);
   void set                (const char *value);
   void set                (float      value);
   
   void force_set          (ustring    &value);
   void force_set          (const char *value);
   void force_set          (float      value);

   void sort               ();

   //inline void set         (const byte *value) { set ((const char *)value); }
   //inline void force_set   (const byte *value) { force_set ((const char *)value); }

   static void enable_no_set ();       // enable the honoring of CVAR_NOSET
   static void enable_callbacks ();

private:
   cvar            (const cvar &var);
   cvar &operator= (const cvar &var);

   void init_self  (ustring &name, ustring &def, int flags, void (*callback)(cvar &));

   static boolean use_callback;
   static boolean do_no_set;

   friend class cmd_cvarlist;

   // Writes all cvars that could effect demo sync to *demo_p. These are
   // cvars that have either CVAR_SERVERINFO or CVAR_DEMOSAVE set.
   //friend void cvar_write_cvars (byte **demo_p, iny filter, bool compact=false);

   // Read all cvars from *demo_p and set them appropriately.
   //friend void cvar_read_cvars (byte **demo_p);

   // Backup demo cvars. Called before a demo starts playing to save all
   // cvars the demo might change.
   friend void cvar_backup_cvars (void);

   // Finds a named cvar
   friend cvar *find_cvar (ustring &var_name, cvar **prev);

   // Called from G_InitNew()
   friend void unlatch_cvars (void);

   // archive cvars to FILE f
   friend void cvar_archive_cvars (void *f);

   // initialize cvars to default values after they are created
   friend void cvar_set_cvars_to_defaults (void);

   friend boolean set_server_var (char *name, char *value);

   //friend void filter_compact_cvars (t_array<cvar *> &cvars, int filter);

protected:
   cvar () {}
};

// console variable interaction
cvar *cvar_set      (ustring &var_name, ustring &value);
cvar *cvar_forceset (ustring &var_name, ustring &value);
cvar *cvar_set      (const char *var_name, const char *value);
cvar *cvar_forceset (const char *var_name, const char *value);
cvar *cvar_set      (const char *var_name, ustring &value);
cvar *cvar_forceset (const char *var_name, ustring &value);

//inline cvar *cvar_set      (const char *var_name, const byte *value) { return cvar_set (var_name, (const char *)value); }
//inline cvar *cvar_forceset (const char *var_name, const byte *value) { return cvar_forceset (var_name, (const char *)value); }


// Maximum number of cvars that can be saved across a demo. If you need
// to save more, bump this up.
#define MAX_DEMOCVARS 1024

// Restore demo cvars. Called after demo playback to restore all cvars
// that might possibly have been changed during the course of demo playback.
void cvar_restore_cvars (void);


#define BEGIN_CUSTOM_CVAR(name, def, flags)                                        \
        static void cvarfunc_##name(cvar &);                                       \
        cvar   name (#name, def, flags, cvarfunc_##name);                          \
        static void cvarfunc_##name(cvar &var)

#define END_CUSTOM_CVAR(name)

#define CVAR(name, def, flags)                                                     \
        cvar name (#name, def, flags);

#define EXTERN_CVAR(name) extern cvar name;


void     cvar_init               (void);
cvar    *cvar_find_var           (ustring &var_name);
float    cvar_variable_value     (ustring &var_name);
ustring  &cvar_variable_string   (ustring &var_name);
float    cvar_variable_value     (const char *var_name);
ustring  &cvar_variable_string   (const char *var_name);
char     *cvar_complete_variable (char *partial);
//void     cvar_set                (char *var_name, char *value);
void     cvar_set_value          (char *var_name, float value);
void     cvar_register_variable  (cvar *variable);
boolean  cvar_command            (void);
void     cvar_write_variables    (char *file);
void     cvar_read_variables     (void);
void     cvar_enable_callbacks   (void);


#endif

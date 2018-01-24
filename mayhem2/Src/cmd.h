#ifndef __CMD_H__
#define __CMD_H__

#include "console.h"

// allocates an initial text buffer that will grow as needed
void cbuf_init (void);

// as new commands are generated from the console or keybindings,
// the text is added to the end of the command buffer.
void cbuf_add_text (char *text);

// when a command wants to issue other commands immediately, the text is
// inserted at the beginning of the buffer, before any remaining unexecuted
// commands.
void cbuf_insert_text (char *text);

// Pulls off \n terminated lines of text from the command buffer and sends
// them through Cmd_ExecuteString.  Stops when the buffer is empty.
// Normally called once per frame, but may be explicitly invoked.
// Do not call inside a command function!
void cbuf_execute (void);

//===========================================================================


typedef void (*xcommand_t) (void);

/*

Command execution takes a null terminated string, breaks it into tokens,
then searches for a command or variable that matches the first token.

Commands can come from three sources, but the handler functions may choose
to dissallow the action or forward it to a remote server if the source is
not apropriate.

*/

typedef enum
{
   src_client,          // came in over a net connection as a clc_stringcmd
                        // host_client will be valid during this state.

   src_command,         // from the command buffer

   src_console          // from the user's console input
} Cmd_source;

extern   Cmd_source   cmd_source;

void  cmd_init (void);

// called by the init functions of other parts of the program to
// register commands and functions to call for them.
// The cmd_name is referenced later, so it should not be in temp memory
//void cmd_add_command (char *cmd_name, xcommand_t function);

// attempts to match a partial command for automatic command line completion
// returns NULL if nothing fits
char *cmd_complete_command (char *partial);

// The functions that execute commands get their parameters with these
// functions. Cmd_Argv () will return an empty string, not a NULL
// if arg > argc, so string operations are always safe.
//int    cmd_argc  (void);
//char   *cmd_argv (int arg);
//char   *cmd_args (void);

// Returns the position (1 to argc-1) in the command's argument list
// where the given parameter apears, or 0 if not present
int cmd_check_parm (const char *parm);
int cmd_check_parm (ustring &parm);

// Takes a null terminated string.  Does not need to be \n terminated.
// breaks the string up into arg tokens.
void cmd_tokenize_string (char *text);

// Parses a single line of text into arguments and tries to execute it.
// The text can come from the command buffer, a remote client, or stdin.
void cmd_execute_string (char *text, Cmd_source src);

// adds the current command line as a clc_stringcmd to the client message.
// things like godmode, noclip, etc, are commands directed to the server,
// so when they are typed in at the console, they will need to be forwarded.
void cmd_forward_to_server (void);

// used by command functions to send output to either the graphics console or
// passed as a print message to the client
void cmd_print (char *text);

//===========================================================================

// Write out alias commands to a file for all current aliases.
void cmd_write_aliases (char *file);

// build a single string out of multiple strings
ustring build_string (int argc, ustring *argv);

//===========================================================================

class Console_command
{
public:
   Console_command (const char *name);
   Console_command (const char *name, const char *desc);
   virtual ~Console_command ();
   void    init_self        ();

   void sort ();

   virtual void run () = 0;
   virtual boolean is_alias () { return false; }
   void print_command ()
   {
      if ( desc.length () )
         console.printf ("%-20s : %s\n", name.get_s (), desc.get_s ());
      else
         console.printf ("%s\n", name.get_s ());
   }

   Console_command *next, *prev;

   ustring  name;

   ustring  desc;

//protected:
   Console_command ();

   int      argc;
   ustring  *argv;
   ustring  args;
};

#define BEGIN_CUSTOM_COMMAND(n, desc, con_name)                                \
        static class cmd__##n : public Console_command                         \
        {                                                                      \
           public:                                                             \
              cmd__##n () : Console_command (con_name, desc) {}                \
              cmd__##n (const char *name) : Console_command (name) {}          \
              void run ()

#define BEGIN_COMMAND(n, desc) BEGIN_CUSTOM_COMMAND(n, desc, #n)

#define END_COMMAND(n)                                                         \
        } Console_command__##n;
        


class Console_alias : public Console_command
{
public:
   Console_alias   (const char *name, const char *command);
   ~Console_alias  ();

   void run () ;

   boolean is_alias () { return true; }
   void print_alias () { console.printf ("%s : \"%s\"\n", name.get_s (), command.get_s ()); }
   void archive (FILE *f);

protected:
   ustring command;
};

// used by the cvar code to check for cvar / command name overlap
Console_command *cmd_exists (ustring cmd_name);


#define  MAX_ARGS       255

//===========================================================================

#endif

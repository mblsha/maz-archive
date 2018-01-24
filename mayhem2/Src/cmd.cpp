#include "def.h"
#include "stdio.h"
#include "string.h"

#define commands first
static Console_command *first = NULL;
static Console_command *last  = NULL;
Console_alias   *aliases  = NULL;

void Console_alias::run ()
{
   ustring buf;

   buf = command;
   buf += " ";
   buf += build_string (argc - 1, &argv[1]);
   buf += "\n";

   cmd_execute_string (buf, src_console);
}

Console_command *cmd_exists (ustring name)
{
   Console_command *cmd;

   for (cmd = commands; cmd; cmd = cmd->next)
      if ( !cmd->name.case_compare (name) )
         return cmd;

   return NULL;
}

static int count;

void Console_command::init_self ()
{
   if (cmd_exists (name))
   {
      console.printf ("Can't add command: %s already exists!\n", name.get_s ());

      //delete[] name;

      return;
   }

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

   sort ();
   
   count++;
}


Console_command::Console_command (const char *_name)
{
   name = _name;

   init_self ();
}

Console_command::Console_command (const char *_name, const char *_desc)
{
   name = _name;

   if (_desc)
      desc = _desc;
   else
      desc = "";

   init_self ();
}

Console_command::~Console_command ()
{
   if (first == this) first = next;
   if (last  == this) last  = prev;
   if (prev) prev->next = next;
   if (next) next->prev = prev;

   sort();
}

void Console_command::sort ()
{
   Console_command *var, *vprev, *vnext, *vnext2;
   boolean sorted = false;

   while (!sorted)
   {
      sorted = true;

      for (var = commands; var; var = var->next)
      {
         vprev = var->prev;
         vnext = var->next;

         if ( vnext )
            vnext2 = vnext->next;
         else
            vnext2 = NULL;

         if ( vnext )
         {
            if (var->name > vnext->name)
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
   if (commands)
      for (var = commands; var->next; var = var->next)
         ;

   last = var;

   if (last) // just another bugfix ;)
      last->next = NULL;
}

Console_alias::Console_alias (const char *_name, const char *_command) : Console_command (_name)
{
   command = copy_string (_command);
}

Console_alias::~Console_alias ()
{
   //delete[] command;
}

ustring build_string (int argc, ustring *argv)
{
   ustring temp;
   char    cur[512];
   int arg;

   if (argc == 1)
   {
      temp = argv[0];
      return temp;
   }
   else
   {
      for (arg = 0; arg < argc; arg++)
      {
         if (ustrchr (argv[arg].get_s (), ' '))
         {
            usprintf (cur, "\"%s\" ", argv[arg].get_s ());
            temp += cur;
         }
         else
         {
            usprintf (cur, "%s ", argv[arg].get_s ());
            temp += cur;
         }
      }

      return temp;
   }
}

void Console_alias::archive (FILE *f)
{
   fprintf (f, "alias \"%s\" \"%s\"\n", name.get_s (), command.get_s ());
}

void cmd_write_aliases (char *file)
{
   Console_command *alias;
   FILE            *fd;

   fd = fopen(file, "a");
   for (alias = commands; alias; alias = alias->next)
   {
      if ( alias->is_alias () )
         static_cast<Console_alias *>(alias)->archive (fd);
   }
   fclose(fd);
}


BEGIN_COMMAND (alias, "Manages Aliases")
{
   Console_command *alias;
   ustring cmd;

   if (argc == 1)
   {
      console.printf ("Current alias commands:\n");

      for (alias = commands; alias; alias = alias->next)
         if ( alias->is_alias () )
            static_cast<Console_alias *>(alias)->print_alias ();
   }
   else
   {
      if (argc >= 2)
      {
         // Remove the alias

         for (alias = commands; alias; alias = alias->next)
            if ( alias->is_alias () )
               if ( !alias->name.case_compare (argv[1].get_s ()) )
                  static_cast<Console_alias *>(alias)->~Console_alias ();
      }

      if (argc >= 3)
      {
         // add alias command
         cmd = build_string (argc - 2, &argv[2]);

         new Console_alias (argv[1].get_s (), cmd.get_s ());
      }
   }
}
END_COMMAND (alias)

BEGIN_COMMAND (cmdlist, "Dumps all commands to console")
{
   int count = 0;
   Console_command *cmd;

   for (cmd = commands; cmd; cmd = cmd->next)
   {
      if ( !cmd->is_alias () )
      {
         cmd->print_command ();
         count++;
      }
   }

   console.printf ("%d commands\n", count);
}
END_COMMAND (cmdlist)

// Execute any console commands specified on the command line.
// These all begin with '+' as opposed to '-'.
// If onlyset is true, only "set" commands will be executed,
// otherwise only non-"set" commands are executed.
/*
void C_ExecCmdLineParams (int onlyset)
{
	int currArg, setComp, cmdlen, argstart;
	char *cmdString;

	for (currArg = 1; currArg < Args.NumArgs(); )
	{
		if (*Args.GetArg (currArg++) == '+')
		{
			setComp = stricmp (Args.GetArg (currArg - 1) + 1, "set");
			if ((onlyset && setComp) || (!onlyset && !setComp))
			{
				continue;
			}

			cmdlen = 1;
			argstart = currArg - 1;

			while (currArg < Args.NumArgs())
			{
				if (*Args.GetArg (currArg) == '-' || *Args.GetArg (currArg) == '+')
					break;
				currArg++;
				cmdlen++;
			}

			if ( (cmdString = BuildString (cmdlen, Args.GetArgList (argstart))) )
			{
				C_DoCommand (cmdString + 1);
				delete[] cmdString;
			}
		}
	}
}*/

//===========================================================================

void cmd_forward_to_server (void);

#define  MAX_ALIAS_NAME 32


boolean        cmd_wait;


//=============================================================================

/*
============
Cmd_Wait_f

Causes execution of the remainder of the command buffer to be delayed until
next frame.  This allows commands like:
bind g "impulse 5 ; +attack ; wait ; -attack ; impulse 2"
============
*/
void cmd_wait_f (void)
{
  cmd_wait = true;
}

/*
=============================================================================

                        COMMAND BUFFER

=============================================================================
*/

Sizebuf cmd_text;

/*
============
Cbuf_Init
============
*/
void cbuf_init (void)
{
   //sz_alloc (&cmd_text, 512000);  // space for commands and script files
   //sz_alloc (&cmd_text, 256000);  // space for commands and script files
   //sz_alloc (&cmd_text, 128000);  // space for commands and script files
   //sz_alloc (&cmd_text, 64000);   // space for commands and script files
   sz_alloc (&cmd_text, 16384);   // space for commands and script files
   //sz_alloc (&cmd_text, 8192);    // space for commands and script files
}


/*
============
Cbuf_AddText

Adds command text at the end of the buffer
============
*/
void cbuf_add_text (char *text)
{
   int   l;

   l = uconvert_size (text, U_CURRENT, U_CURRENT) - 1;

   if ((cmd_text.cursize + l) >= cmd_text.maxsize)
   {
      console.printf (con_error, "cbuf_add_text: overflow\nTrying to add %d, but has space only for %d\n", l, cmd_text.maxsize - cmd_text.cursize);
      return;
   }

   sz_write (&cmd_text, text, l);
}


/*
============
Cbuf_InsertText

Adds command text immediately after the current command
Adds a \n to the text
FIXME: actually change the command buffer to do less copying
============
*/
void cbuf_insert_text (char *text)
{
   char  *temp;
   int   templen;

   // copy off any commands still remaining in the exec buffer
   templen = cmd_text.cursize;
   if (templen)
   {
      temp = (char *)malloc (templen);
      memcpy (temp, cmd_text.data, templen);
      sz_clear (&cmd_text);
   }
   else
      temp = NULL;   // shut up compiler

   // add the entire text of the file
   cbuf_add_text (text);

   // add the copied off data
   if (templen)
   {
      sz_write (&cmd_text, temp, templen);
      free (temp);
   }
}

/*
============
Cbuf_Execute
============
*/
void cbuf_execute (void)
{
   int   i, c, size;
   char  *text;
   char  line[1024];
   int   quotes;

   while (cmd_text.cursize)
   {
      // find a '\n' or ';' or '\0' line break
      text = (char *)cmd_text.data;

      quotes = 0;
      i = 0;
      size = 0;
      while (size < cmd_text.cursize)
      {
         c = ugetxc ((const char **)&text);
         size += ucwidth (c);

         if (c == '"')
            quotes++;
            
         if (c == ';' && !(quotes & 1))
            break;        // don't break if inside a quoted string

         if (c == '\n' || c == 0) // '\n', '\0', etc.
            break;
            
         i++;
      }

      ustrzncpy (line, sizeof(line), (char *)cmd_text.data, i);

      // delete the text from the command buffer and move remaining commands down
      // this is necessary because commands (exec, alias) can insert data at the
      // beginning of the text buffer

      if (size >= cmd_text.cursize)
      {
         cmd_text.cursize = 0;
      }
      else
      {
         //size++;
         cmd_text.cursize -= size;
         memcpy (cmd_text.data, cmd_text.data+size, cmd_text.cursize);
      }

      // execute the command line
      cmd_execute_string (line, src_command);

      if (cmd_wait)
      {  // skip out while text still remains in buffer, leaving it
         // for next frame
         cmd_wait = false;
         break;
      }
   }
}

/*
==============================================================================

                        SCRIPT COMMANDS

==============================================================================
*/

/*
===============
Cmd_StuffCmds_f

Adds command line parameters as script statements
Commands lead with a +, and continue until a - or another +
quake +prog jctest.qp +cmd amlev1
quake -nosound +cmd amlev1
===============
*/
BEGIN_COMMAND (stuffcmds, "Executes command line parameters")
{
   int   i, j;
   int   s;
   char  *text, *build, c;

   if (argc != 1)
   {
      console.printf ("stuffcmds : execute command line parameters\n");
      return;
   }

   // build the combined string to parse from
   s = 0;
   for (i = 1; i < com_argc; i++)
   {
      if (!com_argv[i])
         continue;      // NEXTSTEP nulls out -NXHost

      s += strlen (com_argv[i]) + 1;
   }

   if (!s)
      return;

   text = (char *)malloc (s+1);
   text[0] = 0;
   for (i = 1; i < com_argc; i++)
   {
      if (!com_argv[i])
         continue;      // NEXTSTEP nulls out -NXHost

      strcat (text, com_argv[i]);

      if (i != com_argc-1)
         strcat (text, " ");
   }

   // pull out the commands
   build = (char *)malloc (s+1);
   build[0] = 0;

   for (i = 0; i < s-1; i++)
   {
      if (text[i] == '+')
      {
         i++;

         for (j = i; (text[j] != '+') && (text[j] != '-') && (text[j] != 0); j++)
            ;

         c = text[j];
         text[j] = 0;

         strcat (build, text+i);
         strcat (build, "\n");
         text[j] = c;
         i = j-1;
      }
   }

   if (build[0])
      cbuf_insert_text (build);

   free (text);
   free (build);
}
END_COMMAND (stuffcmds)

/*
===============
Cmd_Exec_f
===============
*/
BEGIN_COMMAND (exec, "Executes script file")
{
   char *f;

   if (argc != 2)
   {
      console.printf (con_description, "exec <filename> : execute a script file\n");
      return;
   }

   f = (char *)com_load_file (argv[1].get_s ());
   if (!f)
   {
      console.printf (con_error, "couldn't exec %s\n", argv[1].get_s ());
      return;
   }

   console.printf ("execing %s\n", argv[1].get_s ());

   cbuf_insert_text (f);
}
END_COMMAND (exec)

/*
===============
Cmd_Echo_f

Just prints the rest of the line to the console
===============
*/
BEGIN_COMMAND (echo, "Prints its parameters to console")
{
   int   i;

   for (i=1; i < argc; i++)
      console.printf ("%s ", argv[i].get_s ());

   console.printf ("\n");
}
END_COMMAND (echo)


/*
=============================================================================

                     COMMAND EXECUTION

=============================================================================
*/


int      _argc_;
ustring  _argv_[MAX_ARGS];

Cmd_source     cmd_source;

extern cvar    *cvar_vars;

/*
============
Cmd_Init
============
*/
void cmd_init (void)
{ /*
  cmd_functions = NULL;
  //
  // register our commands
  //
  cmd_add_command ("stuffcmds", cmd_stuff_cmds_f);
  cmd_add_command ("exec",      cmd_exec_f);
  cmd_add_command ("echo",      cmd_echo_f);
  cmd_add_command ("alias",     cmd_alias_f);
  cmd_add_command ("cmd",       cmd_forward_to_server);
  cmd_add_command ("wait",      cmd_wait_f);
  cmd_add_command ("list",      cmd_list_f);*/
}


/*
============
Cmd_TokenizeString

Parses the given string into command line tokens.
============
*/
void cmd_tokenize_string (char *text)
{
   ustrcat (text, "\n"); // bug fix

   _argc_ = 0;

   while (1)
   {
      text = com_parse (text);

      if (!text)
         break;

      if (_argc_ < MAX_ARGS)
      {
         _argv_[_argc_].assign (U_CURRENT, com_token);
         _argc_++;
      }
   }
}


/*
============
Cmd_CompleteCommand
============
*/
extern ustring empty_ustring;

char *cmd_complete_command (char *partial)
{
   Console_command *cmd;
   int             len, len2;
   char            comp_name[128];

   len = ustrlen (partial);

   for(len2 = 0; len2 < len; len2++)
      if(ugetat (partial, len2) <= ' ')
         break;

   if (!len)
      return empty_string;
      
   ustrzncpy (comp_name, 128, partial, len2);

   // check for next command
   for (cmd = commands; cmd; cmd = cmd->next)
   {
      if(cmd->next)
      {
         if ( !cmd->name.case_compare (comp_name) )
         {
            cmd = cmd->next;

            if(cmd)
               return cmd->name;
         }
      }
   }

   // check functions
   for (cmd = commands; cmd; cmd = cmd->next)
      if ( !cmd->name.case_compare (partial, len) )
         return cmd->name;

   return empty_string;
}

/*
============
Cmd_ExecuteString

A complete command line has been parsed, so try to execute it
FIXME: lookupnoadd the token to speed search?
============
*/
void cmd_execute_string (char *text, Cmd_source src)
{
   Console_command *cmd;

   cmd_source = src;

   cmd_tokenize_string (text);

   // execute the command line
   if (!_argc_)
      return;  // no tokens

   // check functions and aliases
   for (cmd = commands; cmd; cmd = cmd->next)
   {
      if ( !cmd->name.case_compare (_argv_[0]) )
      {
         cmd->argc = _argc_;
         cmd->argv = _argv_;

         cmd->run ();
         return;
      }
   }

   // check cvars
   if ( !cvar_command () )
      console.printf ("Unknown command \"%s\"\n", _argv_[0].get_s ());
}


/*
===================
Cmd_ForwardToServer

Sends the entire command line over to the server
===================
*/
void cmd_forward_to_server (void)
{
  /*
  if (cls.state != ca_connected)
  {
    console_printf ("Can't \"%s\", not connected\n", Cmd_Argv(0));
    return;
  }

  if (cls.demoplayback)
    return;            // not really connected

  MSG_WriteByte (&cls.message, clc_stringcmd);

  if (strcasecmp(cmd_argv(0), "cmd") != 0)
  {
    sz_print (&cls.message, cmd_argv(0));
    sz_print (&cls.message, " ");
  }
  if (cmd_argc() > 1)
    sz_print (&cls.message, cmd_args());
  else
    sz_print (&cls.message, "\n");
  */
}


/*
================
Cmd_CheckParm

Returns the position (1 to argc-1) in the command's argument list
where the given parameter apears, or 0 if not present
================
*/
int cmd_check_parm (const char *parm)
{
   ustring tmp = parm;

   return cmd_check_parm (tmp);
}

int cmd_check_parm (ustring &parm)
{
   int i;

   for (i = 1; i < _argc_; i++)
      if ( !parm.case_compare (_argv_[i]) )
         return i;

   return 0;
}

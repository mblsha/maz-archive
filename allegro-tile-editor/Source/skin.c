/*  ______   ___    ___
 * /\  _  \ /\_ \  /\_ \
 * \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___
 *  \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *   \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *    \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *     \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                    /\____/
 *                                    \_/__/
 *           ______     ___               ______      __        __
 *          /\__  _\__ /\_ \             /\  ___\    /\ \  __  /\ \__
 *          \/_/\ \/\_\\//\ \      __    \ \ \__/    \_\ \/\_\/\__  _\  ___   _ __
 *             \ \ \/_/_ \ \ \   /'__'\   \ \  __\   / __ \/_/\/_/\ \/ / __'\/\''__\
 *              \ \ \/\ \ \_\ \_/\  __/    \ \ \_/__/\ \L\ \/\ \ \ \ \/\ \L\ \ \ \/
 *               \ \_\ \_\/\____\ \____\    \ \_____\ \_____\ \_\ \ \_\ \____/\ \ \
 *                \/_/\/_/\/____/\/____/     \/_____/\/_____/\/_/  \/_/\/___/  \/_/
 *
 *                                     by MAZsoft
 *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *
 * DESCRIPTION:
 *
 *   Skin management code
 *
 * CHANGES:
 *
 *   23.02.02 - v0.3.1
 *     Initial release
 *
 */

#include "ated.h"
#include "jpgalleg.h"

typedef struct FILELIST_NODE
{
   struct FILELIST_NODE *next;
   struct FILELIST_NODE *prev;
   char                 file[256];
   int                  type;
   int                  num;
} FILELIST_NODE;

typedef struct FILELIST
{
   int           count;
   
   FILELIST_NODE *first;
   FILELIST_NODE *last;
} FILELIST;

FILELIST list;

#define DAT_JPEG        DAT_ID('J','P','E','G')

typedef struct BMP_DRIVER
{
   char         *name;
   int          type;

   int          (*extension_supported) (char *ext);
   BITMAP       *(*open_file)          (char *file, RGB *pal);
} BMP_DRIVER;

//================================================================================

char *bmp_types[] =
{
   "bmp",
   "lbm",
   "pcx",
   "tga",
   NULL
};

int bmp_ext (char *ext)
{
   int i;

   for (i = 0; bmp_types[i]; i++)
      if (!stricmp (bmp_types[i], ext))
         return TRUE;

   return FALSE;
}

BITMAP *my_load_bitmap (char *file, RGB *pal)
{
   BITMAP *bmp = load_bitmap (file, pal);

   return bmp;
}

char *jpg_types[] =
{
   "jpg",
   "jpeg",
   NULL
};

int jpg_ext (char *ext)
{
   int i;

   for (i = 0; jpg_types[i]; i++)
      if (!stricmp (jpg_types[i], ext))
         return TRUE;

   return FALSE;
}

BMP_DRIVER drivers[] =
{
   {
      "Allegro Bitmap",
       DAT_BITMAP,

       bmp_ext,
       my_load_bitmap
   }
   ,
   {
      "JPEG",
       DAT_JPEG,

       jpg_ext,
       load_jpg
   }
   ,
   {
      NULL,
      0,

      NULL,
      NULL
   }
};

//================================================================================

static void add_to_filelist (AL_CONST char *file, int type)
{
   FILELIST_NODE *node   = NULL;
   FILELIST_NODE *insert = NULL;

   if (list.first == NULL)
   {
      list.first = (FILELIST_NODE *)malloc(sizeof(FILELIST_NODE));
      node = list.first;

      strcpy (node->file, file);
      node->type = type;
      node->next = NULL;
      node->prev = NULL;
      list.last  = list.first;
   }
   else
   {
      node   = list.last;
      insert = (FILELIST_NODE *)malloc(sizeof(FILELIST_NODE));

      strcpy (insert->file, file);
      insert->type = type;
      insert->next = NULL;
      insert->prev = node;
      node->next   = insert;
      list.last    = node->next;
   }

   list.count++;
}

static int break_search = FALSE;

static void check_file (AL_CONST char *filename, int attrib, int param)
{
   char buf[256];
   PACKFILE *f;
   DATAFILE *dat, *data;
   char *p;
   int i, j;

   if ( break_search || key[KEY_ESC] )
   {
      break_search = TRUE;
      return;
   }

   if (attrib & FA_DIREC)
   {
      //
      // DIRECTORY
      //
      p = get_filename(filename);

      if ( stricmp(p, ".") && stricmp(p, "..") )
      {
         // recurse inside a directory
         strcpy (buf, filename);
         put_backslash (buf);
         strcat (buf, "*.*");

         for_each_file (buf, FA_ARCH | FA_RDONLY | FA_DIREC, check_file, param);

         return;
      }

      return;
   }

   //
   // Simple file
   //
   p = get_extension(filename);

   for (i = 0; drivers[i].name; i++)
   {
      if ( drivers[i].extension_supported (p) )
      {
         f = pack_fopen (filename, F_READ);

         if (!f)
            return;

         pack_fclose (f);

         add_to_filelist (filename, drivers[i].type);
      }
   }

   if ( !stricmp (p, "dat") )
   {
      //
      // DATAFILE
      //
      dat = load_datafile(filename);

      if(dat)
      {
         for(i = 0; dat[i].type != DAT_END; i++)
         {
            data = dat + i;

            usprintf(buf, "%s#%s", filename, get_datafile_property(data, DAT_NAME));

            for(j = 0; drivers[j].name; j++)
            {
               if( dat[i].type == drivers[j].type )
               {
                  add_to_filelist (buf, drivers[j].type);
               }
            }
         }

         unload_datafile(dat);
      }
   }
}

int filelist_add (char *path)
{
   char buf[512], buf2[512];
   int  old_count;

   strcpy (buf, path);
   strcpy (buf, fix_filename_slashes(buf));

   break_search = FALSE;

   old_count = list.count;
   for_each_file (buf, FA_ARCH | FA_RDONLY | FA_DIREC, check_file, strlen(buf2)-3);

   return list.count - old_count;
}

static void remove_filelist_node (FILELIST_NODE *p)
{
   if(list.first == p) list.first = p->next;
   if(list.last  == p) list.last  = p->prev;
   if(p->prev) p->prev->next = p->next;
   if(p->next) p->next->prev = p->prev;

   strcpy (p->file, "");

   free (p);
   list.count--;
}

static void filelist_destroy ()
{
   FILELIST_NODE *node = list.first;
   FILELIST_NODE *x;

   while(node)
   {
      x = node->next;

      remove_filelist_node(node);
      node = NULL;

      node = x;
   }

   list.first   = NULL;

   list.count   = 0;
}

static void filelist ()
{
   FILELIST_NODE *node = list.first;
   FILELIST_NODE *x;
   int i = 1;

   while (node)
   {
      x = node->next;

      printf ("%d: %s\n", i++, node->file);
      //cout << i++ << ": " << node->file << "\n";

      node = x;
   }
}

//================================================================================

int build_filelist (char *path)
{
   char buf[256];
   int c = 0;

   if (strlen (path))
      put_backslash (path);

   usprintf (buf, "%s*.%s", path, "dat"); c += filelist_add (buf);
   usprintf (buf, "%s*.%s", path, "jpg"); c += filelist_add (buf);
   usprintf (buf, "%s*.%s", path, "pcx"); c += filelist_add (buf);
   usprintf (buf, "%s*.%s", path, "bmp"); c += filelist_add (buf);
   usprintf (buf, "%s*.%s", path, "tga"); c += filelist_add (buf);

   return c;
}

BITMAP *get_bmp (FILELIST_NODE *node)
{
   int i;

   if (!node)
      return NULL;

   for (i = 0; drivers[i].name; i++)
      if (drivers[i].type == node->type)
         return drivers[i].open_file (node->file, NULL);
   
   return NULL;
}

BITMAP *get_bmp_file (char *file_name)
{
   FILELIST_NODE *node = list.first;
   char buf [256];
   
   strcpy (buf, "skins");
   put_backslash (buf);
   strcat (buf, file_name);
      
   while (node)
   {
      if ( !stricmp (buf, node->file) )
         return get_bmp (node);
   
      node = node->next;
   }

   return NULL;
}

int skin_init ()
{
   return build_filelist ("skins/");
}

int skin_build_list (INDEX_LIST *l, int *current)
{
   int i = 0;
   FILELIST_NODE *node = list.first;
   char *path;

   //current = 0;
/*
   if (l)
   {
      alert ("already have a list", "", "", "ok", NULL, 0, 0);
      return list.count; // already have a list
   }

   l = (INDEX_LIST *) malloc (sizeof (INDEX_LIST) * (list.count + 1));

   if (!l)
   {
      alert ("already have a list", "000", "000", "ok", NULL, 0, 0);
      return 0;
   }
*/
   while (node)
   {
      path = node->file;
   
      if (strstr (path, ".dat"))
      {
         int pos, c;

         pos = ustrlen (path);

         while (pos > 0)
         {
            c = ugetat (path, pos-1);
            
            if ((c == '/') || (c == OTHER_PATH_SEPARATOR) || (c == DEVICE_SEPARATOR))
               break;
               
            pos--;
         }

         //if (!strcmp (skin_file, (char *)path + uoffset (path, pos)))
         //   *current = i;

         ustrcpy ( l[i].name, (char *)path + uoffset (path, pos) );
      }
      else
      {
         //if (!strcmp (skin_file, get_filename (path)))
         //   *current = i;
            
         ustrcpy ( l[i].name, get_filename (path) );
      }
      
      i++;

      node = node->next;
   }

   return list.count;
}

void update_skin ()
{
   BITMAP *bmp;

   bmp = get_bmp_file (skin_file);

   if (!bmp)
   {
      skin = create_bitmap (32, 32);
      clear_to_color (skin, makecol(50,50,100));
      
      ated_alert ("Whoops!",
                  "Can't load specified bitmap.",
                  "Will use the default one instead.", "Ohhh...", NULL, 0, 0);
   }
   else
      skin = bmp;
}


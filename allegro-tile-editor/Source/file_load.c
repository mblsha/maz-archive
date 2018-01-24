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
 *   Map & stuff loading code
 *
 * CHANGES:
 *
 *   13.02.02 - v0.3.0
 *     Rewritten singlemap file format
 */

#ifdef __FILE_C__

long singlemap_version; // the version of the last singlemap
char ated_error[256] = "";

void load_chunk_planes (PACKFILE *f);
void load_chunk_tinfo  (PACKFILE *f);
void load_chunk_main   (PACKFILE *f);
#ifdef ATED_TILE_EDITOR
void load_chunk_temp   (PACKFILE *f);
#endif

//
// You can customize the singlemap file using custom chunks
// This is the default chunk list
//
ATED_CHUNK ated_chunk[] =
{
   {
      ATED_CHUNK_MAIN,
      load_chunk_main
   }
   ,
   {
      ATED_CHUNK_PLANES,
      load_chunk_planes
   }
   ,
   {
      ATED_CHUNK_TINFO,
      load_chunk_tinfo
   }
   ,
   {
      ATED_CHUNK_TEMP,
      load_chunk_temp
   }
   ,
   {
      0,    // You must always add empty ATED_CHUNK at the end
      NULL
   }
};

void load_plane (unsigned *plane, PACKFILE *f)
{
   long size = pack_igetl (f);
   int x, y;

   switch (size)
   {
      case 1:
         {
            unsigned char *temp;
            temp = calloc (mapwidth*mapheight, sizeof(char));
            pack_fread (temp, sizeof(char)*mapwidth*mapheight, f);            
            for (y = 0; y < mapheight; y++)
               for (x = 0; x < mapwidth; x++)
                  plane[y*mapwidth + x] = temp[y*mapwidth + x];
            free (temp);
         }
         break;
         
      case 2:
         {
            unsigned short *temp;
            temp = calloc (mapwidth*mapheight, sizeof(short));
            pack_fread (temp, sizeof(short)*mapwidth*mapheight, f);
            for (y = 0; y < mapheight; y++)
               for (x = 0; x < mapwidth; x++)
                  plane[y*mapwidth + x] = temp[y*mapwidth + x];
            free (temp);
         }
         break;
         
      case 4:      
         pack_fread (plane, sizeof(unsigned)*mapwidth*mapheight, f);
         break;
   }
}

//===================================
// load_chunk_planes
//
// Here is map data
//===================================
void load_chunk_planes (PACKFILE *f)
{
   long chunk;

   create_planes(0,0,0);
   create_planes(1,1,1);
   
   while (!pack_feof (f))
   {
      chunk = pack_igetl (f);

      switch (chunk)
      {
         case ATED_END:
            return;
            
         case ATED_BACK:
            load_plane (MapBkgnd, f);
            break;
            
         case ATED_FORE:
            load_plane (MapFrgnd, f);
            break;
            
         case ATED_INFO:
            load_plane (MapInfoPl, f);
            break;
//
// Bottom chunks used in ATED temp file
//
#ifdef ATED_TILE_EDITOR
         case ATED_BACK_UNDO:
            load_plane (undoB, f);
            break;
            
         case ATED_FORE_UNDO:
            load_plane (undoF, f);
            break;
            
         case ATED_INFO_UNDO:
            load_plane (undoI, f);
            break;
            
         case ATED_BACK_CUT:
            load_plane (CutBkgnd, f);
            break;
            
         case ATED_FORE_CUT:
            load_plane (CutFrgnd, f);
            break;
            
         case ATED_INFO_CUT:
            load_plane (CutInfoPl, f);
            break;
#endif
      }
   }
}

#ifdef ATED_TILE_EDITOR
//===================================
// load_chunk_temp
//
// This chunk is used in ATED temp file
//===================================
void load_chunk_temp (PACKFILE *f)
{
   xbase = pack_igetl (f);
   ybase = pack_igetl (f);

   planeton = pack_igetl (f);
   planemon = pack_igetl (f);
   planeion = pack_igetl (f);
   viewton  = pack_igetl (f);
   viewmon  = pack_igetl (f);
   viewion  = pack_igetl (f);

   which_t  = pack_igetl (f);
   which_tm = pack_igetl (f);
   which_i  = pack_igetl (f);

   SelectMode  = pack_igetl (f);
   TsearchMode = pack_igetl (f);
   BfillMode   = pack_igetl (f);
   PasteMode   = pack_igetl (f);
   FillMode    = pack_igetl (f);

   pack_fread (&TileCopy,   sizeof(TileCopy),   f);
   SelX1 = pack_igetl (f);
   SelY1 = pack_igetl (f);
   SelX2 = pack_igetl (f);
   SelY2 = pack_igetl (f);
   pack_fread(&UndoRegion, sizeof(UndoRegion), f);
}
#endif

void load_tinfo (unsigned *tinfo, PACKFILE *f)
{
   long num  = pack_igetl (f);
   long size = pack_igetl (f);
   int i;
   
   switch (size)
   {
      case 1:
         {
            unsigned char *temp;
            temp = calloc (num, sizeof(char));
            pack_fread (temp, sizeof(char)*num, f);
            for (i = 0; i < num; i++)
               tinfo[i] = temp[i];
            free (temp);
         }
         break;
         
      case 2:
         {
            unsigned short *temp;
            temp = calloc (num, sizeof(short));
            pack_fread (temp, sizeof(short)*num, f);
            for (i = 0; i < num; i++)
               tinfo[i] = temp[i];
            free (temp);
         }
         break;

      case 4:
         pack_fread (tinfo, sizeof(unsigned)*num, f);
         break;
   }
}

//===================================
// load_chunk_tinfo
//
// TileInfo Chunk
//===================================
void load_chunk_tinfo (PACKFILE *f)
{
   int tb = 0, tf = 0, ti = 0;
   long chunk;
   
   while ( !pack_feof (f) )
   {
      chunk = pack_igetl (f);
      
      switch (chunk)
      {
         case ATED_END:
            return;

         case ATED_BACK:
            pack_fread (&tinfo_back[tb].name, 32, f);
            load_tinfo (tinfo_back[tb++].info, f);
            break;
         
         case ATED_FORE:
            pack_fread (&tinfo_fore[tf].name, 32, f);
            load_tinfo (tinfo_fore[tf++].info, f);
            break;
         
         case ATED_INFO:
            pack_fread (&tinfo_info[ti].name, 32, f);
            load_tinfo (tinfo_info[ti++].info, f);
            break;         
      }
   }
}

//===================================
// load_chunk_main
//
// Base map info is here
//===================================
void load_chunk_main (PACKFILE *f)
{
   char temp [128];
   
   pack_fread (&map_name,   128, f);
   pack_fread (&map_author, 128, f);
   pack_fread (&temp,       128, f);
   replace_filename (tilegfx_file, tilegfx_path, temp, sizeof(tilegfx_file));
   mapwidth  = pack_igetl (f);
   mapheight = pack_igetl (f);
}


//===================================
// load_chunk_header
//
// It isn't actually a chunk, but it
// exist in every ATED-written file
//===================================
int load_chunk_header (PACKFILE *f)
{
   if (pack_igetl (f) != ATED_HEADER)
   {
      ustrcpy (ated_error, "Isn't a valid ATED file");
      return 1;
   }
   
   singlemap_version = pack_igetl (f);
   
   if (singlemap_version != ATED_HEADER_VERSION)
   {
      if (singlemap_version == 0x0100) // In 1.00 format there wasn't
         load_chunk_main (f);          // different chunk for map header
      else
      {
         ustrcpy (ated_error, "Incompatible ATED file format version");
         return 2;
      }
   }
   
   return 0;
}

//===================================
// load_singlemap
//===================================
int load_singlemap (char *filename, ATED_CHUNK *chunk)
{
   PACKFILE *f;
   int i;
   long chunk_id;

   if (!chunk)
      chunk = ated_chunk;

   f = pack_fopen (filename, "rp");
   if (!f)
   {
      f = pack_fopen (filename, "r");
      if (!f)
         return 1;
   }
   
   i = load_chunk_header (f);
   if (i)
      return i;

   //
   // Chunks
   //
   while (!pack_feof (f))
   {
      chunk_id = pack_igetl (f);

      for (i = 0; chunk[i].id; i++)
      {
         if (chunk[i].id == chunk_id)
         {
            chunk[i].handler (f);
            break;
         }
      }
   }

   pack_fclose (f);
   return 0;
}

#endif

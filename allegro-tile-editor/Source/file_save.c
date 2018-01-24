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
 *   Map & stuff saving code
 *
 * CHANGES:
 *
 *   13.02.02 - v0.3.0
 *     Rewritten singlemap file format
 */

#ifdef __FILE_C__

void save_tinfo (unsigned *tinfo, long num, PACKFILE *f)
{
   long size, max_size = 1;
   int i;
   unsigned t;

   if (!tinfo)
      return;

   for (i = 0; i < num; i++)
   {
      t = tinfo[i];

      if      (t <= 0xFF)
         size = 1;
      else if (t <= 0xFFFF)
         size = 2;
      else //  t <= 0xFFFFFFFF
         size = 4;

      if (size > max_size)
         max_size = size;
   }

   pack_iputl (num,      f);
   pack_iputl (max_size, f);
   
   switch (max_size)
   {
      case 1:
         {
            unsigned char *temp;
            temp = calloc (num, sizeof(char));
            for (i = 0; i < num; i++)
               temp[i] = tinfo[i];
            pack_fwrite (temp, sizeof(char)*num, f);
            free (temp);
         }
         break;
         
      case 2:
         {
            unsigned short *temp;
            temp = calloc (num, sizeof(short));
            for (i = 0; i < num; i++)
               temp[i] = tinfo[i];
            pack_fwrite (temp, sizeof(short)*num, f);
            free (temp);
         }
         break;

      case 4:
         pack_fwrite (tinfo, sizeof(unsigned)*num, f);
         break;
   }
}

void save_chunk_tinfo (PACKFILE *f)
{
   int i;

   pack_iputl (ATED_CHUNK_TINFO, f);

   for (i = 0; i < MAX_TINFO; i++)
   {
      if(strlen(tinfo_back[i].name))
      {
         pack_iputl (ATED_BACK, f);
         pack_fwrite(&tinfo_back[i].name, 32, f);
         save_tinfo (tinfo_back[i].info, tinfo_back[i].num, f);
      }
      
      if(strlen(tinfo_fore[i].name))
      {
         pack_iputl (ATED_FORE, f);
         pack_fwrite(&tinfo_fore[i].name, 32, f);
         save_tinfo (tinfo_fore[i].info, tinfo_fore[i].num, f);
      }
      
      if(strlen(tinfo_info[i].name))
      {
         pack_iputl (ATED_INFO, f);
         pack_fwrite(&tinfo_info[i].name, 32, f);
         save_tinfo (tinfo_info[i].info, tinfo_info[i].num, f);
      }
   }
   
   pack_iputl (ATED_END, f);
}

void save_chunk_main (PACKFILE *f)
{
   char temp [128] = "";
      
   pack_iputl (ATED_CHUNK_MAIN, f);
   
   pack_fwrite (&map_name,   128, f);
   pack_fwrite (&map_author, 128, f);
   strcpy (temp, get_filename (tilegfx_file));
   pack_fwrite (&temp,       128, f);
   pack_iputl (mapwidth,  f);
   pack_iputl (mapheight, f);
}

void save_chunk_header (PACKFILE *f)
{
   pack_iputl (ATED_HEADER, f);
   pack_iputl (ATED_HEADER_VERSION, f);
}

void save_plane (unsigned *plane, long id, PACKFILE *f)
{
   int  x, y;
   long max_size = 1, size;
   unsigned t;

   if (!plane)
      return;

   //
   // Optimizing plane
   //

   //
   // Finding max suitable tile size
   //
   for (y = 0; y < mapheight; y++)
   {
      for (x = 0; x < mapwidth; x++)
      {
         t = plane[y*mapwidth + x];
         if      (t <= 0xFF)
            size = 1;
         else if (t <= 0xFFFF)
            size = 2;
         else //  t <= 0xFFFFFFFF
            size = 4;
            
         if (size > max_size)
            max_size = size;
      }
   }

   //
   // Writing data...
   //
   pack_iputl (id,       f);
   pack_iputl (max_size, f);

   switch (max_size)
   {
      case 1:
         {
            unsigned char *temp;
            temp = calloc (mapwidth*mapheight, sizeof(char));
            for (y = 0; y < mapheight; y++)
               for (x = 0; x < mapwidth; x++)
                  temp[y*mapwidth + x] = plane[y*mapwidth + x];
            pack_fwrite (temp, sizeof(char)*mapwidth*mapheight, f);
            free (temp);
         }
         break;
         
      case 2:
         {
            unsigned short *temp;
            temp = calloc (mapwidth*mapheight, sizeof(short));
            for (y = 0; y < mapheight; y++)
               for (x = 0; x < mapwidth; x++)
                  temp[y*mapwidth + x] = plane[y*mapwidth + x];
            pack_fwrite (temp, sizeof(short)*mapwidth*mapheight, f);
            free (temp);
         }
         break;
         
      case 4:
         pack_fwrite (plane, sizeof(unsigned)*mapwidth*mapheight, f);
         break;         
   }
}

int save_singlemap (char *filename)
{
   PACKFILE *f;

   f = pack_fopen (filename, "wp");
   if (!f)
      return 1;

   //
   // Header
   //
   save_chunk_header (f);   
   save_chunk_main (f);

   //
   // Planes
   //
   pack_iputl (ATED_CHUNK_PLANES, f);

   save_plane (MapBkgnd,  ATED_BACK, f);
   save_plane (MapFrgnd,  ATED_FORE, f);
   save_plane (MapInfoPl, ATED_INFO, f);

   pack_iputl (ATED_END, f);

   //
   // End
   //

   pack_fclose (f);

   return 0;
}

void save_chunk_temp (PACKFILE *f)
{
   pack_iputl (ATED_CHUNK_TEMP, f);

   
   pack_iputl (xbase, f);
   pack_iputl (ybase, f);

   pack_iputl (planeton, f);
   pack_iputl (planemon, f);
   pack_iputl (planeion, f);
   pack_iputl (viewton, f);
   pack_iputl (viewmon, f);
   pack_iputl (viewion, f);

   pack_iputl (which_t,  f);
   pack_iputl (which_tm, f);
   pack_iputl (which_i,  f);

   pack_iputl (SelectMode,  f);
   pack_iputl (TsearchMode, f);
   pack_iputl (BfillMode,   f);
   pack_iputl (PasteMode,   f);
   pack_iputl (FillMode,    f);

   pack_fwrite(&TileCopy,   sizeof(TileCopy),   f);
   pack_iputl (SelX1, f);
   pack_iputl (SelY1, f);
   pack_iputl (SelX2, f);
   pack_iputl (SelY2, f);
   pack_fwrite(&UndoRegion, sizeof(UndoRegion), f);
}

int save_tempfile (char *filename)
{
   PACKFILE *f;

   f = pack_fopen (filename, "wp");
   if (!f)
      return 1;

   //
   // Header
   //
   save_chunk_header (f);

   save_chunk_temp (f);   
   save_chunk_main (f);

   //
   // Planes
   //
   pack_iputl (ATED_CHUNK_PLANES, f);

   save_plane (MapBkgnd,  ATED_BACK, f);
   save_plane (MapFrgnd,  ATED_FORE, f);
   save_plane (MapInfoPl, ATED_INFO, f);

   save_plane (CutBkgnd,  ATED_BACK_CUT, f);
   save_plane (CutFrgnd,  ATED_FORE_CUT, f);
   save_plane (CutInfoPl, ATED_INFO_CUT, f);
   
   save_plane (undoB, ATED_BACK_UNDO, f);
   save_plane (undoF, ATED_FORE_UNDO, f);
   save_plane (undoI, ATED_INFO_UNDO, f);
   
   pack_iputl (ATED_END, f);

   //
   // TileInfo
   //
   save_chunk_tinfo (f);
   
   //
   // End
   //

   pack_fclose (f);

   return 0;
}

int save_tinfofile (char *filename)
{
   PACKFILE *f;

   f = pack_fopen (filename, "wp");
   if (!f)
      return 1;

   //
   // Header
   //
   save_chunk_header (f);

   //
   // TileInfo
   //
   save_chunk_tinfo (f);
   
   //
   // End
   //
   pack_fclose (f);

   return 0;
}

#endif

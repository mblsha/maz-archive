/*         ______   ___    ___
 *        /\  _  \ /\_ \  /\_ \ 
 *        \ \ \L\ \\//\ \ \//\ \      __     __   _ __   ___ 
 *         \ \  __ \ \ \ \  \ \ \   /'__`\ /'_ `\/\`'__\/ __`\
 *          \ \ \/\ \ \_\ \_ \_\ \_/\  __//\ \L\ \ \ \//\ \L\ \
 *           \ \_\ \_\/\____\/\____\ \____\ \____ \ \_\\ \____/
 *            \/_/\/_/\/____/\/____/\/____/\/___L\ \/_/ \/___/
 *                                           /\____/
 *                                           \_/__/
 *
 *      Unicode text format conversion utility.
 *
 *      By Shawn Hargreaves.
 *
 *      See readme.txt for copyright information.
 */


#define USE_CONSOLE

#include <stdio.h>

#include <allegro.h>
#include <allegro/internal/aintern.h>


int ustrnicmp (const char *s1, const char *s2, int n)
{
   char buf1[1024];
   char buf2[1024];

   ustrncpy (buf1, s1, n);
   usetat   (buf1, n, 0);

   ustrcpy (buf1, ustrlwr (buf1));


   ustrncpy (buf2, s2, n);
   usetat   (buf2, n, 0);

   ustrcpy (buf2, ustrlwr (buf2));
   
   return ustrncmp (buf1, buf2, n);
}

char *in_name = NULL;
char *out_name = NULL;

int in_format = 0;
int out_format = 0;

int in_flip = FALSE;
int out_flip = FALSE;

UTYPE_INFO *in_type = NULL;
UTYPE_INFO *out_type = NULL;

int flag_c = FALSE;
int flag_hex = FALSE;
int flag_dos = FALSE;
int flag_unix = FALSE;
int flag_verbose = FALSE;
int flag_watermark = FALSE;

int convert_html = FALSE;
int convert_tex  = FALSE;

char *in_data = NULL;
int in_size = 0;
int in_pos = 0;

char *cp_table = NULL;



void usage(void)
{
/*   printf("\nUnicode text format conversion utility for Allegro "
 ALLEGRO_VERSION_STR ", " ALLEGRO_PLATFORM_STR);
   printf("\nBy Shawn Hargreaves, " ALLEGRO_DATE_STR);
   printf("\nalt=\"\" stripping and Hex Unicode conversion abilities by MAZsoft,
 2002");
   printf("\n\n");
 */
   printf("Usage: 'textconv [options] -i{type} [inputfile] -o{type}
 [outputfile]'\n");
   printf("\n");
   printf("Character encoding {type} codes:\n");
   printf("\ta - 8 bit ASCII (Latin-1 codepage)\n");
   printf("\tb - 7 bit bare ASCII or codepage loaded via -f\n");
   printf("\tu - UTF-8 encoding\n");
   printf("\tw - 16 bit Unicode (current machine endianess)\n");
   printf("\tW - 16 bit Unicode (flip endianess)\n");
   printf("\n");
   printf("Option flags:\n");
   printf("\t-f{file} = load alternate codepage file\n");
   printf("\t-c = output data in C string format (default is binary)\n");
   printf("\t-h = output data in hex format (default is binary)\n");
   printf("\t-d = output line breaks in DOS (CR/LF) format\n");
   printf("\t-u = output line breaks in Unix (LF) format\n");
   printf("\t-w = add an endianess watermark to Unicode output files\n");
   printf("\t-v = verbose output\n");
   printf("\t-H = strip 'alt' and convert Hex Unicode characters\n");
   printf("\t-T = do the TeX conversion to support russian\n");
   printf("\n");
   printf("If filenames are not specified, stdin and stdout will be used\n");
}



UTYPE_INFO *get_format_info(int type, char **desc, int *flip)
{
   switch (type) {

      case 'a': 
         *desc = "ASCII (ISO Latin-1)";
         *flip = FALSE;
         return _find_utype(U_ASCII);

      case 'b': 
         *desc = "ASCII (bare 7 bits)";
         *flip = FALSE;
         return _find_utype(U_ASCII_CP);

      case 'u': 
         *desc = "UTF-8 encoding";
         *flip = FALSE;
         return _find_utype(U_UTF8);

      case 'w': 
         *desc = "Unicode (default endianess)";
         *flip = FALSE;
         return _find_utype(U_UNICODE);

      case 'W': 
         *desc = "Unicode (flip endianess)";
         *flip = TRUE;
         return _find_utype(U_UNICODE);
   }

   return NULL;
}



int read_input_file(void)
{
   FILE *f;
   int alloc, c; 

   if (in_name)
   {
      f = fopen(in_name, "rb");
      if (!f)
         return 1;
   }
   else
      f = stdin;

   alloc = 0;

   while ((c = fgetc(f)) != EOF)
   {
      if (in_size >= alloc)
      {
         alloc = in_size+1024;
         in_data = realloc(in_data, alloc);
         if (!in_data)
         {
            fclose(f);
            return 1;
         }
      }

      in_data[in_size++] = c;
   }

   if (in_name)
      fclose(f);

   return 0;
}



int get_input(void)
{
   int c;

   if (in_pos >= in_size)
      return EOF;

   c = in_type->u_getc(in_data + in_pos);
   in_pos += in_type->u_width(in_data + in_pos);

   if (in_pos > in_size)
      return EOF;

   if (in_flip)
      c = ((c&0xFF)<<8) | ((c&0xFF00)>>8);

   return c;
}



void write_output(FILE *f, int c)
{
   static int warned = FALSE;
   static int count = 0;
   static int sstate = -1;
   unsigned char buf[16];
   int size, i;

   if (!out_type->u_isok(c)) {
      if (!warned)
      {
         fprintf(stderr, "Warning: lossy reduction to output format\n");
         warned = TRUE;
      }
      c = '^';
   }

   if (out_flip)
      c = ((c&0xFF)<<8) | ((c&0xFF00)>>8);

   size = out_type->u_setc(buf, c);

   for (i = 0; i < size; i++) {
      if (flag_c)
      {
         /* output in C-string format */
         switch (buf[i])
         {

            case '\r':
               fputs("\\r", f);
               sstate = 0;
               break;

            case '\n':
               fputs("\\n", f);
               sstate = 0;
               break;

            case '\t':
               fputs("\\t", f);
               sstate = 0;
               break;

            case '\"':
               fputs("\\\"", f);
               sstate = 0;
               break;

            case '\\':
               fputs("\\\\", f);
               sstate = 0;
               break;

            default:
               if ((buf[i] >= 32) && (buf[i] < 127))
               {
                  if (sstate == 1)
                  fputs("\" \"", f);

                  putc(buf[i], f);
                  sstate = 0;
               }
               else
               {
                  fprintf(f, "\\x%02X", buf[i]);
                  sstate = 1;
               }
               break;
         }
      }
      else if (flag_hex)
      {
         /* output in hex format */
         if (count&7)
         putc(' ', f);

         fprintf(f, "0x%02X,", buf[i]);

         if ((count&7) == 7)
            putc('\n', f);
      }
      else
      {
         /* output in binary format */
         putc(buf[i], f);
      }

      count++;
   }
}

unsigned short codepage_ru1251[] =
{
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F,
   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
 0x1D, 0x1E, 0x1F,
   0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C,
 0x2D, 0x2E, 0x2F,
   0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C,
 0x3D, 0x3E, 0x3F,
   0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
 0x4D, 0x4E, 0x4F,
   0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C,
 0x5D, 0x5E, 0x5F,
   0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
 0x6D, 0x6E, 0x6F,
   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C,
 0x7D, 0x7E, 0x7F,
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E, 
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E, 
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x401, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E, 0x5E,
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x451, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E, 0x5E,   
   0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A,
 0x41B, 0x41C, 0x41D, 0x41E, 0x41F,
   0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A,
 0x42B, 0x42C, 0x42D, 0x42E, 0x42F,   
   0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A,
 0x43B, 0x43C, 0x43D, 0x43E, 0x43F,
   0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A,
 0x44B, 0x44C, 0x44D, 0x44E, 0x44F,
};

unsigned short codepage_ru866[] =
{
   0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C,
 0x0D, 0x0E, 0x0F,
   0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C,
 0x1D, 0x1E, 0x1F,
   0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C,
 0x2D, 0x2E, 0x2F,
   0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C,
 0x3D, 0x3E, 0x3F,
   0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C,
 0x4D, 0x4E, 0x4F,
   0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59, 0x5A, 0x5B, 0x5C,
 0x5D, 0x5E, 0x5F,
   0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C,
 0x6D, 0x6E, 0x6F,
   0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79, 0x7A, 0x7B, 0x7C,
 0x7D, 0x7E, 0x7F,   
   0x410, 0x411, 0x412, 0x413, 0x414, 0x415, 0x416, 0x417, 0x418, 0x419, 0x41A,
 0x41B, 0x41C, 0x41D, 0x41E, 0x41F,
   0x420, 0x421, 0x422, 0x423, 0x424, 0x425, 0x426, 0x427, 0x428, 0x429, 0x42A,
 0x42B, 0x42C, 0x42D, 0x42E, 0x42F,   
   0x430, 0x431, 0x432, 0x433, 0x434, 0x435, 0x436, 0x437, 0x438, 0x439, 0x43A,
 0x43B, 0x43C, 0x43D, 0x43E, 0x43F,   
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E,   
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E, 
   0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E,   
   0x440, 0x441, 0x442, 0x443, 0x444, 0x445, 0x446, 0x447, 0x448, 0x449, 0x44A,
 0x44B, 0x44C, 0x44D, 0x44E, 0x44F,   
   0x401, 0x451, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E, 0x5E,
 0x5E, 0x5E, 0x5E, 0x5E,
};

int load_codepage(char *filename)
{
   FILE *f;
   int alloc, c, size;

   if (!stricmp (filename, "866"))
   {
      set_ucodepage(codepage_ru866, NULL);
      return 0;
   }
   
   if (!stricmp (filename, "1251"))
   {
      set_ucodepage(codepage_ru1251, NULL);
      return 0;
   }

   f = fopen(filename, "rb");

   if (!f)
      return -1;

   /* format of codepage file:
      table = 256 * unicode value (unsigned short)
      extras = X * [unicode value (unsigned short), codepage value (unsigned
 short)]
      if there are any, file must end with bytes 0,0
    */

   alloc = size = 0;

   while ((c = fgetc(f)) != EOF) {
      if (size >= alloc) {
         alloc = size+1024;
         cp_table = realloc(cp_table, alloc);
         if (!cp_table) {
            fclose(f);
            return 1;
         }
      }

      cp_table[size++] = c;
   }

   set_ucodepage((unsigned short *)cp_table, (size > 256 * (int)sizeof(unsigned
 short)) ? (unsigned short *)cp_table + 256 : NULL);

   fclose(f);

   return 0;
}



int main(int argc, char *argv[])
{
   FILE *out_file;
   char *in_desc, *out_desc;
   int was_cr = FALSE;
   int err = 0;
   int c;
   char buf[16];
   char *dc;
   int len;

   install_allegro(SYSTEM_NONE, &errno, atexit);

   for (c=1; c<argc; c++)
   {
      if (argv[c][0] == '-')
      {
         switch (argv[c][1])
         {

            case 'i':
               if (in_format)
               {
                  usage();
                  return 1;
               }
               in_format = argv[c][2];
               if ((c<argc-1) && (argv[c+1][0] != '-'))
               in_name = argv[++c];
               break;

            case 'o':
               if (out_format)
               {
                  usage();
                  return 1;
               }
               out_format = argv[c][2];
               if ((c<argc-1) && (argv[c+1][0] != '-'))
               out_name = argv[++c];
               break;

            case 'f':
               if (load_codepage(argv[c]+2) != 0)
               {
                  fprintf(stderr, "Error reading %s\n", argv[c]+2);
                  return 1;
               }
               break;

            case 'c':
               flag_c = TRUE;
               break;

            case 'h':
               flag_hex = TRUE;
               break;

            case 'd':
               flag_dos = TRUE;
               break;

            case 'u':
               flag_unix = TRUE;
               break;

            case 'v':
               flag_verbose = TRUE;
               break;

            case 'w':
               flag_watermark = TRUE;
               break;

            case 'H':
               convert_html = TRUE;
               break;

            case 'T':
               convert_tex = TRUE;
               break;

            default:
               fprintf(stderr, "Unknown option '%s'\n", argv[c]);
               return 1;
         }
      }
      else
      {
         usage();
         return 1;
      }
   }

   if ((!in_format) || (!out_format))
   {
      usage();
      return 1;
   }

   in_type = get_format_info(in_format, &in_desc, &in_flip);

   if (!in_type)
   {
      fprintf(stderr, "Unknown input format %c\n", in_format);
      return 1;
   }

   out_type = get_format_info(out_format, &out_desc, &out_flip);

   if (!out_type)
   {
      fprintf(stderr, "Unknown output format %c\n", out_format);
      return 1;
   }

   if (((flag_dos) && (flag_unix)) || ((flag_c) && (flag_hex)))
   {
      fprintf(stderr, "Invalid option combination\n");
      return 1;
   }

   if (flag_verbose)
   {
      printf("Reading %s as %s (%d)\n", (in_name) ? in_name : "{stdin}",
 in_desc, in_size);
      printf("Writing %s as %s\n", (out_name) ? out_name : "{stdout}",
 out_desc);

      if (flag_c)
         printf("Output in C-language string format\n");
      else if (flag_hex)
         printf("Output in hexadecimal format\n");
   }

   if (read_input_file() != 0)
   {
      fprintf(stderr, "Error reading %s\n", in_name);
      return 1;
   }
   
   if (out_name)
   {
      out_file = fopen(out_name, ((flag_c) || (flag_hex)) ? "wt" : "wb");
      if (!out_file)
      {
         fprintf(stderr, "Error writing %s\n", out_name);
         err = 1;
         goto getout;
      }
   }
   else
      out_file = stdout;
      
   if (flag_c)
      putc('"', out_file);

   if (flag_watermark)
   {
      if (flag_verbose)
         printf("Adding Unicode endianess watermark\n");
      write_output(out_file, 0xFEFF);
   }

   while ((c = get_input()) != EOF)
   {
      switch (c)
      {
         case '\n':
            if ((flag_dos) && (!was_cr))
               write_output(out_file, '\r');
            was_cr = FALSE;
            break;

         case '\r':
            was_cr = TRUE;
            if (flag_unix)
               continue;
            break;

         case 0xFEFF:
            if (flag_verbose)
               printf("Endianess watermark ok\n");
            was_cr = FALSE;
            continue;

         case 0xFFFE:
            if (flag_verbose)
               printf("Bad endianess watermark! Toggling input format...\n");
            in_flip = !in_flip;
            was_cr = FALSE;
            continue;


         //
         // ALT="" stripping
         //
         case 'A':
         case 'a':
            if (!convert_html)
               break;
               
            if (!ustrnicmp (in_data + in_pos - ucwidth(c), "ALT=\"",
 ustrlen("ALT=\"")))
            {
               int brackets = 0;
               
               while ((c = get_input()) != EOF)
               {
                  if (c == '\"')
                  {
                     if (++brackets == 2)
                     {
                        goto next_char;
                     }                     
                  }
               }
            }
            break;

         //
         // unicode2codepage conversion
         //
         case '&':
            if (!convert_html)
               break;

            if (!ustrnicmp (in_data + in_pos - ucwidth(c), "&#x", ustrlen
 ("&#x")))
            {
               char *endp;

               in_pos += 2;
               ustrcpy  (buf, "0x");

               dc = ustrchr (in_data + in_pos, ';');
               len = abs(in_data + in_pos - dc);

               ustrncat (buf, in_data + in_pos, len);

               in_pos += len + 1;

               c = ustrtol (buf, &endp, 16);
            }
            break;

         //
         // <BR/> stripping
         //
         case '<':
            if (!convert_html)
               break;

            if (!ustrnicmp (in_data + in_pos - ucwidth(c), "<BR/>", ustrlen
 ("<BR/>")))
            {
               in_pos += 4;

               c = ' ';
            }
            break;

 			//
         // TeX converttion
         //
         case '\\':
         	if (!convert_tex)
            	break;

          	//
            // here we will add some initialication tags to support russian
            //
				if (!ustrnicmp (in_data + in_pos - ucwidth(c), "\\begin{document}", ustrlen
("\\begin{document}")))
				{
					char out_text[] = "\n"
                                 "\\usepackage[margin=1.0cm]{geometry}"
               						"\n"
               						"\\usepackage[cp1251]{inputenc}\n"
               						"\\usepackage[russian]{babel}\n"
                                 "\n"
                                 "\\begin{document}\n";
     				int i;

					for (i = 0; i < sizeof(out_text); i++)
               	write_output (out_file, out_text[i]);

            	in_pos += ustrlen ("\\begin{document}") - 1;

               c = ' ';
            }

            //
            // convert SWP-style Unicode'd cyrillic characters
            //
            if (!ustrnicmp (in_data + in_pos - ucwidth(c), "\\U{", ustrlen
("\\U{")))
				{
            	char *endp;

               in_pos += 2;
               ustrcpy  (buf, "0x");

               dc = ustrchr (in_data + in_pos, '}');
               len = abs(in_data + in_pos - dc);

               ustrncat (buf, in_data + in_pos, len);

               in_pos += len + 1;

               c = ustrtol (buf, &endp, 16);
            }

				break;

         default:
            was_cr = FALSE;
            break;
      }

      write_output(out_file, c);

next_char:

   }

   if (flag_c)
      putc('"', out_file);

   if ((flag_c) || (flag_hex))
      putc('\n', out_file);

   getout:

   if (in_data)
      free(in_data);

   if (cp_table)
      free(cp_table);

   if ((out_name) && (out_file))
      fclose(out_file);

   return err;
}
END_OF_MAIN();

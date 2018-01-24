#ifdef __SOUND_CPP__

#define DATASZ  (1<<15) /* (32768) amount of data to read from disk each time */
#define BUFSZ   (1<<16) /* (65536) size of audiostream buffer */


typedef struct
{
   PACKFILE *f;
   MP3STREAM *s;
} MP3FILE;


MP3FILE *open_mp3_file(char *filename)
{
   MP3FILE   *p = NULL;
   PACKFILE  *f = NULL;
   MP3STREAM *s = NULL;
   char data[DATASZ];
   int len;

   if (!(p = (MP3FILE *)malloc(sizeof(MP3FILE))))
   {
      console.printf (con_error, "Out of memory!\n");
      goto error;
   }
   if (!(f = pack_fopen(filename, "r")))
   {
      console.printf (con_error, "Can't open \"%s\" mp3 file!\n", filename);
      goto error;
   }
   if ((len = pack_fread(data, DATASZ, f)) <= 0)
   {
      console.printf (con_error, "Error reading \"%s\" mp3 file!\n", filename);
      goto error;
   }
   if (len < DATASZ)
   {
      if (!(s = create_mp3stream(data, len, TRUE)))
      {
         console.printf (con_error, "Can't create mp3stream!\n");
         goto error;
      }
   }
   else
   {
      if (!(s = create_mp3stream(data, DATASZ, FALSE)))
      {
         console.printf (con_error, "Can't create mp3stream!\n");
         goto error;
      }
   }

   p->f = f;
   p->s = s;
   return p;

error:

   pack_fclose(f);
   free(p);
   return NULL;
}


int play_mp3_file(MP3FILE *mp3, int buflen, int pan)
{
   return play_mp3stream(mp3->s, buflen, (int)music_vol.value, pan);
}

/*
void resume_mp3(MP3FILE *mp3)
{
  MP3STREAM  *s = mp3->s;
  int        samples;

  samples = (s->outbytes_per_frame * s->frames_per_poll) / (s->stereo ? 2 : 1) / (s->bits / 8);
  s->audiostream = play_audio_stream(samples, s->bits, s->stereo, s->freq, (int)music_vol.value, 128);
}
*/
void close_mp3_file(MP3FILE *mp3)
{
   if (mp3)
   {
      pack_fclose(mp3->f);
      destroy_mp3stream(mp3->s);
      free(mp3);
   }
}


int poll_mp3_file(MP3FILE *mp3)
{
   char *data;
   long len;

   data = (char *)get_mp3stream_buffer(mp3->s);
   if (data)
   {
      len = pack_fread(data, DATASZ, mp3->f);
      if (len < DATASZ)
         free_mp3stream_buffer(mp3->s, len);
      else
         free_mp3stream_buffer(mp3->s, -1);
   }

   return poll_mp3stream(mp3->s);
}

#endif

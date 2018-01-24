#ifndef __SOUND_H__
#define __SOUND_H__

extern cvar     sound_vol;
extern cvar     music_vol;

typedef struct SND_DRIVER
{
   char         *name;
   boolean      initialized;
   boolean      playing;

   int          type;

   int          (*init)                 (void);
   void         (*shutdown)             (void);
   boolean      (*extension_supported)  (char *ext);

   // if file==NULL then driver must stop the sound output and return -1
   // 0 - no error
   // 1 - error
   int          (*play_file)            (char *file);
   int          (*play)                 (void *data);
   int          (*poll)                 (int loop);
   void         (*volume)               (int vol);
   void         (*pause)                (void);
   void         (*resume)               (void);
} SND_DRIVER;

#ifdef USE_FMOD
       #include <fmod.h>
#endif
#ifdef USE_ALMP3
       #include <almp3.h>
#endif
#ifdef USE_JGMOD
       #include <jgmod.h>
#endif

#ifdef USE_MIDI
int     midi_init                       (void);
void    midi_shutdown                   (void);
boolean midi_extension                  (char *ext);
int     midi_play_file                  (char *file);
int     midi_play                       (void *data);
int     midi_poll                       (int   loop);
void    midi_volume                     (int   vol);
#endif

#ifdef USE_FMOD
int     fmod_init                       (void);
void    fmod_shutdown                   (void);
boolean fmod_extension                  (char *ext);
int     fmod_play_file                  (char *file);
int     fmod_play                       (void *data);
int     fmod_poll                       (int   loop);
void    fmod_volume                     (int   vol);
void    fmod_pause                      (void);
void    fmod_resume                     (void);
#endif

#ifdef USE_ALMP3
int     mp3_init                        (void);
void    mp3_shutdown                    (void);
boolean mp3_extension                   (char *ext);
int     mp3_play_file                   (char *file);
int     mp3_play                        (void *data);
int     mp3_poll                        (int   loop);
void    mp3_volume                      (int   vol);
void    mp3_pause                       (void);
void    mp3_resume                      (void);
#endif

#ifdef USE_JGMOD
int     jgmod_init                      (void);
void    jgmod_shutdown                  (void);
boolean jgmod_extension                 (char *ext);
int     jgmod_play_file                 (char *file);
int     jgmod_play                      (void *data);
int     jgmod_poll                      (int   loop);
void    jgmod_volume                    (int   vol);
void    jgmod_pause                     (void);
void    jgmod_resume                    (void);
#endif

void    snd_poll                        (void);
void    snd_init                        (void);
void    snd_shutdown                    (void);
void    play_sound                      (SAMPLE *spl, int pan, int loop);

#endif

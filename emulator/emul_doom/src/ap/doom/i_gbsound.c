//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
// Copyright(C) 2008 David Flater
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	System interface for sound.
//

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#include "deh_str.h"
#include "i_sound.h"
#include "i_system.h"
#include "i_swap.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

#include "doomtype.h"

#define LOW_PASS_FILTER
#define NUM_CHANNELS 16

typedef struct allocated_sound_s allocated_sound_t;


static boolean sound_initialized = false;
static int current_sound_handle = 0;

static boolean use_sfx_prefix;


int use_libsamplerate = 0;


static void GetSfxLumpName(sfxinfo_t *sfx, char *buf, size_t buf_len)
{
    // Linked sfx lumps? Get the lump number for the sound linked to.

    if (sfx->link != NULL)
    {
        sfx = sfx->link;
    }

    // Doom adds a DS* prefix to sound lumps; Heretic and Hexen don't
    // do this.

    if (use_sfx_prefix)
    {
        M_snprintf(buf, buf_len, "ds%s", DEH_String(sfx->name));
    }
    else
    {
        M_StringCopy(buf, DEH_String(sfx->name), buf_len);
    }
}


static void I_SDL_PrecacheSounds(sfxinfo_t *sounds, int num_sounds)
{
    // no-op
  printf("sound : I_SDL_PrecacheSounds\n");
}


//
// Retrieve the raw data lump index
//  for a given SFX name.
//

static int I_SDL_GetSfxLumpNum(sfxinfo_t *sfx)
{
  char namebuf[9];

  GetSfxLumpName(sfx, namebuf, sizeof(namebuf));

  return W_GetNumForName(namebuf);
}

static void I_SDL_UpdateSoundParams(int handle, int vol, int sep)
{
  printf("sound : I_SDL_UpdateSoundParams %d %d %d\n", handle, vol, sep);
}

//
// Starting a sound means adding it
//  to the current list of active sounds
//  in the internal channels.
// As the SFX info struct contains
//  e.g. a pointer to the raw data,
//  it is ignored.
// As our sound handling does not handle
//  priority, it is ignored.
// Pitching (that is, increased speed of playback)
//  is set, but currently not used by mixing.
//
static uint8_t *current_sound_lump = NULL;
static uint8_t *current_sound_pos = NULL;
static unsigned int current_sound_remaining = 0;
static int current_sound_lump_num = -1;


static int I_SDL_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep, int pitch)
{
  printf("sound : I_SDL_StartSound %d %d %d %d\n", channel, vol, sep, pitch);



  unsigned int lumplen;
  int samplerate;
  unsigned int length;
  static int lumpnum = -1;
  static byte *data = NULL;
  static byte *p_out_buf = NULL;

  // need to load the sound
#if 0
  @baram
  if (drvAudioGetReady() == false)
  {
    audioStop();
  }

  if (data != NULL)
  {
    W_ReleaseLumpNum(lumpnum);
  }
  if (p_out_buf != NULL)
  {
    memFree(p_out_buf);
  }
#endif


  lumpnum = sfxinfo->lumpnum;
  data = W_CacheLumpNum(lumpnum, PU_STATIC);
  lumplen = W_LumpLength(lumpnum);

  // Check the header, and ensure this is a valid sound

  current_sound_lump = data;
  current_sound_lump_num = channel;


  if (lumplen < 8
      || data[0] != 0x03 || data[1] != 0x00)
  {
    // Invalid sound

    return false;
  }


  // 16 bit sample rate field, 32 bit length field

  samplerate = (data[3] << 8) | data[2];
  length = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];


  // If the header specifies that the length of the sound is greater than
  // the length of the lump itself, this is an invalid sound lump

  // We also discard sound lumps that are less than 49 samples long,
  // as this is how DMX behaves - although the actual cut-off length
  // seems to vary slightly depending on the sample rate.  This needs
  // further investigation to better understand the correct
  // behavior.

  if (length > lumplen - 8 || length <= 48)
  {
    return false;
  }

  // The DMX sound library seems to skip the first 16 and last 16
  // bytes of the lump - reason unknown.

  data += 16;
  length -= 32;


  //printf("%d, %d, \n", samplerate, length );


  p_out_buf = memMalloc(length * 4);

  if (p_out_buf == NULL)
  {
    return channel;
  }

  int buf_length = 0;
  uint8_t data_out;
  for (int i=0; i<length; i++)
  {
    data_out = data[i] - 0x80;
    p_out_buf[buf_length++] = data_out;
    p_out_buf[buf_length++] = data_out;
    p_out_buf[buf_length++] = data_out;
    p_out_buf[buf_length++] = data_out;
  }

  //audioPlay((uint16_t *)data, length);


  //audioPlay((uint16_t *)p_out_buf, buf_length);  @baram


  current_sound_handle = channel;

  return channel;
}

static void I_SDL_StopSound(int handle)
{
  printf("sound : I_SDL_StopSound\n");

    if (!sound_initialized || handle < 0 || handle >= NUM_CHANNELS)
    {
        return;
    }
    //audioStop(); @baram
}


static boolean I_SDL_SoundIsPlaying(int handle)
{
  printf("sound : I_SDL_SoundIsPlaying\n");

    if (!sound_initialized || handle < 0 || handle >= NUM_CHANNELS)
    {
        return false;
    }

#if 0
    @baram
    if (drvAudioGetReady() == false)
    {
      printf("Playing\n");
      return true;
    }
#endif
    return false;
}

//
// Periodically called to update the sound system
//

static void I_SDL_UpdateSound(void)
{
  printf("sound : I_SDL_UpdateSound\n");
}

static void I_SDL_ShutdownSound(void)
{
  printf("sound : I_SDL_ShutdownSound\n");

    if (!sound_initialized)
    {
        return;
    }



    sound_initialized = false;
}


static boolean I_SDL_InitSound(boolean _use_sfx_prefix)
{
    use_sfx_prefix = _use_sfx_prefix;
    sound_initialized = true;

    printf("sound : I_SDL_InitSound\n");
    return true;
}
#if 1
static snddevice_t sound_sdl_devices[] = 
{
    SNDDEVICE_SB,
    SNDDEVICE_PAS,
    SNDDEVICE_GUS,
    SNDDEVICE_WAVEBLASTER,
    SNDDEVICE_SOUNDCANVAS,
    SNDDEVICE_AWE32,
};
#else
static snddevice_t sound_sdl_devices[] =
{
    SNDDEVICE_PCSPEAKER,
};
#endif

sound_module_t sound_sdl_module = 
{
    sound_sdl_devices,
    arrlen(sound_sdl_devices),
    I_SDL_InitSound,
    I_SDL_ShutdownSound,
    I_SDL_GetSfxLumpNum,
    I_SDL_UpdateSound,
    I_SDL_UpdateSoundParams,
    I_SDL_StartSound,
    I_SDL_StopSound,
    I_SDL_SoundIsPlaying,
    I_SDL_PrecacheSounds,
};


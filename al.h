/*
 * Copyright (c) 2021 Thomas Frohwein
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef AL_H
#define AL_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define MAXSOUNDS	65536
#define NUM_BUFFERS	16
#define BUFFER_SAMPLES	65536
#define NUM_SOURCES	16

typedef enum {
	PLAYING,
	PAUSED,
	STOPPED
} PLAYER_STATUS;

// based on openal-soft's alstream.c example
typedef struct StreamPlayer {
	ALuint buffers[NUM_BUFFERS];
	ALuint source;

	FILE *fp;
	OggVorbis_File *ov_file;
	vorbis_info ov_info;	// contains samplerate
	char *membuf;
	const char *fm_path;	// FMOD internal path

	ALenum format;

	PLAYER_STATUS status;
	bool released;		// FMOD wants source to be free'd when stopped next
	bool retired;		// this is an inactive StreamPlayer for reuse
} StreamPlayer;

typedef struct SoundObject{
	unsigned int n_filepaths;	// number of filepaths in array
	char **filepaths;		// array of filepaths
	const char *path;		// FMOD internal path
} SoundObject;

static SoundObject sounds[MAXSOUNDS];
static unsigned int sound_counter = 0;

static StreamPlayer StreamPlayerArr[MAXSOUNDS];
static unsigned int sp_counter = 0;

int al_init(void);

SoundObject *NewSoundObject(void);
StreamPlayer *NewPlayer(void);

void DeletePlayer(StreamPlayer *player);
void ClosePlayer(StreamPlayer *player);
int StartPlayer(StreamPlayer *player);
int StopPlayer(StreamPlayer *player);
int UpdatePlayer(StreamPlayer *player);
int OpenPlayerFile(StreamPlayer *player, const char *filename);
void ClosePlayerFile(StreamPlayer *player);

#endif // AL_H

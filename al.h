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
#define BUFFER_SIZE	65536
#define BUFFER_SAMPLES	65536
#define NUM_SOURCES	16

// based on openal-soft's alstream.c example
typedef struct StreamPlayer {
	ALuint buffers[NUM_BUFFERS];
	ALuint source;

	FILE *fp;
	OggVorbis_File *ov_file;
	vorbis_info ov_info;
	char *membuf;
	const char *fm_path;	// FMOD internal path

	/* The format of the output stream (sample rate is in ov_info) */
	ALenum format;
} StreamPlayer;

typedef struct SoundObject{
	char *fp;		// filepath
	const char *path;	// FMOD internal path
	bool issample;
} SoundObject;

static SoundObject sounds[MAXSOUNDS];
static unsigned int sound_counter = 0;

static StreamPlayer StreamPlayerArr[MAXSOUNDS];
static unsigned int sp_counter = 0;

static ALuint al_buffers[NUM_BUFFERS];
static ALuint al_sources[NUM_SOURCES];

static int current_buffer;
static int current_source;

int al_init(void);
int al_play(SoundObject *so);
void al_check_error(void);

/*
size_t read_ogg_callback(void* destination, size_t size1, size_t size2, void* fileHandle);
int32_t seek_ogg_callback(void* fileHandle, ogg_int64_t to, int32_t type);
long int tell_ogg_callback(void* fileHandle);
*/

StreamPlayer *NewPlayer(void);
void DeletePlayer(StreamPlayer *player);
void ClosePlayer(StreamPlayer *player);
int StartPlayer(StreamPlayer *player);
int UpdatePlayer(StreamPlayer *player);
int OpenPlayerFile(StreamPlayer *player, const char *filename);
void ClosePlayerFile(StreamPlayer *player);

#endif // AL_H

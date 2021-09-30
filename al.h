#ifndef AL_H
#define AL_H

#include <stdbool.h>
#include <stdlib.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#define MAXSOUNDS	65536
#define NUM_BUFFERS	4
#define BUFFER_SIZE	65536
#define NUM_SOURCES	16

// disable until reviewed
#if 0
struct StreamingAudioData
{
	ALuint buffers[NUM_BUFFERS];
	char *filename;
	int file;		// file descriptor
	uint8_t channels;
	int32_t sampleRate;
	uint8_t bitsPerSample;
	ALsizei size;
	ALuint source;
	ALsizei sizeConsumed = 0;
	ALenum format;
	OggVorbis_File oggVorbisFile;
	int32_t oggCurrentSection = 0;
	size_t duration;
};
#endif

// based on openal-soft's alstream.c example
typedef struct StreamPlayer {
	ALuint buffers[NUM_BUFFERS];
	ALuint source;

	FILE *fp;
	OggVorbis_File ov_file;
	vorbis_info ov_info;
	short *membuf;

	/* The format of the output stream (sample rate is in sfinfo) */
	ALenum format;
} StreamPlayer;

typedef struct SoundObject{
	const char *fp;		// filepath
	vorbis_info *vi;
	char *handle;
	size_t size;
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
SoundObject *al_load (char *filepath);
int al_play(SoundObject *so);
void al_check_error(void);
//int playOgg (char *eventPath);
size_t read_ogg_callback(void* destination, size_t size1, size_t size2, void* fileHandle);
int32_t seek_ogg_callback(void* fileHandle, ogg_int64_t to, int32_t type);
long int tell_ogg_callback(void* fileHandle);
static inline ALenum to_al_format(short channels);

//from openal-soft's alstream.c
static StreamPlayer *NewPlayer(void);
static void DeletePlayer(StreamPlayer *player);
static void ClosePlayer(StreamPlayer *player);
static int StartPlayer(StreamPlayer *player);
static int UpdatePlayer(StreamPlayer *player);
static int OpenPlayerFile(StreamPlayer *player, const char *filename);
static void ClosePlayerFile(StreamPlayer *player);

#endif // AL_H

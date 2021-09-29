#ifndef AL_H
#define AL_H

#include <stdlib.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

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

typedef struct {
	const char *fp;		// filepath
	vorbis_info *vi;
	char *handle;
	size_t size;
} vorbis_object;

int al_init(void);
vorbis_object *al_load (char *filepath);
int al_play(vorbis_object *vo);
void al_check_error(void);
//int playOgg (char *eventPath);
size_t read_ogg_callback(void* destination, size_t size1, size_t size2, void* fileHandle);
int32_t seek_ogg_callback(void* fileHandle, ogg_int64_t to, int32_t type);
long int tell_ogg_callback(void* fileHandle);
static inline ALenum to_al_format(short channels);

#endif // AL_H

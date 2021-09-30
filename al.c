/*
 * https://gist.github.com/progschj/3885932
 * https://ffainelli.github.io/openal-example/
 * https://indiegamedev.net/2020/01/16/how-to-stream-ogg-files-with-openal-in-c/
 */

#include "al.h"

static inline ALenum to_al_format(short channels)
{
	if (channels == 1)
		return AL_FORMAT_MONO16;
	else return AL_FORMAT_STEREO16;
}

void al_check_error(void)
{
	ALenum error = alGetError();
	switch(error)
	{
		case AL_INVALID_NAME: fprintf(stderr,"invalid name\n"); return;
		case AL_INVALID_ENUM: fprintf(stderr,"invalid enum\n"); return;
		case AL_INVALID_VALUE: fprintf(stderr,"invalid value\n"); return;
		case AL_INVALID_OPERATION: fprintf(stderr,"invalid operation\n"); return;
		case AL_OUT_OF_MEMORY: fprintf(stderr,"out of memory\n"); return;
		default: return;
	}
}

int al_init(void)
{
	alutInit (NULL, NULL);
	alGenBuffers(NUM_BUFFERS, al_buffers);
	alGenSources (NUM_SOURCES, al_sources);
	return 0;
}

SoundObject *al_load (char *filepath)
{
	OggVorbis_File vf;
	int current_section;
	SoundObject *out = (SoundObject *)malloc(sizeof(SoundObject));;
	// TODO: free() later
	FILE *fp = fopen(filepath, "rb");
	if(fp == NULL)
	{
		fprintf(stderr, "could not open file %s\n", filepath);
		exit(1);
	}
	if(ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_NOCLOSE)<0)
	{
		fprintf(stderr, "input does not appear to be in an ogg bitstream\n");
		exit(1);
	}
	out->fp = filepath;
	out->vi = ov_info(&vf, -1);
	out->size = ov_pcm_total(&vf, -1) * out->vi->channels * 2; // 2 for 16 bit
	fprintf(stderr, "rate %ld channels: %d bytes: %ld\n", out->vi->rate, out->vi->channels, (long)out->size);
	char *pcmout = malloc(out->size);	// TODO: replace magic number
	long pos = 0;
	
	while(pos < out->size)
	{
		long ret = ov_read(&vf, pcmout+pos, 4096, 0, 2, 1, &current_section);
		pos+=ret;
		if(ret == 0)
		{
			break;
		}
	}
	out->handle = pcmout;
	fclose(fp);
	return out;
}

int al_play(SoundObject *so)
{
	alBufferData(al_buffers[current_buffer], to_al_format(so->vi->channels), so->handle, so->size, so->vi->rate);
	al_check_error();
	alSourceQueueBuffers(al_sources[current_source], 1, &al_buffers[current_buffer]);
	al_check_error();
	alSourcePlay(al_sources[current_source]);
	al_check_error();
	current_buffer++;
	if (++current_buffer >= NUM_BUFFERS)
		current_buffer = 0;
	if (++current_source >= NUM_SOURCES)
		current_source = 0;
	/*
	int eof = 0;
	while(!eof)
	{
		ALuint released[16];
		ALint count;
		alGetSourcei(al_sources, AL_BUFFERS_PROCESSED, &count);
		alSourceUnqueueBuffers(al_sources, count, released);
		
		for(int i = 0;i<count;++i)
		{
			long pos = 0;
			while(pos < so->bytes)
			{
				long ret = ov_read(&memfile, so->handle+pos, sizeof(so->handle)-pos, 0, 2, 1, &current_section);
				pos+=ret;
				if(ret == 0)
				{
					eof = 1;
					break;
				}
			}
			alBufferData(released[i], to_al_format(so->vi->channels), pcmout, pos, so->vi->rate);
		}
		alSourceQueueBuffers(al_sources, count, released);
		alutSleep(1/20.);
	}
	*/
//	alutExit ();
	return EXIT_SUCCESS;
}

// functions borrowed from C++ (https://indiegamedev.net/2020/01/16/how-to-stream-ogg-files-with-openal-in-c/) disabled for now
#if 0
size_t read_ogg_callback(void* destination, size_t size1, size_t size2, void* fileHandle)
{
    StreamingAudioData* audioData = (StreamingAudioData*)fileHandle;

    ALsizei length = size1 * size2;

    if(audioData->sizeConsumed + length > audioData->size)
        length = audioData->size - audioData->sizeConsumed;

    if(!audioData->file.is_open())
    {
        audioData->file.open(audioData->filename, std::ios::binary);
        if(!audioData->file.is_open())
        {
            fprintf(stderr, "ERROR: Could not re-open streaming file \"%s\"\n", audioData->filename);
            return 0;
        }
    }

    char* moreData = new char[length];

    audioData->file.clear();
    audioData->file.seekg(audioData->sizeConsumed);
    if(!audioData->file.read(&moreData[0],length))
    {
        if(audioData->file.eof())
        {
            audioData->file.clear(); // just clear the error, we will resolve it later
        }
        else if(audioData->file.fail())
        {
            fprintf(stderr, "ERROR: OGG stream has fail bit set %s\n", audioData->filename);
            audioData->file.clear();
            return 0;
        }
        else if(audioData->file.bad())
        {
            fprintf(stderr, "ERROR: OGG stream has bad bit set %s\n", audioData->filename);
            audioData->file.clear();
            return 0;
        }
    }
    audioData->sizeConsumed += length;

    memcpy(destination, &moreData[0], length);

    delete[] moreData;	// replace with free() on the array?

    audioData->file.clear();

    return length;
}

int32_t seek_ogg_callback(void* fileHandle, ogg_int64_t to, int32_t type)
{
    StreamingAudioData* audioData = (StreamingAudioData*)fileHandle;

    if(type == SEEK_CUR)
    {
        audioData->sizeConsumed += to;
    }
    else if(type == SEEK_END)
    {
        audioData->sizeConsumed = audioData->size - to;
    }
    else if(type == SEEK_SET)
    {
        audioData->sizeConsumed = to;
    }
    else
        return -1; // what are you trying to do vorbis?

    if(audioData->sizeConsumed < 0)
    {
        audioData->sizeConsumed = 0;
        return -1;
    }
    if(audioData->sizeConsumed > audioData->size)
    {
        audioData->sizeConsumed = audioData->size;
        return -1;
    }

    return 0;
}

long int tell_ogg_callback(void* fileHandle)
{
    StreamingAudioData* audioData = (StreamingAudioData*)fileHandle;
    return audioData->sizeConsumed;
}
#endif

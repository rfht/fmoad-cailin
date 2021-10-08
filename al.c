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

#include "al.h"

int al_init(void)
{
	const ALCchar *name;
	ALCdevice *device;
	ALCcontext *ctx;

	device = NULL;
	device = alcOpenDevice(NULL);	// select the default device
	if(!device)
	{
		fprintf(stderr, "Could not open the OpenAL device!\n");
		return 1;
	}
	ctx = alcCreateContext(device, NULL);
	if(ctx == NULL || alcMakeContextCurrent(ctx) == ALC_FALSE)
	{
		if(ctx != NULL)
			alcDestroyContext(ctx);
		alcCloseDevice(device);
		fprintf(stderr, "Could not set a context!\n");
		return 1;
	}
	name = NULL;
	if(alcIsExtensionPresent(device, "ALC_ENUMERATE_ALL_EXT"))
		name = alcGetString(device, ALC_ALL_DEVICES_SPECIFIER);
	if(!name || alcGetError(device) != AL_NO_ERROR)
		name = alcGetString(device, ALC_DEVICE_SPECIFIER);
	printf("Opened \"%s\"\n", name);

	return 0;
}

// from openal-soft's alstream.c example
StreamPlayer *NewPlayer(void)
{
	StreamPlayer *player;
	player = malloc(sizeof(*player));
	assert(player != NULL);
	player->status = STOPPED;
	player->released = false;
	player->retired = false;
	alGenBuffers(NUM_BUFFERS, player->buffers);
	assert(alGetError() == AL_NO_ERROR && "Could not create buffers");
	alGenSources(1, &player->source);
	assert(alGetError() == AL_NO_ERROR && "Could not create source");

	/* Set parameters so mono sources play out the front-center speaker and
	 * won't distance attenuate. */
	alSource3i(player->source, AL_POSITION, 0, 0, -1);
	alSourcei(player->source, AL_SOURCE_RELATIVE, AL_TRUE);
	alSourcei(player->source, AL_ROLLOFF_FACTOR, 0);
	assert(alGetError() == AL_NO_ERROR && "Could not set source parameters");

	return player;
}

SoundObject *NewSoundObject(void)
{
	SoundObject *so;
	so = malloc(sizeof(*so));
	assert(so != NULL);
	so->n_filepaths = 0;
	so->filepaths = NULL;
	so->path = "\0";
	return so;
}

// from openal-soft's alstream.c example
void DeletePlayer(StreamPlayer *player)
{
	alDeleteSources(1, &player->source);
	alDeleteBuffers(NUM_BUFFERS, player->buffers);
	if(alGetError() != AL_NO_ERROR)
		fprintf(stderr, "Failed to delete object IDs\n");
	player->released = true;
	fclose(player->fp);
	free(player->ov_file);
	free(player->membuf);
	//memset(player, 0, sizeof(*player));
	//free(player);
}

// from openal-soft's alstream.c example
int StartPlayer(StreamPlayer *player)
{
	ALsizei i;
	int current_section;

	/* Rewind the source position and clear the buffer queue */
	alSourceRewind(player->source);
	alSourcei(player->source, AL_BUFFER, 0);

	/* Fill the buffer queue */
	for(i = 0; i < NUM_BUFFERS; i++)
	{
		/* Get some data to give it to the buffer */
		long ov_len = ov_read(player->ov_file, player->membuf, BUFFER_SAMPLES, 0, 2, 1, &current_section);
		if(ov_len < 1) break;

		alBufferData(player->buffers[i], player->format, player->membuf, (ALsizei)ov_len,
			player->ov_info.rate);
	}
	if(alGetError() != AL_NO_ERROR)
	{
		fprintf(stderr, "Error buffering for playback\n");
		return 0;
	}

	/* Now queue and start playback! */
	alSourceQueueBuffers(player->source, i, player->buffers);
	alSourcePlay(player->source);
	if(alGetError() != AL_NO_ERROR)
	{
		fprintf(stderr, "Error starting playback\n");
		return 0;
	}

	return 1;
}

int StopPlayer(StreamPlayer *player)
{
	alSourceStop(player->source);
	if (alGetError() != AL_NO_ERROR)
	{
		fprintf(stderr, "Error stopping playback\n");
		return 0;
	}
	return 1;
}

int UpdatePlayer(StreamPlayer *player)
{
	ALint processed, state;
	int current_section;

	/* Get relevant source info */
	alGetSourcei(player->source, AL_SOURCE_STATE, &state);
	alGetSourcei(player->source, AL_BUFFERS_PROCESSED, &processed);
	if(alGetError() != AL_NO_ERROR)
	{
		fprintf(stderr, "Error checking source state\n");
		return 0;
	}

	/* Unqueue and handle each processed buffer */
	while(processed > 0)
	{
		ALuint bufid;
		long ov_len;

		alSourceUnqueueBuffers(player->source, 1, &bufid);
		processed--;

		/* Read the next chunk of data, refill the buffer, and queue it
		 * back on the source */
		ov_len = ov_read(player->ov_file, player->membuf, BUFFER_SAMPLES, 0, 2, 1, &current_section);
		if(ov_len > 0)
		{
			//ov_len *= player->ov_info.channels * (long)sizeof(char);	// this distorts audio
			alBufferData(bufid, player->format, player->membuf, (ALsizei)ov_len,
				player->ov_info.rate);
			alSourceQueueBuffers(player->source, 1, &bufid);
		}
		if(alGetError() != AL_NO_ERROR)
		{
			fprintf(stderr, "Error buffering data\n");
			return 0;
		}
	}

	/* Make sure the source hasn't underrun */
	if(state != AL_PLAYING && state != AL_PAUSED)
	{
		ALint queued;

		/* If no buffers are queued, playback is finished */
		alGetSourcei(player->source, AL_BUFFERS_QUEUED, &queued);
		if(queued == 0)
			return 0;

		alSourcePlay(player->source);
		if(alGetError() != AL_NO_ERROR)
		{
			fprintf(stderr, "Error restarting playback\n");
			return 0;
		}
	}

	return 1;
}

/* Opens the first audio stream of the named file. */
int OpenPlayerFile(StreamPlayer *player, const char *filename)
{
	size_t frame_size;
	player->ov_file = (OggVorbis_File *)malloc(sizeof(OggVorbis_File));

	/* Open the audio file and check that it's usable. */
	player->fp = fopen(filename, "r");
	if (!player->fp)
	{
		fprintf(stderr, "Failed to open %s\n", filename);
		exit(1);
	}
	if(ov_open_callbacks(player->fp, player->ov_file, NULL, 0, OV_CALLBACKS_DEFAULT) < 0)
	{
		fprintf(stderr, "Could not open audio in %s\n", filename);
		return 0;
	}

	player->ov_info = *ov_info(player->ov_file, -1);

	/* Get the sound format, and figure out the OpenAL format */
	if(player->ov_info.channels == 1)
		player->format = AL_FORMAT_MONO16;
	else
		player->format = AL_FORMAT_STEREO16;
	if(!player->format)
	{
		fprintf(stderr, "Unsupported channel count: %d\n", player->ov_info.channels);
		fclose(player->fp);
		player->ov_file = NULL;
		return 0;
	}

	frame_size = (size_t)(BUFFER_SAMPLES * player->ov_info.channels) * sizeof(char);
	player->membuf = malloc(frame_size);

	return 1;
}

/* Closes the audio file stream */
void ClosePlayerFile(StreamPlayer *player)
{
	if(player->fp)
		fclose(player->fp);
	if(player->ov_file)
		player->ov_file = NULL;
	if(player->membuf)
	{
		free(player->membuf);
		player->membuf = NULL;
	}
}

#include <string.h>

/* https://gist.github.com/progschj/3885932 */
#include <stdlib.h>
#include <stdio.h>
#include <AL/alut.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

char pcmout[16*1024];

/* https://ffainelli.github.io/openal-example/ */
static inline ALenum to_al_format(short channels)
{
	if (channels == 1)
		return AL_FORMAT_MONO16;
	else return AL_FORMAT_STEREO16;
}

void check_error()
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

int playOgg (char *eventPath)
{
   OggVorbis_File vf;
   int current_section;
   int eof = 0;
   FILE *fp = fopen(eventPath, "rb");
   if(fp == NULL)
   {
      fprintf(stderr, "could not open file %s", eventPath);
      exit(1);
   }

   if(ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_DEFAULT)<0)
   {
      fprintf(stderr, "input does not appear to be in an ogg bitstream");
      exit(1);
   }

   vorbis_info *vi = ov_info(&vf, -1);
   printf("rate %ld channels: %d\n",vi->rate, vi->channels);

   ALuint source;
   alutInit (NULL, NULL);
   ALuint buffers[16];
   alGenBuffers(16, buffers);
   alGenSources (1, &source);
   
   for(int i = 0;i<16;++i)
   {
      long pos = 0;

      while(pos < sizeof(pcmout))
      {
	 long ret = ov_read(&vf, pcmout+pos, sizeof(pcmout)-pos, 0, 2, 1, &current_section);
	 pos+=ret;
	 if(ret == 0)
	 {
	    eof = 1;
	    break;
	 }
      }
   
      alBufferData(buffers[i], to_al_format(vi->channels), pcmout, pos, vi->rate);
   
   }
   alSourceQueueBuffers(source, 16, buffers);
   alSourcePlay(source);
   check_error();
   while(!eof)
   {
      ALuint released[16];
      ALint count;
      alGetSourcei(source, AL_BUFFERS_PROCESSED, &count);
      alSourceUnqueueBuffers(source, count, released);
    
      for(int i = 0;i<count;++i)
      {
	 long pos = 0;
	 while(pos < sizeof(pcmout))
	 {
	    long ret = ov_read(&vf, pcmout+pos, sizeof(pcmout)-pos, 0, 2, 1, &current_section);
	    pos+=ret;
	    if(ret == 0)
	    {
	       eof = 1;
	       break;
	    }
	 }
	 alBufferData(released[i], to_al_format(vi->channels), pcmout, pos, vi->rate);
      }
      alSourceQueueBuffers(source, count, released);
      alutSleep(1/20.);
   }
   alutExit ();
   return EXIT_SUCCESS;
}

int FMOD_Studio_System_Create()
{
	/*
	FILE *f;
	f = fopen("fmod.log", "w");
	fprintf(f, "FMOD_Studio_System_Create\n");
	*/
	return 0;
}

int FMOD_Studio_System_Initialize()
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_Initialize\n");
	*/

	/*
	dev = alcOpenDevice(NULL);
	if (!dev) {
		printf("ERROR: Couldn't open openal device\n");
		abort();
	}
	context = alcCreateContext(dev, NULL);
	if (!alcMakeContextCurrent(context)) {
		printf("ERROR: Couldn't make context current\n");
		abort();
	}

	alGenSources((ALuint)1, &source);
	// FIXME: check for errors
	alSourcef(source, AL_PITCH, 1);
	// FIXME: check for errors
	alSourcef(source, AL_GAIN, 1);
	// FIXME: check for errors
	alSource3f(source, AL_POSITION, 0, 0, 0);
	// FIXME: check for errors
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	// FIXME: check for errors
	alSourcei(source, AL_LOOPING, AL_FALSE);
	// FIXME: check for errors

	alGenBuffers((ALuint)1, &buffer);
	// FIXME: check for errors

	ALenum format;
	ALsizei freq;
	char bufferData[BUFFER_SIZE];
	ALint state;

	LoadOGG("/home/thfr/games/fnaify/celeste/fmod_complete_project/unzipped/fmodstudio10000celeste-project/FMOD Studio Celeste Project/Assets/ui/world_map/ui_world_chapter_level_select.ogg", &bufferData, &format, &freq);

	alBufferData(buffer, format, &bufferData[0],  bufferData.size(), freq);
	alSourcei(source, AL_BUFFER, buffer);

	alSourcePlay(source);
	do {
		// Query the state of the souce
		alGetSourcei(source, AL_SOURCE_STATE, &state);
	} while (state != AL_STOPPED);
	*/

	return 0;
}

int FMOD_Studio_System_SetListenerAttributes()
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_SetListenerAttributes\n");
	*/
	return 0;
}

int FMOD_Studio_System_Update(int *system)
{
	// Commented out because segfaults because of threading/concurrency
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_Update\n");
	*/
	return 0;
}

int FMOD_Studio_System_LoadBankFile(int *system, char *filename, int *flags, int **bank)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_LoadBankFile: %s\n", filename);
	*/
	return 0;
}

int FMOD_Studio_System_GetVCA(int *system, char *path, int **vca)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_GetVCA: %s\n", path);
	*/
	return 0;
}

int FMOD_Studio_VCA_SetVolume(int *vca, float volume)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_VCA_SetVolume: , %f\n", volume);
	*/
	return 0;
}

int FMOD_Studio_VCA_GetVolume(int *vca, float *volume, float *finalvolume)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_VCA_GetVolume: , %f, %f\n", *volume, *finalvolume);
	*/
	return 0;
}

int FMOD_Studio_System_GetEvent(int *system, char *path, int **event)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_GetEvent: %s, event: %p\n", path, (void *) event);
	if (!strncmp(path, "event:/char/madeline/jump", 26)) {
		*event = (int *) "9";
	} else {
		*event = (int *) "0";
	}
	return 0;
}

int FMOD_Studio_EventDescription_LoadSampleData(int *eventdescription)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventDescription_LoadSampleData: %x, addr: %p\n", *eventdescription, &eventdescription);
	return 0;
}

int FMOD_Studio_EventDescription_CreateInstance(int *eventdescription, int **instance)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventDescription_CreateInstance: descr %x, inst %p\n", *eventdescription, (void *) instance);
	return 0;
}


int FMOD_Studio_EventDescription_Is3D(int *eventdescription, int *is3D)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventDescription_Is3D, %d\n", *is3D);
	*/
	return 0;
}

int FMOD_Studio_EventInstance_Start(int *eventinstance)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Start: x\n"); //, *eventinstance);
	playOgg("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko/sfx-char_mad_death.ogg");
	return 0;
}

int FMOD_Studio_System_GetBus(int *system, char *path, int **bus)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_GetBus, %s\n", path);
	*/
	return 0;
}

int FMOD_Studio_Bus_SetPaused(int *bus, int paused)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_Bus_SetPaused\n");
	*/
	return 0;
}

int FMOD_Studio_Bus_GetPaused(int *bus, int *paused)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_Bus_GetPaused\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_GetDescription(int *eventinstance, int **description)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_GetDescription: inst x, descr p\n"); //, *eventinstance, (void *) description);
	return 0;
}

int FMOD_Studio_EventDescription_GetPath(int *eventdescription, char *path, int size, int *retrieved)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventDescription_GetPath: %s, %d\n", path, size);
	path = "event:/env/amb/worldmap\0";
	*retrieved = 24;
	return 0;
}

int FMOD_Studio_Bank_LoadSampleData(int *bank)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_Bank_LoadSampleData\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_SetVolume(int *eventinstance, float volume)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_SetVolume, %f\n", volume);
	*/
	return 0;
}

int FMOD_Studio_System_GetLowLevelSystem(int *system, int **lowLevelSystem)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_SetVolume, %f\n", volume);
	*/
	return 0;
}

int FMOD_Studio_System_GetListenerAttributes(int *system, int listener, int *attributes)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_GetListenerAttributes\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_Set3DAttributes(int *eventinstance, int *attributes)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Set3DAttributes\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_Release(int *eventinstance)
{
	// Commented out to prevent segfault from concurrency
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Release\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_GetVolume(int *eventinstance, float *volume, float *finalvolume)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_GetVolume: , %a, %a\n", *volume, *finalvolume);
	*/
	return 0;
}

int FMOD_Studio_EventInstance_Stop(int *eventinstance, int mode)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Stop\n");
	return 0;
}

int FMOD_Studio_EventInstance_Get3DAttributes(int *eventinstance, int *attributes)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Get3DAttributes\n");
	*/
	return 0;
}

int FMOD_Studio_System_Release(int *system)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_Release\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_SetParameterValue(int *eventinstance, char *name, float value)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_SetParameterValue: %s, %f\n", name, value);
	*/
	return 0;
}

int FMOD_Studio_EventDescription_IsOneshot(int *eventdescription, int *oneshot)
{
	// Commented out to prevent segfault from concurrency
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventDescription_IsOneshot\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_SetPaused(int *eventinstance, int paused)
{
	/*
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_SetPaused\n");
	*/
	return 0;
}

int FMOD_Studio_EventInstance_TriggerCue(int *eventinstance)
{
	return 0;
}

int FMOD_Studio_Bus_StopAllEvents(int *bus, int mode)
{
	return 0;
}

int FMOD_Studio_EventInstance_GetPaused(int *eventinstance, int *paused)
{
	return 0;
}

int FMOD_Studio_EventInstance_GetPlaybackState(int *eventinstance, int *state)
{
	return 0;
}

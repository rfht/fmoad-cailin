#include "fmoad-cailin.h"
#include "al.h"

FMOD_RESULT FMOD_Studio_System_Create(FMOD_STUDIO_SYSTEM **system, unsigned int headerversion)
{
	// TODO: create new system object at the address **system; log headerversion
	STUB();
}

FMOD_RESULT FMOD_Studio_System_Initialize(FMOD_STUDIO_SYSTEM *system,
	int maxchannels, FMOD_STUDIO_INITFLAGS studioflags,
	FMOD_INITFLAGS flags,
	void *extradriverdata)
{
	// TODO: log/use maxchannels; ignore studioflags, flags , and extradriverdata for now
	STUB();
}

FMOD_RESULT FMOD_Studio_System_SetListenerAttributes(FMOD_STUDIO_SYSTEM *system,
	int listener,
	FMOD_3D_ATTRIBUTES *attributes)
{
	/* listener: the "listener index"
	 * attributes: are 3D attributes of the listener
	 */
	STUB();
}

FMOD_RESULT FMOD_Studio_System_Update(int *system)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_System_LoadBankFile(FMOD_STUDIO_SYSTEM *system,
	const char *filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags,
	FMOD_STUDIO_BANK **bank)
{
	// TODO: get filename
	fprintf(stderr, "filename: %s\n", filename);
	STUB();
}

FMOD_RESULT FMOD_Studio_System_GetVCA(int *system, char *path, int **vca)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_VCA_SetVolume(int *vca, float volume)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_VCA_GetVolume(int *vca, float *volume, float *finalvolume)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_System_GetEvent(int *system, char *path, int **event)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_System_GetEvent: %s, event: %p\n", path, (void *) event);
	// this likely corresponds to: ./Content/FMOD/Desktop/sfx.banko/sfx-char_mad_jump.ogg
	if (!strncmp(path, "event:/char/madeline/jump", 26)) {
		*event = (int *) "9";
	} else {
		*event = (int *) "0";
	}
	return 0;
}

FMOD_RESULT FMOD_Studio_EventDescription_LoadSampleData(int *eventdescription)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventDescription_CreateInstance(int *eventdescription, int **instance)
{
	STUB();
}


FMOD_RESULT FMOD_Studio_EventDescription_Is3D(int *eventdescription, int *is3D)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_Start(int *eventinstance)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Start: x\n"); //, *eventinstance);
	playOgg("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko/sfx-char_mad_death.ogg");
	return 0;
}

FMOD_RESULT FMOD_Studio_System_GetBus(int *system, char *path, int **bus)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_Bus_SetPaused(int *bus, int paused)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_Bus_GetPaused(int *bus, int *paused)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_GetDescription(int *eventinstance, int **description)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_GetDescription: inst x, descr p\n"); //, *eventinstance, (void *) description);
	return 0;
}

FMOD_RESULT FMOD_Studio_EventDescription_GetPath(int *eventdescription, char *path, int size, int *retrieved)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventDescription_GetPath: %s, %d\n", path, size);
	path = "event:/env/amb/worldmap\0";	// corresponds to ./Content/FMOD/Desktop/sfx.banko/sfx-env_amb_worldmap.ogg
	*retrieved = 24;
	return 0;
}

FMOD_RESULT FMOD_Studio_Bank_LoadSampleData(int *bank)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_SetVolume(int *eventinstance, float volume)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_System_GetLowLevelSystem(FMOD_STUDIO_SYSTEM *system, FMOD_SYSTEM **lowLevelSystem)
{
	// get the low level (FMOD) system object
	STUB();
}

FMOD_RESULT FMOD_Studio_System_GetListenerAttributes(int *system, int listener, int *attributes)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_Set3DAttributes(int *eventinstance, int *attributes)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_Release(int *eventinstance)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_GetVolume(int *eventinstance, float *volume, float *finalvolume)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_Stop(int *eventinstance, int mode)
{
	FILE *f;
	f = fopen("fmod.log", "a");
	fprintf(f, "FMOD_Studio_EventInstance_Stop\n");
	return 0;
}

FMOD_RESULT FMOD_Studio_EventInstance_Get3DAttributes(int *eventinstance, int *attributes)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_System_Release(int *system)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_SetParameterValue(int *eventinstance, char *name, float value)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventDescription_IsOneshot(int *eventdescription, int *oneshot)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_SetPaused(int *eventinstance, int paused)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_TriggerCue(int *eventinstance)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_Bus_StopAllEvents(int *bus, int mode)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_GetPaused(int *eventinstance, int *paused)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_GetPlaybackState(int *eventinstance, int *state)
{
	STUB();
}

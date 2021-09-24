#include "fmoad-cailin.h"
#include "al.h"

FMOD_RESULT FMOD_Studio_System_Create(FMOD_STUDIO_SYSTEM **system, unsigned int headerversion)
{
	// TODO: create new system object at the address **system; log headerversion
	fprintf(stderr, "%s headerversion: %d\n", __func__, headerversion);
	STUB();
}

FMOD_RESULT FMOD_Studio_System_Initialize(FMOD_STUDIO_SYSTEM *system,
	int maxchannels, FMOD_STUDIO_INITFLAGS studioflags,
	FMOD_INITFLAGS flags,
	void *extradriverdata)
{
	// TODO: log/use maxchannels; ignore studioflags, flags , and extradriverdata for now
	fprintf(stderr, "%s maxchannels: %d\n", __func__, maxchannels);
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

FMOD_RESULT FMOD_Studio_System_Update(FMOD_STUDIO_SYSTEM *system)
{
	//STUB();
	return FMOD_OK;
}

FMOD_RESULT FMOD_Studio_System_LoadBankFile(FMOD_STUDIO_SYSTEM *system,
	const char *filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags,
	FMOD_STUDIO_BANK **bank)
{
	size_t fnlen;
	fnlen = strnlen(filename, MAXSTR);
	// drop the extension ".bank" which should be on all these filenames
	size_t dstsize = fnlen - 4;
	char shortname[dstsize];
	strlcpy(shortname, filename, sizeof(shortname));
	fprintf(stderr, "filename: %s, shortname: %s\n", filename, shortname);
	// TODO: Store the bank which with fsb-extract-dumb + python-fsb5 is a directory "*.banko"
	FMOD_STUDIO_BANK *testbank = malloc(sizeof(FMOD_STUDIO_BANK));
	if (!testbank)
		err(1, NULL);
	// TODO: free() later
	/*
	if ((bank = (FMOD_STUDIO_BANK*)malloc(sizeof(FMOD_STUDIO_BANK)))) == NULL)
		err(1, NULL);
	*bank = {
		.name = basename(shortname),
		.parentdir = dirname(shortname),
		.fullpath = filename
	};
	*/
	testbank->name = basename(shortname);
	testbank->parentdir = dirname(shortname);
	testbank->fullpath = filename;
	*bank = testbank;
	STUB();
}

FMOD_RESULT FMOD_Studio_System_GetVCA(FMOD_STUDIO_SYSTEM *system, char *path, int **vca)
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

FMOD_RESULT FMOD_Studio_System_GetEvent(FMOD_STUDIO_SYSTEM *system, const char *path, FMOD_STUDIO_EVENTDESCRIPTION **event)
{
	fprintf(stderr, "%s path: %s\n", __func__, path);
	STUB();
}

FMOD_RESULT FMOD_Studio_EventDescription_LoadSampleData(FMOD_STUDIO_EVENTDESCRIPTION *eventdescription)
{
	// loads all non-streaming sample data used by the event
	STUB();
}

FMOD_RESULT FMOD_Studio_EventDescription_CreateInstance(FMOD_STUDIO_EVENTDESCRIPTION *eventdescription, int **instance)
{
	STUB();
}


FMOD_RESULT FMOD_Studio_EventDescription_Is3D(FMOD_STUDIO_EVENTDESCRIPTION *eventdescription, FMOD_BOOL *is3D)
{
	fprintf(stderr, "%s is3D: %d\n", __func__, *is3D);
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_Start(int *eventinstance)
{
	//playOgg("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko/sfx-char_mad_death.ogg");
	STUB();
}

FMOD_RESULT FMOD_Studio_System_GetBus(FMOD_STUDIO_SYSTEM *system, char *path, int **bus)
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

FMOD_RESULT FMOD_Studio_EventInstance_GetDescription(int *eventinstance, FMOD_STUDIO_EVENTDESCRIPTION **description)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventDescription_GetPath(FMOD_STUDIO_EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved)
{
	fprintf(stderr, "FMOD_Studio_EventDescription_GetPath: %s, %d\n", path, size);
	path = "event:/env/amb/worldmap\0";	// corresponds to ./Content/FMOD/Desktop/sfx.banko/sfx-env_amb_worldmap.ogg
	*retrieved = 24;
	return FMOD_OK;
}

FMOD_RESULT FMOD_Studio_Bank_LoadSampleData(FMOD_STUDIO_BANK *bank)
{
	/* TODO: implement this to preload the samples
	 * may need to walk through all .ogg files in the bank directory
	 */

	fprintf(stderr, "bank name: %s\n", bank->name);
	// TEMPORARY: load sfx.banko only once!
	if (SFX_LOADED)
		STUB();
	// for now just use sfx.banko; later need to get the right bank from FMOD_Studio_System_LoadBankFile
	char bankpath[] = "/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko";
	// https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program/17683417
	DIR *d;
	struct dirent *dir;
	d = opendir(bankpath);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			//fprintf(stderr, "sample data found: %s\n", dir->d_name);
		}
		closedir(d);
	}
	SFX_LOADED = 1;
	return FMOD_OK;
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

FMOD_RESULT FMOD_Studio_System_GetListenerAttributes(FMOD_STUDIO_SYSTEM *system, int listener, int *attributes)
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
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_Get3DAttributes(int *eventinstance, int *attributes)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_System_Release(FMOD_STUDIO_SYSTEM *system)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventInstance_SetParameterValue(int *eventinstance, char *name, float value)
{
	STUB();
}

FMOD_RESULT FMOD_Studio_EventDescription_IsOneshot(FMOD_STUDIO_EVENTDESCRIPTION *eventdescription, int *oneshot)
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

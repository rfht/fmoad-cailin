#include "fmoad-cailin.h"
#include "al.h"

FM_RESULT FMOD_Studio_System_Create(FM_STUDIO_SYSTEM **system, unsigned int headerversion)
{
	/* TODO: FMOD_Studio_System_Create is the first function called by Celeste.
	 *	unclear if that is always the case. */
	if (!init_done)
	{
		char *log_env;
		fprintf(stderr, "initializing fmoad-cailin\n");
		al_init();
		if ((log_env = getenv("FMOAD_LOGLEVEL")) != NULL)
		{
			loglevel = (unsigned int)atoi(log_env);
		}
		fprintf(stderr, "fmoad-cailin loglevel: %d\n", loglevel);
		init_done = 1;
	}
	// TODO: create new system object at the address **system
	fprintf(stderr, "%s headerversion: %d\n", __func__, headerversion);
	STUB();
}

FM_RESULT FMOD_Studio_System_Initialize(FM_STUDIO_SYSTEM *system,
	int maxchannels, FM_STUDIO_INITFLAGS studioflags,
	FM_INITFLAGS flags,
	void *extradriverdata)
{
	// ignoring studioflags, flags , and extradriverdata for now
	fprintf(stderr, "%s maxchannels: %d\n", __func__, maxchannels);
	// TEST SOUND
	vorbis_object *test_sound = al_load("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/ui.banko/ui-ui_main_button_select.ogg");
	
	fprintf(stderr, "test_sound size: %ld\n", (long)test_sound->size);
	al_play(test_sound);
	STUB();
}

FM_RESULT FMOD_Studio_System_SetListenerAttributes(FM_STUDIO_SYSTEM *system,
	int listener,
	FM_3D_ATTRIBUTES *attributes)
{
	/* listener: the "listener index"
	 * attributes: are 3D attributes of the listener
	 */
	fprintf(stderr, "%s: listener %d\n", __func__, listener);
	//STUB();
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_Update(FM_STUDIO_SYSTEM *system)
{
	// stay quiet; this one is called a lot
	//STUB();
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_LoadBankFile(FM_STUDIO_SYSTEM *system,
	const char *filename, FM_STUDIO_LOAD_BANK_FLAGS flags,
	FM_STUDIO_BANK **bank)
{
	size_t fnlen;
	// TODO: free() later
	char *db = reallocarray(NULL, MAXSTR, sizeof(char));

	fnlen = strnlen(filename, MAXSTR);
	size_t dstsize = fnlen - 4; // drop ".bank" extension
	char shortname[dstsize];
	strlcpy(shortname, filename, sizeof(shortname));
	strlcpy(db, filename, sizeof(db) * MAXSTR); // TODO: check return val
	strlcat(db, "o", sizeof(db) * MAXSTR); // TODO: check return val
	fprintf(stderr, "filename: %s, shortname: %s, db: %s\n", filename, shortname, db);
	// TODO: Store the bank which with fsb-extract-dumb + python-fsb5 is a directory "*.banko"
	FM_STUDIO_BANK *testbank = malloc(sizeof(FM_STUDIO_BANK)); // TODO: rename var
	if (!testbank)
		err(1, NULL);
	// TODO: free() later
	testbank->name = basename(shortname);
	testbank->parentdir = dirname(shortname);
	testbank->bankpath = filename;
	testbank->dirbank = db;
	*bank = testbank;
	STUB();
}

FM_RESULT FMOD_Studio_System_GetVCA(FM_STUDIO_SYSTEM *system, char *path, FM_STUDIO_VCA **vca)
{
	fprintf(stderr, "%s path: %s\n", __func__, path);
	STUB();
}

FM_RESULT FMOD_Studio_VCA_SetVolume(FM_STUDIO_VCA *vca, float volume)
{
	fprintf(stderr, "%s volume: %.2f\n", __func__, volume);
	// TODO: store the volume for the VCA
	STUB();
}

FM_RESULT FMOD_Studio_VCA_GetVolume(FM_STUDIO_VCA *vca, float *volume, float *finalvolume)
{
	STUB();
}

FM_RESULT FMOD_Studio_System_GetEvent(FM_STUDIO_SYSTEM *system, const char *path, FM_STUDIO_EVENTDESCRIPTION **event)
{
	fprintf(stderr, "%s path: %s\n", __func__, path);
	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_LoadSampleData(FM_STUDIO_EVENTDESCRIPTION *eventdescription)
{
	// loads all non-streaming sample data used by the event
	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_CreateInstance(FM_STUDIO_EVENTDESCRIPTION *eventdescription, int **instance)
{
	STUB();
}


FM_RESULT FMOD_Studio_EventDescription_Is3D(FM_STUDIO_EVENTDESCRIPTION *eventdescription, FM_BOOL *is3D)
{
	fprintf(stderr, "%s is3D: %d\n", __func__, *is3D);
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Start(int *eventinstance)
{
	//playOgg("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko/sfx-char_mad_death.ogg");
	STUB();
}

FM_RESULT FMOD_Studio_System_GetBus(FM_STUDIO_SYSTEM *system, char *path, FM_STUDIO_BUS **bus)
{
	fprintf(stderr, "%s path: %s\n", __func__, path);
	STUB();
}

FM_RESULT FMOD_Studio_Bus_SetPaused(FM_STUDIO_BUS *bus, FM_BOOL paused)
{
	fprintf(stderr, "%s paused: %d\n", __func__, (int)paused);
	STUB();
}

FM_RESULT FMOD_Studio_Bus_GetPaused(FM_STUDIO_BUS *bus, FM_BOOL *paused)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetDescription(int *eventinstance, FM_STUDIO_EVENTDESCRIPTION **description)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_GetPath(FM_STUDIO_EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved)
{
	fprintf(stderr, "%s path: %s, size: %d\n", __func__, path, size);
	path = "event:/env/amb/worldmap\0";	// corresponds to ./Content/FMOD/Desktop/sfx.banko/sfx-env_amb_worldmap.ogg
	*retrieved = 24;
	return FM_OK;
}

FM_RESULT FMOD_Studio_Bank_LoadSampleData(FM_STUDIO_BANK *bank)
{
	fprintf(stderr, "dirbank: %s\n", bank->dirbank);
	// https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program/17683417
	DIR *d;
	struct dirent *dir;
	d = opendir(bank->dirbank);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] != '.')	// filter out '.' and '..'
			{
				//fprintf(stderr, "loading sample data from %s\n", dir->d_name);
				// TODO: implement loading the sample into memory
			}
		}
		closedir(d);
	} else {
		fprintf(stderr, "Error opening dirbank at %s\n", bank->dirbank);
	}
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_SetVolume(int *eventinstance, float volume)
{
	STUB();
}

FM_RESULT FMOD_Studio_System_GetLowLevelSystem(FM_STUDIO_SYSTEM *system, FM_SYSTEM **lowLevelSystem)
{
	// get the low level (FMOD) system object
	STUB();
}

FM_RESULT FMOD_Studio_System_GetListenerAttributes(FM_STUDIO_SYSTEM *system, int listener, int *attributes)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Set3DAttributes(int *eventinstance, int *attributes)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Release(int *eventinstance)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetVolume(int *eventinstance, float *volume, float *finalvolume)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Stop(int *eventinstance, int mode)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Get3DAttributes(int *eventinstance, int *attributes)
{
	STUB();
}

FM_RESULT FMOD_Studio_System_Release(FM_STUDIO_SYSTEM *system)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_SetParameterValue(int *eventinstance, char *name, float value)
{
	//STUB();
	return FM_OK;
}

FM_RESULT FMOD_Studio_EventDescription_IsOneshot(FM_STUDIO_EVENTDESCRIPTION *eventdescription, int *oneshot)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_SetPaused(int *eventinstance, int paused)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_TriggerCue(int *eventinstance)
{
	STUB();
}

FM_RESULT FMOD_Studio_Bus_StopAllEvents(FM_STUDIO_BUS *bus, int mode)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetPaused(int *eventinstance, int *paused)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetPlaybackState(int *eventinstance, int *state)
{
	STUB();
}

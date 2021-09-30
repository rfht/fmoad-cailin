#include "fmoad-cailin.h"
#include "al.h"

const char **get_event_paths(BANK *bank)
{
	json_object *events = json_object_object_get(bank->jo, "events");	// TODO: replace with json_object_object_get_ex()
	size_t n_events = json_object_array_length(events);
	const char** ret = NULL;
	json_object *event;
	for (int i = 0; i < n_events; i++)
	{
		event = json_object_array_get_idx(events, i);
		//if (!strncmp("event:/env/amb/worldmap", json_object_get_string(json_object_object_get(test_obj, "path")), MAXSTR))
		ret[i] = json_object_get_string(json_object_object_get(event, "path"));
		DPRINT(1, "%d: %s", i+1, ret[i]);
	}
	return ret;
}

const char **get_vca_paths(BANK *bank)
{
	json_object *vcas = json_object_object_get(bank->jo, "VCAs");	// TODO: replace with json_object_object_get_ex()
	size_t n_vcas = json_object_array_length(vcas);
	const char** ret = NULL;
	json_object *vca;
	for (int i = 0; i < n_vcas; i++)
	{
		vca = json_object_array_get_idx(vcas, i);
		ret[i] = json_object_get_string(json_object_object_get(vca, "path"));
		DPRINT(1, "%d: %s", i+1, ret[i]);
	}
	return ret;
}

FM_RESULT FMOD_Studio_System_Create(SYSTEM **system, unsigned int headerversion)
{
	/* TODO: FMOD_Studio_System_Create is the first function called by Celeste.
	 *	unclear if that is always the case. */
	if (!init_done)
	{
		char *log_env;
		fprintf(stderr, "[%s] initializing OpenAL backend\n", PROJ);
		al_init();
		if ((log_env = getenv("FMOAD_LOGLEVEL")) != NULL)
		{
			loglevel = (unsigned int)atoi(log_env);
		}
		fprintf(stderr, "[%s] FMOAD_LOGLEVEL=%d\n", PROJ, loglevel);
		init_done = 1;
	}
	// TODO: create new system object at the address **system
	DPRINT(2, "headerversion %d", headerversion);
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_Initialize(SYSTEM *system,
	int maxchannels, FM_STUDIO_INITFLAGS studioflags,
	FM_INITFLAGS flags,
	void *extradriverdata)
{
	// ignoring studioflags, flags , and extradriverdata for now
	fprintf(stderr, "%s maxchannels: %d\n", __func__, maxchannels);
	// TEST SOUND
	sound_object *test_sound = al_load("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/ui.banko/ui-ui_main_button_select.ogg");
	fprintf(stderr, "test_sound size: %ld\n", (long)test_sound->size);
	al_play(test_sound);
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_SetListenerAttributes(SYSTEM *system,
	int listener,
	FM_3D_ATTRIBUTES *attributes)
{
	/* listener: the "listener index"
	 * attributes: are 3D attributes of the listener
	 */
	DPRINT(2, "listener %d", listener);
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_Update(SYSTEM *system)
{
	DPRINT(3, "STUB"); // stay quiet; this one is called a lot
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_LoadBankFile(SYSTEM *system,
	const char *filename, FM_STUDIO_LOAD_BANK_FLAGS flags,
	BANK **bank)
{
	size_t fnlen;
	char jo_path[MAXSTR];
	// TODO: free() later
	char *db = reallocarray(NULL, MAXSTR, sizeof(char));

	fnlen = strnlen(filename, MAXSTR);
	size_t dstsize = fnlen - 4; // drop ".bank" extension
	char shortname[dstsize];
	strlcpy(shortname, filename, sizeof(shortname));
	strlcpy(db, filename, sizeof(db) * MAXSTR); // TODO: check return val
	strlcat(db, "o", sizeof(db) * MAXSTR); // TODO: check return val
	strlcpy(jo_path, shortname, MAXSTR);
	strlcat(jo_path, ".json", MAXSTR);
	// TODO: Store the bank which with fsb-extract-dumb + python-fsb5 is a directory "*.banko"
	BANK *newbank = malloc(sizeof(BANK));
	if (!newbank)
		err(1, NULL);
	// TODO: free() later

	json_object *jo = json_object_from_file(jo_path);

	newbank->name = basename(shortname);
	newbank->parentdir = dirname(shortname);
	newbank->bankpath = filename;
	newbank->dirbank = db;
	newbank->jo = jo;	// the json_object structure
	newbank->path = json_object_get_string(json_object_object_get(newbank->jo, "path"));
	newbank->guid = json_object_get_string(json_object_object_get(newbank->jo, "GUID"));
	DPRINT(2, "filename: %s, shortname: %s, db: %s, jo_path: %s, path: %s, guid: %s", filename, shortname, db, jo_path, newbank->path, newbank->guid);

	// TEST for newbank->jo
	json_object *test = json_object_object_get(newbank->jo, "events");	// TODO: replace with json_object_object_get_ex()
	size_t n_test = json_object_array_length(test);
	DPRINT(1, "n_test: %zu", n_test);
	json_object *test_obj;
	for (int i = 0; i < n_test; i++)
	{
		test_obj = json_object_array_get_idx(test, i);
		if (!strncmp("event:/env/amb/worldmap", json_object_get_string(json_object_object_get(test_obj, "path")), MAXSTR))
		{
			DPRINT(1, "%d: %s\n", i+1, json_object_get_string(json_object_object_get(test_obj, "path")));
		}
	}

	*bank = newbank;
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_GetVCA(SYSTEM *system, char *path, VCA **vca)
{
	DPRINT(2, "path: %s", path);
	STUB();
}

FM_RESULT FMOD_Studio_VCA_SetVolume(VCA *vca, float volume)
{
	DPRINT(1, "volume: %f", volume);
	// TODO: store the volume for the VCA
	STUB();
}

FM_RESULT FMOD_Studio_VCA_GetVolume(VCA *vca, float *volume, float *finalvolume)
{
	STUB();
}

FM_RESULT FMOD_Studio_System_GetEvent(SYSTEM *system, const char *path, EVENTDESCRIPTION **event)
{
	DPRINT(1, "path: %s", path);
	/*
	 * example: event:/env/amb/worldmap
	 *
	 * in file Content/FMOD/Desktop/sfx.json
	 * search for "path": "event:/env/amb/worldmap"
	 * in this object, query for "files":[]"filename"
	 * this will get "env_amb_worldmap"
	 */

	/*
	json_object *test = json_object_object_get(newbank->jo, "events");
	size_t n_test = json_object_array_length(test);
	DPRINT(1, "n_test: %zu", n_test);
	json_object *test_obj;
	for (int i = 0; i < n_test; i++)
	{
		test_obj = json_object_array_get_idx(test, i);
		if (!strncmp("event:/env/amb/worldmap", json_object_get_string(json_object_object_get(test_obj, "path")), MAXSTR))
		{
			DPRINT(1, "%d: %s\n", i+1, json_object_get_string(test_obj));
		}
	}
	*/

	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_LoadSampleData(EVENTDESCRIPTION *eventdescription)
{
	// loads all non-streaming sample data used by the event
	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_CreateInstance(EVENTDESCRIPTION *eventdescription, int **instance)
{
	STUB();
}


FM_RESULT FMOD_Studio_EventDescription_Is3D(EVENTDESCRIPTION *eventdescription, bool *is3D)
{
	DPRINT(2, "is3D %d", *is3D);
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Start(EVENTINSTANCE *eventinstance)
{
	//playOgg("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko/sfx-char_mad_death.ogg");
	STUB();
}

FM_RESULT FMOD_Studio_System_GetBus(SYSTEM *system, char *path, BUS **bus)
{
	DPRINT(1, "path %s", path);
	STUB();
}

FM_RESULT FMOD_Studio_Bus_SetPaused(BUS *bus, bool paused)
{
	DPRINT(1, "paused %d", (int)paused);
	STUB();
}

FM_RESULT FMOD_Studio_Bus_GetPaused(BUS *bus, bool *paused)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetDescription(EVENTINSTANCE *eventinstance, EVENTDESCRIPTION **description)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_GetPath(EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved)
{
	path = "event:/env/amb/worldmap\0";	// corresponds to ./Content/FMOD/Desktop/sfx.banko/sfx-env_amb_worldmap.ogg
	*retrieved = strnlen(path, size) + 1;	// +1 to account for terminating null character
	DPRINT(1, "path %s, buffer size %d, retrieved %d", path, size, *retrieved);
	return FM_OK;
}

FM_RESULT FMOD_Studio_Bank_LoadSampleData(BANK *bank)
{
	DPRINT(2, "dirbank: %s", bank->dirbank);
	// https://stackoverflow.com/questions/4204666/how-to-list-files-in-a-directory-in-a-c-program/17683417
	DIR *d;
	struct dirent *dir;
	d = opendir(bank->dirbank);
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			if (dir->d_name[0] != '.')	// filter out '.' and '..' and hidden files '.*'
			{
				DPRINT(2, "sample %s", dir->d_name);
				// TODO: implement loading the sample into memory
			}
		}
		closedir(d);
	} else {
		fprintf(stderr, "Error opening dirbank at %s\n", bank->dirbank);
	}
	return FM_OK;
}

FM_RESULT FMOD_Studio_EventInstance_SetVolume(EVENTINSTANCE *eventinstance, float volume)
{
	STUB();
}

FM_RESULT FMOD_Studio_System_GetLowLevelSystem(SYSTEM *system, FM_SYSTEM **lowLevelSystem)
{
	// get the low level (FMOD) system object
	STUB();
}

FM_RESULT FMOD_Studio_System_GetListenerAttributes(SYSTEM *system, int listener, int *attributes)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Set3DAttributes(EVENTINSTANCE *eventinstance, int *attributes)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Release(EVENTINSTANCE *eventinstance)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetVolume(EVENTINSTANCE *eventinstance, float *volume, float *finalvolume)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Stop(EVENTINSTANCE *eventinstance, int mode)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Get3DAttributes(EVENTINSTANCE *eventinstance, int *attributes)
{
	STUB();
}

FM_RESULT FMOD_Studio_System_Release(SYSTEM *system)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_SetParameterValue(EVENTINSTANCE *eventinstance, char *name, float value)
{
	DPRINT(2, "name: %s, value: %.2f", name, value);
	return FM_OK;
}

FM_RESULT FMOD_Studio_EventDescription_IsOneshot(EVENTDESCRIPTION *eventdescription, int *oneshot)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_SetPaused(EVENTINSTANCE *eventinstance, int paused)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_TriggerCue(EVENTINSTANCE *eventinstance)
{
	STUB();
}

FM_RESULT FMOD_Studio_Bus_StopAllEvents(BUS *bus, int mode)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetPaused(EVENTINSTANCE *eventinstance, int *paused)
{
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_GetPlaybackState(EVENTINSTANCE *eventinstance, int *state)
{
	STUB();
}

/* Missing API:
 * FMOD_Studio_EventInstance_GetParameterValue()
 */

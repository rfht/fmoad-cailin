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

#include "fmoad-cailin.h"
#include "al.h"

int get_sound_idx(char *path)
{
	for (int i = 0; i < sound_counter; i++)
	{
		if (!strncmp(path, sounds[i].path, MAXSTR))
			return i;
	}
	return -1;	// not found, this is an error
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
	DPRINT(2, "headerversion %d", headerversion);
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_Initialize(SYSTEM *system,
	int maxchannels, FM_STUDIO_INITFLAGS studioflags,
	FM_INITFLAGS flags,
	void *extradriverdata)
{
	fprintf(stderr, "%s maxchannels: %d\n", __func__, maxchannels);
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_SetListenerAttributes(SYSTEM *system,
	int listener,
	FM_3D_ATTRIBUTES *attributes)
{
	 //attributes: are 3D attributes of the listener
	DPRINT(2, "listener %d", listener);
	return FM_OK;
}

FM_RESULT FMOD_Studio_System_Update(SYSTEM *system)
{
	for (int i = 0; i < sp_counter; i++)
	{
		if(StreamPlayerArr[i].retired == false)
		{
			if(!UpdatePlayer(&StreamPlayerArr[i])) // returns 0 when playback finished or errors occurred. Prints a message if error.
			{
				DeletePlayer(&StreamPlayerArr[i]);
			}
		}
	}
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
	/*
	 * example: event:/env/amb/worldmap
	 *
	 * in file Content/FMOD/Desktop/sfx.json
	 * search for "path": "event:/env/amb/worldmap"
	 * in this object, query for "files":[]"filename"
	 * this will get "env_amb_worldmap"
	 */

	EVENTDESCRIPTION *newevent = malloc(sizeof(EVENTDESCRIPTION));
	if (!newevent)
		err(1, NULL);
	newevent->path = path;
	newevent->sound_idx = get_sound_idx((char *)path);	// returns -1 if error;
	DPRINT(1, "path: %s, sound_idx: %d", newevent->path, newevent->sound_idx);
	/* THIS SHOULD BE HANDLED IN ..._CreateInstance
	if (newevent->sound_idx < 0)
		return FM_ERR_INTERNAL;
	*/
	*event = newevent;
	return FM_OK;
}

FM_RESULT FMOD_Studio_EventDescription_LoadSampleData(EVENTDESCRIPTION *eventdescription)
{
	// loads all non-streaming sample data used by the event
	STUB();
}

FM_RESULT FMOD_Studio_EventDescription_CreateInstance(EVENTDESCRIPTION *eventdescription, EVENTINSTANCE **instance)
{
	int player_idx = -1; // -1 would be invalid

	if (eventdescription->sound_idx < 0)
	{
		instance = NULL;
		return FM_OK;	// don't mess; just keep going
	}
	EVENTINSTANCE *newinstance = malloc(sizeof(EVENTINSTANCE));
	newinstance->evd = eventdescription;
	int sound_num = newinstance->evd->sound_idx;
	DPRINT(1, "sp_counter: %d, evd->sound_idx: %d, fp: %s, path: %s, issample: %d", sp_counter, sound_num, sounds[sound_num].fp, sounds[sound_num].path, sounds[sound_num].issample);

	// check if a retired StreamPlayer can be reused

	for (int i = 0; i <= sp_counter; i++)
	{
		if (StreamPlayerArr[i].retired)
		{
			player_idx = i;
			break;
		}
	}
	if (player_idx < 0)
		player_idx = sp_counter++;

	memset(&StreamPlayerArr[player_idx], 0, sizeof(StreamPlayer));
	StreamPlayerArr[player_idx] = *NewPlayer();
	if (!OpenPlayerFile(&StreamPlayerArr[player_idx], sounds[newinstance->evd->sound_idx].fp))
	{
		fprintf(stderr, "ERROR with OpenPlayerFile; aborting\n");
		exit(1);	// TODO: return an error instead
	}
	StreamPlayerArr[player_idx].fm_path = sounds[newinstance->evd->sound_idx].path;
	newinstance->sp_idx = player_idx;
	*instance = newinstance;
	return FM_OK;
}


FM_RESULT FMOD_Studio_EventDescription_Is3D(EVENTDESCRIPTION *eventdescription, bool *is3D)
{
	DPRINT(2, "is3D %d", *is3D);
	STUB();
}

FM_RESULT FMOD_Studio_EventInstance_Start(EVENTINSTANCE *eventinstance)
{
	if (!eventinstance)
	{
		// something went wrong earlier - bail
		return FM_OK;
	}
	//playOgg("/home/thfr/games/fnaify/celeste/1.3.1.2/unzipped/Content/FMOD/Desktop/sfx.banko/sfx-char_mad_death.ogg");
	DPRINT(1, "sound_idx: %d, sp_idx: %d", eventinstance->evd->sound_idx, eventinstance->sp_idx);
	DPRINT(1, "sound object path: %s", sounds[eventinstance->evd->sound_idx].path);
	if(!StartPlayer(&StreamPlayerArr[eventinstance->sp_idx]))
	{
		fprintf(stderr, "ERROR in StartPlayer\n");
		exit(1);
	}
	//al_play(&sounds[eventinstance->evd->sound_idx]);
	// TODO: check return value
	return FM_OK;
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
	// iterate over all events in the bank
	json_object *events = json_object_object_get(bank->jo, "events");	// TODO: replace with json_object_object_get_ex()
	size_t n_events = json_object_array_length(events);
	json_object *event;
	json_object *files;
	json_object *file;
	char *path;
	char *filename;
	bool issample;
	for (int i = 0; i < n_events; i++)
	{
		event = json_object_array_get_idx(events, i);
		files = json_object_object_get(event, "files");
		path = (char *)json_object_get_string(json_object_object_get(event, "path"));
		DPRINT(1, "%s: %zu", path, json_object_array_length(files));
		if (json_object_array_length(files) == 1)	// TODO: not accounting for >1
		// for (int i = 0; i < json_object_array_length(files); i++)
		{
			filename[0] = '\0';	// empty the string array
			file = json_object_array_get_idx(files, 0);
			filename = (char *)json_object_get_string(json_object_object_get(file, "filename"));
			issample = json_object_get_boolean(json_object_object_get(file, "issample"));
			char *ogg_path = reallocarray(NULL, MAXSTR, sizeof(char));
			strlcpy(ogg_path, bank->dirbank, MAXSTR);
			strlcat(ogg_path, "/", MAXSTR);
			strlcat(ogg_path, bank->name, MAXSTR);
			strlcat(ogg_path, "-", MAXSTR);
			strlcat(ogg_path, filename, MAXSTR);
			strlcat(ogg_path, ".ogg", MAXSTR);
			DPRINT(1, "ogg_path: %s", ogg_path);
			DPRINT(1, "issample: %d", issample);
			sounds[sound_counter].fp = ogg_path;
			//strlcpy(sounds[sound_counter].fp, ogg_path, MAXSTR);
			sounds[sound_counter].path = path;
			sounds[sound_counter].issample = issample;
			sound_counter++;
		}
	}
	DPRINT(2, "sound_counter: %d", sound_counter);
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
	DPRINT(4, "name: %s, value: %.2f", name, value);
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

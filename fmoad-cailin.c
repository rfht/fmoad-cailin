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
		if (!sounds[i].path)	// skip if an empty entry
			continue;
		if (!strncmp(path, sounds[i].path, MAXSTR))
			return i;
	}
	return -1;	// not found, this is an error
}

VCA *NewVca(void)
{
	VCA *vca;
	vca = malloc(sizeof(*vca));
	assert(vca != NULL);
	vca->path = NULL;
	vca->volume = 1.0;		// Start new VCAs at full volume
	vca->finalvolume = 1.0;
	return vca;
}

int FMOD_Studio_System_Create(SYSTEM **system, unsigned int headerversion)
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
	return 0;
}

int FMOD_Studio_System_Initialize(SYSTEM *system,
	int maxchannels, FM_STUDIO_INITFLAGS studioflags,
	FM_INITFLAGS flags,
	void *extradriverdata)
{
	DPRINT(2, " STUB; maxchannels: %d", maxchannels);
	return 0;
}

int FMOD_Studio_System_SetListenerAttributes(SYSTEM *system,
	int listener,
	FM_3D_ATTRIBUTES *attributes)
{
	DPRINT(2, "listener %d", listener);
	return 0;
}

int FMOD_Studio_System_Update(SYSTEM *system)
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
	return 0;
}

int FMOD_Studio_System_LoadBankFile(SYSTEM *system,
	const char *filename, FM_STUDIO_LOAD_BANK_FLAGS flags,
	BANK **bank)
{
	size_t fnlen;
	char jo_path[MAXSTR];
	char *db = reallocarray(NULL, MAXSTR, sizeof(char));

	fnlen = strnlen(filename, MAXSTR);
	size_t dstsize = fnlen - 4; // drop ".bank" extension
	char shortname[dstsize];
	strlcpy(shortname, filename, sizeof(shortname));
	strlcpy(db, filename, sizeof(db) * MAXSTR); // TODO: check return val
	strlcat(db, "o", sizeof(db) * MAXSTR); // TODO: check return val
	strlcpy(jo_path, shortname, MAXSTR);
	strlcat(jo_path, ".json", MAXSTR);

	BANK *newbank = malloc(sizeof(BANK));
	if (!newbank)
		err(1, NULL);
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
	return 0;
}

int FMOD_Studio_System_GetVCA(SYSTEM *system, char *path, VCA **vca)
{
	DPRINT(2, "STUB; path: %s", path);
	VCA *newvca = NewVca();
	newvca->path = path;
	*vca = newvca;
	return 0;
}

int FMOD_Studio_VCA_SetVolume(VCA *vca, float volume)
{
	DPRINT(1, "STUB; volume: %.2f", volume);
	vca->volume = volume;
	vca->finalvolume = volume;	// for now keep volume and finalvolume the same
	return 0;
}

int FMOD_Studio_VCA_GetVolume(VCA *vca, float *volume, float *finalvolume)
{
	DPRINT(1, "STUB; vca path: %s, volume: %.2f, finalvolume: %.2f", vca->path,
		vca->volume, vca->finalvolume);
	*volume = vca->volume;
	*finalvolume = vca->finalvolume;
	return 0;
}

int FMOD_Studio_System_GetEvent(SYSTEM *system, const char *path, EVENTDESCRIPTION **event)
{
	EVENTDESCRIPTION *newevent = malloc(sizeof(EVENTDESCRIPTION));
	if (!newevent)
		err(1, NULL);
	newevent->path = path;
	newevent->sound_idx = get_sound_idx((char *)path);
	/* Errors (sound_idx < 0) are handled in ..._CreateInstance */
	DPRINT(1, "path: %s, sound_idx: %d", newevent->path, newevent->sound_idx);
	*event = newevent;
	return 0;
}

int FMOD_Studio_EventDescription_LoadSampleData(EVENTDESCRIPTION *eventdescription)
{
	STUB();
}

int FMOD_Studio_EventDescription_CreateInstance(EVENTDESCRIPTION *eventdescription, EVENTINSTANCE **instance)
{
	int player_idx = -1; // -1 would be invalid
	int i;

	if (eventdescription->sound_idx < 0)
	{
		instance = NULL;
		return 0;	// don't mess; just keep going
	}
	EVENTINSTANCE *newinstance = malloc(sizeof(EVENTINSTANCE));
	newinstance->evd = eventdescription;
	int sound_num = newinstance->evd->sound_idx;
	DPRINT(1, "sp_counter: %d, evd->sound_idx: %d, path: %s, n_filepaths: %d", sp_counter, sound_num, sounds[sound_num].path, sounds[sound_num].n_filepaths);

	for (i = 0;
		i < sounds[newinstance->evd->sound_idx].n_filepaths;
		i++)
	{
		if (i >= MAX_INST_SP)
		{
			fprintf(stderr, "Error: exceding maximum number of StreamPlayers that can be associated with an EventInstance.\n");
			exit(1);
		}
		// check if a retired StreamPlayer can be reused
		for (int j = 0; j <= sp_counter; j++)
		{
			if (StreamPlayerArr[j].retired)
			{
				player_idx = j;
				break;
			}
		}

		// no retired StreamPlayer found; therefore create a new one
		if (player_idx < 0)
			player_idx = sp_counter++;

		memset(&StreamPlayerArr[player_idx], 0, sizeof(StreamPlayer));
		StreamPlayerArr[player_idx] = *NewPlayer();
		if (!OpenPlayerFile(&StreamPlayerArr[player_idx], sounds[newinstance->evd->sound_idx].filepaths[i]))
		{
			fprintf(stderr, "ERROR with OpenPlayerFile; aborting\n");
			exit(1);	// TODO: return an error instead
		}
		StreamPlayerArr[player_idx].fm_path = sounds[newinstance->evd->sound_idx].path;
		newinstance->sp_idx[i] = player_idx;
	}
	newinstance->n_sp = i;
	*instance = newinstance;
	return 0;
}


int FMOD_Studio_EventDescription_Is3D(EVENTDESCRIPTION *eventdescription, bool *is3D)
{
	STUB();
}

int FMOD_Studio_EventInstance_Start(EVENTINSTANCE *eventinstance)
{
	int i;

	if (!eventinstance)
	{
		// something went wrong earlier - bail
		return 0;
	}
	for (i = 0; i < eventinstance->n_sp; i++)
	{
		DPRINT(1, "sound_idx: %d, sp_idx: %d, path: %s", eventinstance->evd->sound_idx, eventinstance->sp_idx[i], sounds[eventinstance->evd->sound_idx].path);
		if(!StartPlayer(&StreamPlayerArr[eventinstance->sp_idx[i]]))
		{
			fprintf(stderr, "ERROR in StartPlayer\n");
			exit(1);
		}
	}
	return 0;
}

int FMOD_Studio_System_GetBus(SYSTEM *system, char *path, BUS **bus)
{
	DPRINT(1, "path %s", path);	// TODO: create bus
	STUB();
}

int FMOD_Studio_Bus_SetPaused(BUS *bus, bool paused)
{
	DPRINT(1, "STUB; paused %d", (int)paused);
	return 0;
}

int FMOD_Studio_Bus_GetPaused(BUS *bus, bool *paused)
{
	STUB();
}

int FMOD_Studio_EventInstance_GetDescription(EVENTINSTANCE *eventinstance, EVENTDESCRIPTION **description)
{
	STUB();
}

int FMOD_Studio_EventDescription_GetPath(EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved)
{
	path = "event:/env/amb/worldmap\0";	// corresponds to ./Content/FMOD/Desktop/sfx.banko/sfx-env_amb_worldmap.ogg
	*retrieved = strnlen(path, size) + 1;	// +1 to account for terminating null character
	DPRINT(1, "path %s, buffer size %d, retrieved %d", path, size, *retrieved);
	return 0;
}

int FMOD_Studio_Bank_LoadSampleData(BANK *bank)
{
	// iterate over all events in the bank
	json_object *events = json_object_object_get(bank->jo, "events");
	size_t n_events = json_object_array_length(events);
	json_object *event;
	for (int i = 0; i < n_events; i++)
	{
		int j;
		json_object *files;
		json_object *file;
		int num_files;
		char *filename = reallocarray(NULL, MAXSTR, sizeof(char));
		char *path = reallocarray(NULL, MAXSTR, sizeof(char));
		event = json_object_array_get_idx(events, i);
		files = json_object_object_get(event, "files");
		path = (char *)json_object_get_string(json_object_object_get(event, "path"));
		num_files = json_object_array_length(files);
		char **filepaths_buf = reallocarray(NULL, num_files, sizeof(char*));
		DPRINT(1, "%s: %d", path, num_files);
		sounds[sound_counter] = *NewSoundObject();
		for (j = 0;
			j < num_files;
			j++)
		{
			filename[0] = '\0';	// empty the string array
			file = json_object_array_get_idx(files, j);
			filename = (char *)json_object_get_string(json_object_object_get(file, "filename"));
			char *ogg_path = reallocarray(NULL, MAXSTR, sizeof(char));
			strlcpy(ogg_path, bank->dirbank, MAXSTR);
			strlcat(ogg_path, "/", MAXSTR);
			strlcat(ogg_path, bank->name, MAXSTR);
			strlcat(ogg_path, "-", MAXSTR);
			strlcat(ogg_path, filename, MAXSTR);
			strlcat(ogg_path, ".ogg", MAXSTR);
			DPRINT(1, "ogg_path: %s", ogg_path);
			//sounds[sound_counter].filepaths[j] = ogg_path;
			filepaths_buf[j] = reallocarray(NULL, MAXSTR, sizeof(char));
			filepaths_buf[j] = ogg_path;
			sounds[sound_counter].path = path;
		}
		sounds[sound_counter].n_filepaths = j;
		sounds[sound_counter].filepaths = filepaths_buf;
		sound_counter++;
	}
	DPRINT(2, "sound_counter: %d", sound_counter);
	return 0;
}

int FMOD_Studio_EventInstance_SetVolume(EVENTINSTANCE *eventinstance, float volume)
{
	DPRINT(2, "STUB; volume: %.2f", volume);
	return 0;
}

int FMOD_Studio_System_GetLowLevelSystem(SYSTEM *system, FM_SYSTEM **lowLevelSystem)
{
	STUB();
}

int FMOD_Studio_System_GetListenerAttributes(SYSTEM *system, int listener, int *attributes)
{
	STUB();
}

int FMOD_Studio_EventInstance_Set3DAttributes(EVENTINSTANCE *eventinstance, int *attributes)
{
	STUB();
}

int FMOD_Studio_EventInstance_Release(EVENTINSTANCE *eventinstance)
{
	if (eventinstance)	// don't do anything if eventinstance has already been emptied
	{
		// TODO: schedule instance to be destroyed when it stops
		DPRINT(1, "path: %s", eventinstance->evd->path);
	}
	return 0;
}

int FMOD_Studio_EventInstance_GetVolume(EVENTINSTANCE *eventinstance, float *volume, float *finalvolume)
{
	STUB();
}

int FMOD_Studio_EventInstance_Stop(EVENTINSTANCE *eventinstance, FM_STOP_MODE mode)
{
	if (eventinstance)	// avoid doing anything if there's a NULL
	{
		int i;
		for (i = 0; i < eventinstance->n_sp; i++)
		{
			DPRINT(1, "stop mode: %d, on sp_idx: %d, path: %s", (int)mode, eventinstance->sp_idx[i], eventinstance->evd->path);
			// TODO: implement fading out, e.g. https://stackoverflow.com/questions/47384635/how-to-stop-a-sound-smooth-in-openal
			if (StreamPlayerArr[eventinstance->sp_idx[i]].retired == false)
				DeletePlayer(&StreamPlayerArr[eventinstance->sp_idx[i]]);
			// TODO: check return value
		}
	}
	return 0;
}

int FMOD_Studio_EventInstance_Get3DAttributes(EVENTINSTANCE *eventinstance, int *attributes)
{
	STUB();
}

int FMOD_Studio_System_Release(SYSTEM *system)
{
	STUB();
}

int FMOD_Studio_EventInstance_SetParameterValue(EVENTINSTANCE *eventinstance, char *name, float value)
{
	DPRINT(4, "STUB; name: %s, value: %.2f", name, value);
	return 0;
}

int FMOD_Studio_EventDescription_IsOneshot(EVENTDESCRIPTION *eventdescription, int *oneshot)
{
	STUB();
}

int FMOD_Studio_EventInstance_SetPaused(EVENTINSTANCE *eventinstance, int paused)
{
	// TODO: pause eventinstance
	STUB();
}

int FMOD_Studio_EventInstance_TriggerCue(EVENTINSTANCE *eventinstance)
{
	STUB();
}

int FMOD_Studio_Bus_StopAllEvents(BUS *bus, FM_STOP_MODE mode)
{
	STUB();
}

int FMOD_Studio_EventInstance_GetPaused(EVENTINSTANCE *eventinstance, int *paused)
{
	STUB();
}

int FMOD_Studio_EventInstance_GetPlaybackState(EVENTINSTANCE *eventinstance, int *state)
{
	STUB();
}

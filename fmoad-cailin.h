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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <err.h>
#include <libgen.h>
#include <json-c/json.h>

#define PROJ		"FMOAD-CAILIN"	// project name
#define MAXSTR		1024

#define FM_INITFLAGS			unsigned int
#define FM_SOUND			int
#define FM_STUDIO_INITFLAGS		unsigned int
#define FM_STUDIO_LOAD_BANK_FLAGS	unsigned int
#define FM_SYSTEM			int

static bool init_done = false;
static unsigned int loglevel = 0;

#define DPRINT(threshold, ...) do { \
	if (threshold <= loglevel) { \
		fprintf(stderr, "[%s] %s: ", PROJ, __func__); \
		fprintf(stderr, __VA_ARGS__); \
		fprintf(stderr, "\n"); \
	} \
} while (0)

#define STUB() do { \
	DPRINT(2, "STUB"); \
	return 0; \
} while (0)

typedef struct FM_3D_ATTRIBUTES{
	float position;
	float velocity;
	float forward;
	float up;
} FM_3D_ATTRIBUTES;

typedef struct BANK{
	const char *name;
	const char *parentdir;
	const char *bankpath;
	const char *dirbank;
	json_object *jo;
	const char *path;	// the FMOD bank internal path, like "bank:/Master Bank"
	const char *guid;
} BANK;

typedef struct SYSTEM{
	unsigned int maxchannels;
	unsigned int flags;
	unsigned int studioflags;
} SYSTEM;

typedef struct VCA{
	const char *path;
	float volume;
	float finalvolume;
} VCA;

typedef struct BUS{
	const char *path;
	bool paused;
} BUS;

typedef struct EVENTDESCRIPTION{
	const char *path;
	int sound_idx;		// index of the sound_object in the sounds array
} EVENTDESCRIPTION;

typedef struct EVENTINSTANCE{
	EVENTDESCRIPTION *evd;
	int sp_idx;
} EVENTINSTANCE;

int FMOD_Studio_Bank_LoadSampleData(BANK *bank);
int FMOD_Studio_Bus_GetPaused(BUS *bus, bool *paused);
int FMOD_Studio_Bus_SetPaused(BUS *bus, bool paused);
int FMOD_Studio_Bus_StopAllEvents(BUS *bus, int mode);
int FMOD_Studio_EventDescription_CreateInstance(EVENTDESCRIPTION *eventdescription, EVENTINSTANCE **instance);
int FMOD_Studio_EventDescription_GetPath(EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved);
int FMOD_Studio_EventDescription_Is3D(EVENTDESCRIPTION *eventdescription, bool *is3D);
int FMOD_Studio_EventDescription_IsOneshot(EVENTDESCRIPTION *eventdescription, int *oneshot);
int FMOD_Studio_EventDescription_LoadSampleData(EVENTDESCRIPTION *eventdescription);
int FMOD_Studio_EventInstance_Get3DAttributes(EVENTINSTANCE *eventinstance, int *attributes);
int FMOD_Studio_EventInstance_GetDescription(EVENTINSTANCE *eventinstance, EVENTDESCRIPTION **description);
int FMOD_Studio_EventInstance_GetPaused(EVENTINSTANCE *eventinstance, int *paused);
int FMOD_Studio_EventInstance_GetPlaybackState(EVENTINSTANCE *eventinstance, int *state);
int FMOD_Studio_EventInstance_GetVolume(EVENTINSTANCE *eventinstance, float *volume, float *finalvolume);
int FMOD_Studio_EventInstance_Release(EVENTINSTANCE *eventinstance);
int FMOD_Studio_EventInstance_Set3DAttributes(EVENTINSTANCE *eventinstance, int *attributes);
int FMOD_Studio_EventInstance_SetParameterValue(EVENTINSTANCE *eventinstance, char *name, float value);
int FMOD_Studio_EventInstance_SetPaused(EVENTINSTANCE *eventinstance, int paused);
int FMOD_Studio_EventInstance_SetVolume(EVENTINSTANCE *eventinstance, float volume);
int FMOD_Studio_EventInstance_Start(EVENTINSTANCE *eventinstance);
int FMOD_Studio_EventInstance_Stop(EVENTINSTANCE *eventinstance, int mode);
int FMOD_Studio_EventInstance_TriggerCue(EVENTINSTANCE *eventinstance);
int FMOD_Studio_System_Create(SYSTEM **system, unsigned int headerversion);
int FMOD_Studio_System_GetBus(SYSTEM *system, char *path, BUS **bus);
int FMOD_Studio_System_GetEvent(SYSTEM *system, const char *path, EVENTDESCRIPTION **event);
int FMOD_Studio_System_GetLowLevelSystem(SYSTEM *system, int **lowLevelSystem);
int FMOD_Studio_System_GetListenerAttributes(SYSTEM *system, int listener, int *attributes);
int FMOD_Studio_System_GetVCA(SYSTEM *system, char *path, VCA **vca);
int FMOD_Studio_System_Initialize(SYSTEM *system, int maxchannels, FM_STUDIO_INITFLAGS studioflags, FM_INITFLAGS flags, void *extradriverdata);
int FMOD_Studio_System_LoadBankFile(SYSTEM *system, const char *filename, FM_STUDIO_LOAD_BANK_FLAGS flags, BANK **bank);
int FMOD_Studio_System_Release(SYSTEM *system);
int FMOD_Studio_System_SetListenerAttributes();	// TODO: complete function signature
int FMOD_Studio_System_Update(SYSTEM *system);
int FMOD_Studio_VCA_SetVolume(VCA *vca, float volume);
int FMOD_Studio_VCA_GetVolume(VCA *vca, float *volume, float *finalvolume);

int get_sound_idx(char *path);

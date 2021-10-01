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
	return FM_OK; \
} while (0)

typedef enum FM_RESULT {
	FM_OK,
	FM_ERR_BADCOMMAND,
	FM_ERR_CHANNEL_ALLOC,
	FM_ERR_CHANNEL_STOLEN,
	FM_ERR_DMA,
	FM_ERR_DSP_CONNECTION,
	FM_ERR_DSP_DONTPROCESS,
	FM_ERR_DSP_FORMAT,
	FM_ERR_DSP_INUSE,
	FM_ERR_DSP_NOTFOUND,
	FM_ERR_DSP_RESERVED,
	FM_ERR_DSP_SILENCE,
	FM_ERR_DSP_TYPE,
	FM_ERR_FILE_BAD,
	FM_ERR_FILE_COULDNOTSEEK,
	FM_ERR_FILE_DISKEJECTED,
	FM_ERR_FILE_EOF,
	FM_ERR_FILE_ENDOFDATA,
	FM_ERR_FILE_NOTFOUND,
	FM_ERR_FORMAT,
	FM_ERR_HEADER_MISMATCH,
	FM_ERR_HTTP,
	FM_ERR_HTTP_ACCESS,
	FM_ERR_HTTP_PROXY_AUTH,
	FM_ERR_HTTP_SERVER_ERROR,
	FM_ERR_HTTP_TIMEOUT,
	FM_ERR_INITIALIZATION,
	FM_ERR_INITIALIZED,
	FM_ERR_INTERNAL,
	FM_ERR_INVALID_FLOAT,
	FM_ERR_INVALID_HANDLE,
	FM_ERR_INVALID_PARAM,
	FM_ERR_INVALID_POSITION,
	FM_ERR_INVALID_SPEAKER,
	FM_ERR_INVALID_SYNCPOINT,
	FM_ERR_INVALID_THREAD,
	FM_ERR_INVALID_VECTOR,
	FM_ERR_MAXAUDIBLE,
	FM_ERR_MEMORY,
	FM_ERR_MEMORY_CANTPOINT,
	FM_ERR_NEEDS3D,
	FM_ERR_NEEDSHARDWARE,
	FM_ERR_NET_CONNECT,
	FM_ERR_NET_SOCKET_ERROR,
	FM_ERR_NET_URL,
	FM_ERR_NET_WOULD_BLOCK,
	FM_ERR_NOTREADY,
	FM_ERR_OUTPUT_ALLOCATED,
	FM_ERR_OUTPUT_CREATEBUFFER,
	FM_ERR_OUTPUT_DRIVERCALL,
	FM_ERR_OUTPUT_FORMAT,
	FM_ERR_OUTPUT_INIT,
	FM_ERR_OUTPUT_NODRIVERS,
	FM_ERR_PLUGIN,
	FM_ERR_PLUGIN_MISSING,
	FM_ERR_PLUGIN_RESOURCE,
	FM_ERR_PLUGIN_VERSION,
	FM_ERR_RECORD,
	FM_ERR_REVERB_CHANNELGROUP,
	FM_ERR_REVERB_INSTANCE,
	FM_ERR_SUBSOUNDS,
	FM_ERR_SUBSOUND_ALLOCATED,
	FM_ERR_SUBSOUND_CANTMOVE,
	FM_ERR_TAGNOTFOUND,
	FM_ERR_TOOMANYCHANNELS,
	FM_ERR_TRUNCATED,
	FM_ERR_UNIMPLEMENTED,
	FM_ERR_UNINITIALIZED,
	FM_ERR_UNSUPPORTED,
	FM_ERR_VERSION,
	FM_ERR_EVENT_ALREADY_LOADED,
	FM_ERR_EVENT_LIVEUPDATE_BUSY,
	FM_ERR_EVENT_LIVEUPDATE_MISMATCH,
	FM_ERR_EVENT_LIVEUPDATE_TIMEOUT,
	FM_ERR_EVENT_NOTFOUND,
	FM_ERR_STUDIO_UNINITIALIZED,
	FM_ERR_STUDIO_NOT_LOADED,
	FM_ERR_INVALID_STRING,
	FM_ERR_ALREADY_LOCKED,
	FM_ERR_NOT_LOCKED,
	FM_ERR_RECORD_DISCONNECTED,
	FM_ERR_TOOMANYSAMPLES
} FM_RESULT;

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

FM_RESULT FMOD_Studio_Bank_LoadSampleData(BANK *bank);
FM_RESULT FMOD_Studio_Bus_GetPaused(BUS *bus, bool *paused);
FM_RESULT FMOD_Studio_Bus_SetPaused(BUS *bus, bool paused);
FM_RESULT FMOD_Studio_Bus_StopAllEvents(BUS *bus, int mode);
FM_RESULT FMOD_Studio_EventDescription_CreateInstance(EVENTDESCRIPTION *eventdescription, EVENTINSTANCE **instance);
FM_RESULT FMOD_Studio_EventDescription_GetPath(EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved);
FM_RESULT FMOD_Studio_EventDescription_Is3D(EVENTDESCRIPTION *eventdescription, bool *is3D);
FM_RESULT FMOD_Studio_EventDescription_IsOneshot(EVENTDESCRIPTION *eventdescription, int *oneshot);
FM_RESULT FMOD_Studio_EventDescription_LoadSampleData(EVENTDESCRIPTION *eventdescription);
FM_RESULT FMOD_Studio_EventInstance_Get3DAttributes(EVENTINSTANCE *eventinstance, int *attributes);
FM_RESULT FMOD_Studio_EventInstance_GetDescription(EVENTINSTANCE *eventinstance, EVENTDESCRIPTION **description);
FM_RESULT FMOD_Studio_EventInstance_GetPaused(EVENTINSTANCE *eventinstance, int *paused);
FM_RESULT FMOD_Studio_EventInstance_GetPlaybackState(EVENTINSTANCE *eventinstance, int *state);
FM_RESULT FMOD_Studio_EventInstance_GetVolume(EVENTINSTANCE *eventinstance, float *volume, float *finalvolume);
FM_RESULT FMOD_Studio_EventInstance_Release(EVENTINSTANCE *eventinstance);
FM_RESULT FMOD_Studio_EventInstance_Set3DAttributes(EVENTINSTANCE *eventinstance, int *attributes);
FM_RESULT FMOD_Studio_EventInstance_SetParameterValue(EVENTINSTANCE *eventinstance, char *name, float value);
FM_RESULT FMOD_Studio_EventInstance_SetPaused(EVENTINSTANCE *eventinstance, int paused);
FM_RESULT FMOD_Studio_EventInstance_SetVolume(EVENTINSTANCE *eventinstance, float volume);
FM_RESULT FMOD_Studio_EventInstance_Start(EVENTINSTANCE *eventinstance);
FM_RESULT FMOD_Studio_EventInstance_Stop(EVENTINSTANCE *eventinstance, int mode);
FM_RESULT FMOD_Studio_EventInstance_TriggerCue(EVENTINSTANCE *eventinstance);
FM_RESULT FMOD_Studio_System_Create(SYSTEM **system, unsigned int headerversion);
FM_RESULT FMOD_Studio_System_GetBus(SYSTEM *system, char *path, BUS **bus);
FM_RESULT FMOD_Studio_System_GetEvent(SYSTEM *system, const char *path, EVENTDESCRIPTION **event);
FM_RESULT FMOD_Studio_System_GetLowLevelSystem(SYSTEM *system, int **lowLevelSystem);
FM_RESULT FMOD_Studio_System_GetListenerAttributes(SYSTEM *system, int listener, int *attributes);
FM_RESULT FMOD_Studio_System_GetVCA(SYSTEM *system, char *path, VCA **vca);
FM_RESULT FMOD_Studio_System_Initialize(SYSTEM *system, int maxchannels, FM_STUDIO_INITFLAGS studioflags, FM_INITFLAGS flags, void *extradriverdata);
FM_RESULT FMOD_Studio_System_LoadBankFile(SYSTEM *system, const char *filename, FM_STUDIO_LOAD_BANK_FLAGS flags, BANK **bank);
FM_RESULT FMOD_Studio_System_Release(SYSTEM *system);
FM_RESULT FMOD_Studio_System_SetListenerAttributes();	// TODO: complete function signature
FM_RESULT FMOD_Studio_System_Update(SYSTEM *system);
FM_RESULT FMOD_Studio_VCA_SetVolume(VCA *vca, float volume);
FM_RESULT FMOD_Studio_VCA_GetVolume(VCA *vca, float *volume, float *finalvolume);

int get_sound_idx(char *path);

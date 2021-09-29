#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <err.h>
#include <libgen.h>
#include <json-c/json.h>
#include <json-c/json_util.h>

#define PROJ	"FMOAD-CAILIN"	// project name
#define MAXSTR	1024

#define FM_SYSTEM			int
#define FM_SOUND			int
#define FM_STUDIO_INITFLAGS		unsigned int
#define FM_INITFLAGS			unsigned int
#define FM_STUDIO_LOAD_BANK_FLAGS	unsigned int

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

#define FM_VECTOR			float

typedef struct {
	FM_VECTOR position;
	FM_VECTOR velocity;
	FM_VECTOR forward;
	FM_VECTOR up;
} FM_3D_ATTRIBUTES;

typedef struct {
	const char *name;
	const char *parentdir;
	const char *bankpath;
	const char *dirbank;
} FM_STUDIO_BANK;

typedef struct {
	unsigned int maxchannels;
	unsigned int flags;
	unsigned int studioflags;
} FM_STUDIO_SYSTEM;

typedef struct {
	const char *path;
	float volume;
	float finalvolume;
} FM_STUDIO_VCA;

typedef struct {
	const char *path;
	bool paused;
} FM_STUDIO_BUS;

typedef struct {
	FM_STUDIO_SYSTEM *sys;
	const char *path;
	FM_SOUND **sounds;
} FM_STUDIO_EVENTDESCRIPTION;

FM_RESULT FMOD_Studio_Bank_LoadSampleData(FM_STUDIO_BANK *bank);
FM_RESULT FMOD_Studio_Bus_GetPaused(FM_STUDIO_BUS *bus, bool *paused);
FM_RESULT FMOD_Studio_Bus_SetPaused(FM_STUDIO_BUS *bus, bool paused);
FM_RESULT FMOD_Studio_Bus_StopAllEvents(FM_STUDIO_BUS *bus, int mode);
FM_RESULT FMOD_Studio_EventDescription_CreateInstance(FM_STUDIO_EVENTDESCRIPTION *eventdescription, int **instance);
FM_RESULT FMOD_Studio_EventDescription_GetPath(FM_STUDIO_EVENTDESCRIPTION *eventdescription, char *path, int size, int *retrieved);
FM_RESULT FMOD_Studio_EventDescription_Is3D(FM_STUDIO_EVENTDESCRIPTION *eventdescription, bool *is3D);
FM_RESULT FMOD_Studio_EventDescription_IsOneshot(FM_STUDIO_EVENTDESCRIPTION *eventdescription, int *oneshot);
FM_RESULT FMOD_Studio_EventDescription_LoadSampleData(FM_STUDIO_EVENTDESCRIPTION *eventdescription);
FM_RESULT FMOD_Studio_EventInstance_Get3DAttributes(int *eventinstance, int *attributes);
FM_RESULT FMOD_Studio_EventInstance_GetDescription(int *eventinstance, FM_STUDIO_EVENTDESCRIPTION **description);
FM_RESULT FMOD_Studio_EventInstance_GetPaused(int *eventinstance, int *paused);
FM_RESULT FMOD_Studio_EventInstance_GetPlaybackState(int *eventinstance, int *state);
FM_RESULT FMOD_Studio_EventInstance_GetVolume(int *eventinstance, float *volume, float *finalvolume);
FM_RESULT FMOD_Studio_EventInstance_Release(int *eventinstance);
FM_RESULT FMOD_Studio_EventInstance_Set3DAttributes(int *eventinstance, int *attributes);
FM_RESULT FMOD_Studio_EventInstance_SetParameterValue(int *eventinstance, char *name, float value);
FM_RESULT FMOD_Studio_EventInstance_SetPaused(int *eventinstance, int paused);
FM_RESULT FMOD_Studio_EventInstance_SetVolume(int *eventinstance, float volume);
FM_RESULT FMOD_Studio_EventInstance_Start(int *eventinstance);
FM_RESULT FMOD_Studio_EventInstance_Stop(int *eventinstance, int mode);
FM_RESULT FMOD_Studio_EventInstance_TriggerCue(int *eventinstance);
FM_RESULT FMOD_Studio_System_Create(FM_STUDIO_SYSTEM **system, unsigned int headerversion);
FM_RESULT FMOD_Studio_System_GetBus(FM_STUDIO_SYSTEM *system, char *path, FM_STUDIO_BUS **bus);
FM_RESULT FMOD_Studio_System_GetEvent(FM_STUDIO_SYSTEM *system, const char *path, FM_STUDIO_EVENTDESCRIPTION **event);
FM_RESULT FMOD_Studio_System_GetLowLevelSystem(FM_STUDIO_SYSTEM *system, int **lowLevelSystem);
FM_RESULT FMOD_Studio_System_GetListenerAttributes(FM_STUDIO_SYSTEM *system, int listener, int *attributes);
FM_RESULT FMOD_Studio_System_GetVCA(FM_STUDIO_SYSTEM *system, char *path, FM_STUDIO_VCA **vca);
FM_RESULT FMOD_Studio_System_Initialize(FM_STUDIO_SYSTEM *system, int maxchannels, FM_STUDIO_INITFLAGS studioflags, FM_INITFLAGS flags, void *extradriverdata);
FM_RESULT FMOD_Studio_System_LoadBankFile(FM_STUDIO_SYSTEM *system, const char *filename, FM_STUDIO_LOAD_BANK_FLAGS flags, FM_STUDIO_BANK **bank);
FM_RESULT FMOD_Studio_System_Release(FM_STUDIO_SYSTEM *system);
FM_RESULT FMOD_Studio_System_SetListenerAttributes();	// TODO: complete function signature
FM_RESULT FMOD_Studio_System_Update(FM_STUDIO_SYSTEM *system);
FM_RESULT FMOD_Studio_VCA_SetVolume(FM_STUDIO_VCA *vca, float volume);
FM_RESULT FMOD_Studio_VCA_GetVolume(FM_STUDIO_VCA *vca, float *volume, float *finalvolume);

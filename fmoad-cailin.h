#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <libgen.h>
#include <json-c/json.h>
#include <json-c/json_util.h>

// TEMPORARY VARIABLES FOR TESTING. REMOVE LATER!
int SFX_LOADED = 0;
// END OF TEMPORARY VARIABLES

#define MAXSTR	1024

typedef enum FMOD_RESULT {
	FMOD_OK,
	FMOD_ERR_BADCOMMAND,
	FMOD_ERR_CHANNEL_ALLOC,
	FMOD_ERR_CHANNEL_STOLEN,
	FMOD_ERR_DMA,
	FMOD_ERR_DSP_CONNECTION,
	FMOD_ERR_DSP_DONTPROCESS,
	FMOD_ERR_DSP_FORMAT,
	FMOD_ERR_DSP_INUSE,
	FMOD_ERR_DSP_NOTFOUND,
	FMOD_ERR_DSP_RESERVED,
	FMOD_ERR_DSP_SILENCE,
	FMOD_ERR_DSP_TYPE,
	FMOD_ERR_FILE_BAD,
	FMOD_ERR_FILE_COULDNOTSEEK,
	FMOD_ERR_FILE_DISKEJECTED,
	FMOD_ERR_FILE_EOF,
	FMOD_ERR_FILE_ENDOFDATA,
	FMOD_ERR_FILE_NOTFOUND,
	FMOD_ERR_FORMAT,
	FMOD_ERR_HEADER_MISMATCH,
	FMOD_ERR_HTTP,
	FMOD_ERR_HTTP_ACCESS,
	FMOD_ERR_HTTP_PROXY_AUTH,
	FMOD_ERR_HTTP_SERVER_ERROR,
	FMOD_ERR_HTTP_TIMEOUT,
	FMOD_ERR_INITIALIZATION,
	FMOD_ERR_INITIALIZED,
	FMOD_ERR_INTERNAL,
	FMOD_ERR_INVALID_FLOAT,
	FMOD_ERR_INVALID_HANDLE,
	FMOD_ERR_INVALID_PARAM,
	FMOD_ERR_INVALID_POSITION,
	FMOD_ERR_INVALID_SPEAKER,
	FMOD_ERR_INVALID_SYNCPOINT,
	FMOD_ERR_INVALID_THREAD,
	FMOD_ERR_INVALID_VECTOR,
	FMOD_ERR_MAXAUDIBLE,
	FMOD_ERR_MEMORY,
	FMOD_ERR_MEMORY_CANTPOINT,
	FMOD_ERR_NEEDS3D,
	FMOD_ERR_NEEDSHARDWARE,
	FMOD_ERR_NET_CONNECT,
	FMOD_ERR_NET_SOCKET_ERROR,
	FMOD_ERR_NET_URL,
	FMOD_ERR_NET_WOULD_BLOCK,
	FMOD_ERR_NOTREADY,
	FMOD_ERR_OUTPUT_ALLOCATED,
	FMOD_ERR_OUTPUT_CREATEBUFFER,
	FMOD_ERR_OUTPUT_DRIVERCALL,
	FMOD_ERR_OUTPUT_FORMAT,
	FMOD_ERR_OUTPUT_INIT,
	FMOD_ERR_OUTPUT_NODRIVERS,
	FMOD_ERR_PLUGIN,
	FMOD_ERR_PLUGIN_MISSING,
	FMOD_ERR_PLUGIN_RESOURCE,
	FMOD_ERR_PLUGIN_VERSION,
	FMOD_ERR_RECORD,
	FMOD_ERR_REVERB_CHANNELGROUP,
	FMOD_ERR_REVERB_INSTANCE,
	FMOD_ERR_SUBSOUNDS,
	FMOD_ERR_SUBSOUND_ALLOCATED,
	FMOD_ERR_SUBSOUND_CANTMOVE,
	FMOD_ERR_TAGNOTFOUND,
	FMOD_ERR_TOOMANYCHANNELS,
	FMOD_ERR_TRUNCATED,
	FMOD_ERR_UNIMPLEMENTED,
	FMOD_ERR_UNINITIALIZED,
	FMOD_ERR_UNSUPPORTED,
	FMOD_ERR_VERSION,
	FMOD_ERR_EVENT_ALREADY_LOADED,
	FMOD_ERR_EVENT_LIVEUPDATE_BUSY,
	FMOD_ERR_EVENT_LIVEUPDATE_MISMATCH,
	FMOD_ERR_EVENT_LIVEUPDATE_TIMEOUT,
	FMOD_ERR_EVENT_NOTFOUND,
	FMOD_ERR_STUDIO_UNINITIALIZED,
	FMOD_ERR_STUDIO_NOT_LOADED,
	FMOD_ERR_INVALID_STRING,
	FMOD_ERR_ALREADY_LOCKED,
	FMOD_ERR_NOT_LOCKED,
	FMOD_ERR_RECORD_DISCONNECTED,
	FMOD_ERR_TOOMANYSAMPLES
} FMOD_RESULT;

#define FMOD_VECTOR			float

typedef struct {
	FMOD_VECTOR position;
	FMOD_VECTOR velocity;
	FMOD_VECTOR forward;
	FMOD_VECTOR up;
} FMOD_3D_ATTRIBUTES;

typedef struct {
	const char *name;
	const char *parentdir;
	const char *fullpath;
} FMOD_STUDIO_BANK;

#define FMOD_STUDIO_SYSTEM		int
#define FMOD_SYSTEM			int
#define FMOD_STUDIO_INITFLAGS		unsigned int
#define FMOD_INITFLAGS			unsigned int
#define FMOD_STUDIO_LOAD_BANK_FLAGS	unsigned int
//#define FMOD_STUDIO_BANK		char	// XXX: may be wrong type; I think this needs a struct

#define STUB() do { \
	fprintf(stderr, "%s: STUB\n", __func__); \
	return FMOD_OK; \
} while (0)

FMOD_RESULT FMOD_Studio_Bank_LoadSampleData(FMOD_STUDIO_BANK *bank);
FMOD_RESULT FMOD_Studio_Bus_GetPaused(int *bus, int *paused);
FMOD_RESULT FMOD_Studio_Bus_SetPaused(int *bus, int paused);
FMOD_RESULT FMOD_Studio_Bus_StopAllEvents(int *bus, int mode);
FMOD_RESULT FMOD_Studio_EventDescription_CreateInstance(int *eventdescription, int **instance);
FMOD_RESULT FMOD_Studio_EventDescription_GetPath(int *eventdescription, char *path, int size, int *retrieved);
FMOD_RESULT FMOD_Studio_EventDescription_Is3D(int *eventdescription, int *is3D);
FMOD_RESULT FMOD_Studio_EventDescription_IsOneshot(int *eventdescription, int *oneshot);
FMOD_RESULT FMOD_Studio_EventDescription_LoadSampleData(int *eventdescription);
FMOD_RESULT FMOD_Studio_EventInstance_Get3DAttributes(int *eventinstance, int *attributes);
FMOD_RESULT FMOD_Studio_EventInstance_GetDescription(int *eventinstance, int **description);
FMOD_RESULT FMOD_Studio_EventInstance_GetPaused(int *eventinstance, int *paused);
FMOD_RESULT FMOD_Studio_EventInstance_GetPlaybackState(int *eventinstance, int *state);
FMOD_RESULT FMOD_Studio_EventInstance_GetVolume(int *eventinstance, float *volume, float *finalvolume);
FMOD_RESULT FMOD_Studio_EventInstance_Release(int *eventinstance);
FMOD_RESULT FMOD_Studio_EventInstance_Set3DAttributes(int *eventinstance, int *attributes);
FMOD_RESULT FMOD_Studio_EventInstance_SetParameterValue(int *eventinstance, char *name, float value);
FMOD_RESULT FMOD_Studio_EventInstance_SetPaused(int *eventinstance, int paused);
FMOD_RESULT FMOD_Studio_EventInstance_SetVolume(int *eventinstance, float volume);
FMOD_RESULT FMOD_Studio_EventInstance_Start(int *eventinstance);
FMOD_RESULT FMOD_Studio_EventInstance_Stop(int *eventinstance, int mode);
FMOD_RESULT FMOD_Studio_EventInstance_TriggerCue(int *eventinstance);
FMOD_RESULT FMOD_Studio_System_Create();
FMOD_RESULT FMOD_Studio_System_GetBus(int *system, char *path, int **bus);
FMOD_RESULT FMOD_Studio_System_GetEvent(int *system, char *path, int **event);
FMOD_RESULT FMOD_Studio_System_GetLowLevelSystem(int *system, int **lowLevelSystem);
FMOD_RESULT FMOD_Studio_System_GetListenerAttributes(int *system, int listener, int *attributes);
FMOD_RESULT FMOD_Studio_System_GetVCA(int *system, char *path, int **vca);
FMOD_RESULT FMOD_Studio_System_Initialize();
FMOD_RESULT FMOD_Studio_System_LoadBankFile(FMOD_STUDIO_SYSTEM *system, const char *filename, FMOD_STUDIO_LOAD_BANK_FLAGS flags, FMOD_STUDIO_BANK **bank);
FMOD_RESULT FMOD_Studio_System_Release(int *system);
FMOD_RESULT FMOD_Studio_System_SetListenerAttributes();
FMOD_RESULT FMOD_Studio_System_Update(int *system);
FMOD_RESULT FMOD_Studio_VCA_SetVolume(int *vca, float volume);
FMOD_RESULT FMOD_Studio_VCA_GetVolume(int *vca, float *volume, float *finalvolume);

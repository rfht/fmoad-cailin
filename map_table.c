#include <stdio.h>
#include <string.h>

struct fmod_events_map {
	const char *fmod_event;
	const char *ogg_file;
};

const struct fmod_events_map sounds_map[] = {
	{ "event:/char/madeline/jump",
	  "character/madeline/char_mad_jump.ogg" },
	{ "event:/char/madeline/slack",
	  "character/madeline/char_mad_slack.ogg" },
	/* ... */
};

#define nitems(_a)      (sizeof((_a)) / sizeof((_a)[0]))

int main(void)
{
	const struct fmod_events_map *ep;
	char *e = "event:/char/madeline/jump";

	int i;
	for (i = 0, ep = sounds_map; i < nitems(sounds_map); ep++, i++)
		if (!strcmp(e, ep->fmod_event))
			break;

	if (i == nitems(sounds_map))
		return 1;

	printf("%s - %s\n", ep->fmod_event, ep->ogg_file);
	return 0;
}

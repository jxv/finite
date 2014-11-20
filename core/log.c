#include "common.h"

void mkPlaceLog(action_t *a, log_t *l)
{
        (void)a;
        (void)l;
}

void mkLog(action_t *a, log_t *l)
{
        (void)a;
        (void)l;
	switch (a->type) {
	case ACTION_PLACE:
                mkPlaceLog(a, l);
                break;
	default:
                break;
	}
}

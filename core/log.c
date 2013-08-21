#include "common.h"

void mkPlaceLog(struct Action *a, struct Log *l)
{
	NOT(a);
	NOT(l);

	
}

void mkLog(struct Action *a, struct Log *l)
{
	NOT(a);
	NOT(l);

	switch (a->type) {
	case actionPlace: mkPlaceLog(a, l); break;
	default: break;
	}
}


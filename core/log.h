#ifndef FINITE_CORE_LOG_H
#define FINITE_CORE_LOG_H

struct Log
{
	ActionType type;
	union {
	ActionErrType err;
	} data;
};

void mkLog(struct Action *, struct Log *);

#endif


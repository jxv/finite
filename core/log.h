#ifndef FINITE_CORE_LOG_H
#define FINITE_CORE_LOG_H

typedef struct Log
{
	ActionType type;
	union {
	ActionErrType err;
	} data;
} Log;

void mkLog(Action *, Log *);

#endif


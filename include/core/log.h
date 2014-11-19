#ifndef FINITE_CORE_LOG_H
#define FINITE_CORE_LOG_H

typedef struct log
{
	action_tag_t type;
	union {
	        action_err_tag_t err;
	} data;
} Log;

typedef struct log log_t;

void mkLog(action_t *, log_t *);

#endif


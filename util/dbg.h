#ifndef FINITE_UTIL_DBG_H
#define FINITE_UTIL_DBG_H

#include <assert.h>


#ifdef LOGGING
#define LOG(x)			printf("["x"]\n")
#else
#define LOG(x)			((void)NULL)
#endif


#define NOT(t)			assert((t) != NULL)
#define RANGE(a,b,c)		assert((a) >= (b) && (a) <= (c))
#define XRANGE(a,b,c)		assert((a) > (b) && (a) < (c))


#endif



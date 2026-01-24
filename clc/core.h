#ifndef _CORE_H_
#define _CORE_H_

#include <stdbool.h>
#include <stddef.h>


/* == useful macros == */


/** Marks a parameter that can be NULL */
#define CL_NULLABLE(T)  T

#define CL_STR0(s)      #s
#define CL_STR(s)       CL_STR0(s)


/* == auxiliary types == */

typedef char *const string_t;


#endif /* _CORE_H_ */

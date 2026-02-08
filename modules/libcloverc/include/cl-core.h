#ifndef _CORE_H_
#define _CORE_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* == useful macros == */

#define __CL_TNAME(T)   _##T

#define CL_TYPE(T)      typedef struct __CL_TNAME(T) T; struct __CL_TNAME(T)
#define CL_ENUM(T)      typedef enum __CL_TNAME(T) T; enum __CL_TNAME(T)

#define CL_STRING0(s)   #s
#define CL_STRING(s)    CL_STRING0(s)

#define CL_VOIDPTR(ptr) ((void *)ptr)

#define CL_N_ELEMS(arr) (sizeof(arr) / sizeof(*arr))

/* == auxiliary types == */

/* String */
typedef const char *str_t;

/* String view */
typedef const char *const sview_t;

#endif /* _CORE_H_ */

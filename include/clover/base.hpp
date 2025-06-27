#ifndef CLOVER_BASE_H_
#define CLOVER_BASE_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* Handy macros */

#define _CLV_STRING0(x)             #x
#define CLV_STRING(x)               _CLV_STRING0 (x)

/* Calculates the length of an array. */
#define CLV_LENGTH(x)               (sizeof (x) / sizeof (*x))

/* Gets a value from an array, or a fallback value if the given index is out of bounds of the array. */
#define CLV_GET_OR(arr,i,fallback)  (((i) >= 0 && (i) < CLV_LENGTH (arr)) ? arr[(i)] : (fallback))

/* Attribute macros */

#define _CLV_NODISCARD              __attribute_warn_unused_result__
#define _CLV_NOTHROW                __THROWNL
#define _CLV_LEAF                   __attribute__ ((__leaf__))
#define _CLV_DEPRECATED(msg)        __attribute_deprecated__(msg)
#define _CLV_ALWAYS_INLINE          __always_inline

/* Type macros */

#define CLV_VOIDPTR(p)              ((void *)(p))

/* Type defs */

typedef const char *clv_str;

#endif /* CLOVER_BASE_H_ */

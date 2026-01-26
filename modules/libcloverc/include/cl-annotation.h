#ifndef CL_ANNOTATION_H_
#define CL_ANNOTATION_H_

#ifdef _MSC_VER
#define __attribute__(...) /* ignore */
#endif /* _MSC_VER */

/**
 * Marks a parameter that is a pointer used as input
 * of a function and can be set to NULL.
 */
#define __Nullable

/**
 * Marks a parameter that is a pointer used as output
 * of a function and can NOT be set to NULL.
 */
#define __Out

/**
 * Marks a parameter that is a pointer whose ownership
 * is taken by the called function.
 */
#define __Owned

/**
 * Marks a function that never returns NULL.
 */
#define __NonNull __attribute__((returns_nonnull))

/**
 * Marks a function of which the result MUST NOT be
 * discarded.
 */
#define __NoDiscard __attribute__((warn_unused_result))

/**
 * Marks a function that accepts a format string like
 * printf.
 *
 * @param f the format string argument
 * @param a the index of the variadic arguments
 */
#define __Format(f,a) __attribute__((format (printf, f, a)))

#endif /* CL_ANNOTATION_H_ */

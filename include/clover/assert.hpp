#ifndef CLOVER_ASSERT_H_
#define CLOVER_ASSERT_H_

#include <clover/base.hpp>
#include <clover/log.hpp>

#define clv_assert(expr,_tail) \
    do { \
        if (!(expr)) { \
            clv_error ("assertion failed: " #expr); \
            _tail; \
        } \
    } while (0)

#endif /* CLOVER_ASSERT_H_ */

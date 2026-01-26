#ifndef CL_VECTOR_H_
#define CL_VECTOR_H_

#include "cl-core.h"
#include "cl-annotation.h"

#define CL_VECTOR_INITIAL_CAPACITY  16
#define CL_VECTOR_GROWTH_PERCENT    50


CL_TYPE(Vector) {
    void **data;
    size_t count;
    size_t capacity;
};

typedef void (*VectorCallbackFn)(void *user_data);


Vector *vector_new  () __NoDiscard;
bool    vector_push (Vector *self, void *ptr);
void   *vector_pop  (Vector *self);
void   *vector_at   (Vector *self, size_t index);
void    vector_iter (Vector *self, VectorCallbackFn iter);
void    vector_trim (Vector *self);
void    vector_free (Vector *self);

#endif /* CL_VECTOR_H_ */

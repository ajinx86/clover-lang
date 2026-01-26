#define CL_LOG_SCOPE "vector"

#include "cl-log.h"
#include "cl-vector.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>


#define CL_VECTOR_DATA_SIZE sizeof(void *)


static bool _vector_enlarge(Vector *self) {
    if (self->count + 1 < self->capacity) {
        return true;
    }

    size_t increase_amount = self->capacity * 100 / CL_VECTOR_GROWTH_PERCENT;
    size_t new_capacity = self->capacity + increase_amount;

    size_t old_size = self->capacity * CL_VECTOR_DATA_SIZE;
    size_t new_size = new_capacity * CL_VECTOR_DATA_SIZE;

    if (new_size < old_size) {
        cl_debug("%s: maximum capacity reached\n", __func__);
        return false;
    }

    void **tmp = realloc(self->data, new_size);

    if (!tmp) {
        cl_debug("%s: %s\n", __func__, strerror(errno));
        return false;
    }

    self->capacity = new_capacity;
    self->data = tmp;

    return true;
}


Vector *vector_new() {
    Vector *new_vector = malloc(sizeof(Vector));

    if (!new_vector) {
        cl_debug("%s: %s\n", __func__, strerror(errno));
        return NULL;
    }

    void *data = calloc(CL_VECTOR_INITIAL_CAPACITY, CL_VECTOR_DATA_SIZE);

    if (!data) {
        free(new_vector);
        cl_debug("%s: %s\n", __func__, strerror(errno));
        return NULL;
    }

    new_vector->data = data;
    new_vector->count = 0;
    new_vector->capacity = CL_VECTOR_INITIAL_CAPACITY;

    return new_vector;
}


bool vector_push(Vector *self, void *ptr) {
    if (!_vector_enlarge(self)) {
        return false;
    }

    self->data[self->count] = ptr;
    self->count++;

    return true;
}


void *vector_pop(Vector *self) {
    if (self->count == 0) {
        return NULL;
    }

    self->count--;

    return self->data[self->count];
}


void *vector_at(Vector *self, size_t index) {
    if (index >= self->count) {
        return NULL;
    }

    return self->data[index];
}


void vector_iter(Vector *self, VectorCallbackFn callback) {
    for (size_t i = 0; i < self->count; i++) {
        callback(self->data[i]);
    }
}


void vector_trim(Vector *self) {
    if ((self->capacity - self->count) <= (CL_VECTOR_INITIAL_CAPACITY * 4)) {
        return;
    }

    void **tmp = realloc(self->data, self->count * CL_VECTOR_DATA_SIZE);

    if (!tmp) {
        cl_debug("%s: %s\n", __func__, strerror(errno));
        return;
    }

    self->data = tmp;
    self->capacity = self->count;
}


void vector_free(Vector *self) {
    free(CL_VOIDPTR(self->data));
    free(CL_VOIDPTR(self));
}

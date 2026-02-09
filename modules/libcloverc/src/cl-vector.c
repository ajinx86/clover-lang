#define CL_LOG_SCOPE "vector"

#include "cl-log.h"
#include "cl-vector.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>


static __Inline void *_vector_getaddr(Vector *self, size_t index) {
    return self->data + index * self->item_size;
}


static __Inline void **_vector_getaddrp(Vector *self, size_t index) {
    return (void **)(self->data + index * self->item_size);
}


static bool _vector_enlarge(Vector *self) {
    if (self->count + 1 < self->capacity) {
        return true;
    }

    size_t increase_amount = self->capacity * 100 / CL_VECTOR_GROWTH_PERCENT;
    size_t new_capacity = self->capacity + increase_amount;

    size_t old_size = self->capacity * self->item_size;
    size_t new_size = new_capacity * self->item_size;

    if (new_size < old_size) {
        cl_debug("%s: maximum capacity reached\n", __func__);
        return false;
    }

    void *tmp = realloc(self->data, new_size);

    if (!tmp) {
        cl_debug("%s: %s\n", __func__, strerror(errno));
        return false;
    }

    self->capacity = new_capacity;
    self->data = tmp;

    return true;
}


Vector *vector_new(size_t item_size) {
    Vector *new_vector = malloc(sizeof(Vector));

    if (!new_vector) {
        cl_debug("%s: %s\n", __func__, strerror(errno));
        return NULL;
    }

    void *data = calloc(CL_VECTOR_INITIAL_CAPACITY, item_size);

    if (!data) {
        cl_debug("%s: %s\n", __func__, strerror(errno));
        free(new_vector);
        return NULL;
    }

    new_vector->data = data;
    new_vector->count = 0;
    new_vector->capacity = CL_VECTOR_INITIAL_CAPACITY;
    new_vector->item_size = item_size;

    return new_vector;
}


bool vector_push(Vector *self, void *data) {
    if (!_vector_enlarge(self)) {
        return false;
    }

    void *data_addr = _vector_getaddr(self, self->count);
    memcpy(data_addr, data, self->item_size);
    self->count++;

    return true;
}


bool vector_pop(Vector *self, __Out __Nullable void *data) {
    if (self->count == 0) {
        return false;
    }

    self->count--;

    if (data) {
        void *data_addr = _vector_getaddr(self, self->count);
        memcpy(data, data_addr, self->item_size);
    }

    return true;
}


void *vector_get(Vector *self, size_t index) {
    if (index >= self->count) {
        return NULL;
    }

    return _vector_getaddr(self, index);
}


void **vector_getp(Vector *self, size_t index) {
    if (index >= self->count) {
        return NULL;
    }

    return _vector_getaddrp(self, index);
}


void vector_iter(Vector *self, VectorCallbackFn callback) {
    for (size_t i = 0; i < self->count; i++) {
        callback(_vector_getaddr(self, i));
    }
}


void vector_iterp(Vector *self, VectorCallbackFn callback) {
    for (size_t i = 0; i < self->count; i++) {
        callback(_vector_getaddrp(self, i));
    }
}


void vector_trim(Vector *self) {
    if ((self->capacity - self->count) <= (CL_VECTOR_INITIAL_CAPACITY * 4)) {
        return;
    }

    void *tmp = realloc(self->data, self->count * self->item_size);

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

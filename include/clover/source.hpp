#ifndef CLOVER_FILE_H_
#define CLOVER_FILE_H_

#include <clover/base.hpp>

#include <stdio.h>

/* String buffer */
typedef struct clv_source clv_source_t;

clv_source_t *clv_source_new      (clv_str path);
char          clv_source_at       (clv_source_t *self, size_t index);
clv_str       clv_source_offset   (clv_source_t *self, size_t offset);
clv_str       clv_source_substr   (clv_source_t *self, size_t offset, size_t length);
int           clv_source_compare  (clv_source_t *self, clv_str string, size_t offset, size_t length);
clv_str       clv_source_cstr     (clv_source_t *self);
size_t        clv_source_length   (clv_source_t *self);
clv_str       clv_source_get_file (clv_source_t *self);
void          clv_source_free     (clv_source_t *self);

#endif /* CLOVER_FILE_H_ */

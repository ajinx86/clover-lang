#ifndef CL_SOURCE_H_
#define CL_SOURCE_H_

#include "cl-core.h"
#include "cl-annotation.h"

CL_TYPE(Source) {
    str_t  path;
    str_t  text;
    size_t length;
};

Source *source_new   (str_t file) __NoDiscard;
char    source_at    (Source *self, size_t offset);
sview_t source_get   (Source *self, size_t offset);
size_t  source_span  (Source *self, size_t offset, str_t accept);
size_t  source_cspan (Source *self, size_t offset, str_t reject);
size_t  source_lnlen (Source *self, size_t offset);
int     source_cmp   (Source *self, size_t offset, size_t length, str_t other);
void    source_free  (Source *self);

#endif /* CL_SOURCE_H_ */

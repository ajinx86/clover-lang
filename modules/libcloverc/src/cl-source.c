#define CL_LOG_SCOPE "source"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "cl-source.h"
#include "cl-log.h"


/**
 * Reads the text from a file into a malloc'd buffer.
 */
static bool _read_file(str_t path, char **out_text, size_t *out_size) {
    FILE *fp = fopen(path, "r");
    size_t size = 0;
    char *text = NULL;

    if (!fp) {
        cl_error("failed to open file: %s\n", strerror(errno));
        return false;
    }

    // detect file size

    if (fseek(fp, 0, SEEK_END) != 0) {
        cl_error("failed to open file: %s\n", strerror(errno));
        fclose(fp);
        return false;
    }

    size = ftell(fp);
    rewind(fp);

    // allocate text buffer

    text = malloc(size + 1);

    if (!text) {
        cl_error("out of memory!\n");
        fclose(fp);
        return false;
    }

    // read text from file

    size_t total = 0;
    size_t count = 0;

    while ((count = fread(&text[total], 1, 1024, fp)) > 0) {
        total += count;
    }

    text[total] = '\0';

    fclose(fp);

    *out_text = text;
    *out_size = size;

    return true;
}


Source *source_new(str_t path) {
    Source *new_source = malloc(sizeof(Source));

    if (!new_source) {
        return NULL;
    }

    new_source->path = strdup(path);

    if (!new_source->path) {
        free(new_source);
        return NULL;
    }

    if (!_read_file(path, (char **)&new_source->text, &new_source->length)) {
        free(CL_VOIDPTR(new_source->path));
        free(CL_VOIDPTR(new_source));
        return NULL;
    }

    return new_source;
}


char source_at(Source *self, size_t offset) {
    if (offset >= self->length) {
        cl_debug("%s: index out of bounds: %zu\n", __func__, offset);
        return 0;
    }

    return self->text[offset];
}


sview_t source_get(Source *self, size_t offset) {
    if (offset >= self->length) {
        cl_debug("%s: index out of bounds: %zu\n", __func__, offset);
        return NULL;
    }

    return (sview_t)&self->text[offset];
}


size_t source_span(Source *self, size_t offset, str_t accept) {
    if (offset >= self->length) {
        cl_debug("%s: index out of bounds: %zu\n", __func__, offset);
        return 0;
    }

    return strspn(&self->text[offset], accept);
}


size_t source_cspan(Source *self, size_t offset, str_t reject) {
    if (offset >= self->length) {
        cl_debug("%s: index out of bounds: %zu\n", __func__, offset);
        return 0;
    }

    return strcspn(&self->text[offset], reject);
}


size_t source_lnlen(Source *self, size_t offset) {
    return strcspn(&self->text[offset], "\r\n");
}


int source_cmp(Source *self, size_t offset, size_t length, str_t str) {
    return strncmp(self->text + offset, str, length);
}


void source_free(Source *self) {
    free(CL_VOIDPTR(self->path));
    free(CL_VOIDPTR(self->text));
    free(CL_VOIDPTR(self));
}

#include <clover/source.hpp>
#include <clover/log.hpp>

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <errno.h>


struct clv_source {
    clv_str file;
    clv_str data;
    size_t  length;
};


static bool
read_file (clv_str file, char **out_data, size_t *out_length) {
    FILE *fp = fopen (file, "r");

    if (fp == NULL) {
        return false;
    }

    char *data = NULL;
    size_t length = 0;

    if (fseek (fp, 0, SEEK_END) != 0) {
        fclose (fp);
        return false;
    }

    length = ftell (fp);
    rewind (fp);

    if (length == 0) {
        errno = EBADF;
        fclose (fp);
        return false;
    }

    data = static_cast<char *>(malloc (length + 1));

    if (data == NULL) {
        fclose (fp);
        return false;
    }

    size_t offset = 0;
    int read = 0;

    do {
        read = fread (&data[offset], 1, 4096, fp);
        offset += read;

        if (read <= 0) {
            break;
        }
    } while (true);

    fclose (fp);

    *out_data = data;
    *out_length = length;

    return true;
}


clv_source_t *
clv_source_new (clv_str file) {
    clv_source_t *new_src = static_cast<clv_source_t *>(malloc (sizeof (*new_src)));

    if (new_src == NULL) {
        return NULL;
    }

    new_src->file = strdup (file);

    if (new_src->file == NULL) {
        free (new_src);
        return NULL;
    }

    if (!read_file (file, (char **)&new_src->data, &new_src->length)) {
        free (CLV_VOIDPTR (new_src->file));
        free (CLV_VOIDPTR (new_src));
        return NULL;
    }

    return new_src;
}


char
clv_source_at (clv_source_t *self, size_t index) {
    if (self == NULL) {
        errno = EINVAL;
        return '\0';
    }

    if (index >= self->length) {
        errno = EOVERFLOW;
        return '\0';
    }

    return self->data[index];
}


clv_str
clv_source_offset (clv_source_t *self, size_t offset) {
    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (offset >= self->length) {
        errno = EOVERFLOW;
        return NULL;
    }

    return &self->data[offset];
}


clv_str
clv_source_substr (clv_source_t *self, size_t offset, size_t length) {
    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    if (offset + length >= self->length) {
        errno = EOVERFLOW;
        return NULL;
    }

    return strndup ((self->data + offset), length);
}


int
clv_source_compare (clv_source_t *self, clv_str string, size_t offset, size_t length) {
    if (self == NULL || string == NULL) {
        errno = EINVAL;
        return -1;
    }

    if (offset < 0 || offset + length > self->length) {
        errno = EOVERFLOW;
        return -1;
    }

    return strncmp (self->data + offset, string, length);
}


clv_str
clv_source_cstr (clv_source_t *self) {
    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->data;
}


clv_str
clv_source_get_file (clv_source_t *self) {
    if (self == NULL) {
        errno = EINVAL;
        return NULL;
    }

    return self->file;
}


size_t
clv_source_length (clv_source_t *self) {
    if (self == NULL) {
        errno = EINVAL;
        return -1;
    }

    return self->length;
}


void
clv_source_free (clv_source_t *self) {
    if (self == NULL) {
        errno = EINVAL;
        return;
    }

    free (CLV_VOIDPTR (self->file));
    free (CLV_VOIDPTR (self->data));
    free (CLV_VOIDPTR (self));
}

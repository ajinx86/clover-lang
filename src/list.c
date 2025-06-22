#include <clover/list.h>

#include <stdlib.h>
#include <errno.h>

struct clv_list {
    struct clv_list_node *head;
    struct clv_list_node *tail;
    size_t count;
};

struct clv_list_node {
    struct clv_list_node *prev;
    struct clv_list_node *next;
    void *ptr;
};


static struct clv_list_node *
_clv_list_make_node (void *ptr) {
    struct clv_list_node *node = malloc (sizeof (*node));

    if (node == NULL) {
        return NULL;
    }

    node->ptr = ptr;
    node->prev = NULL;
    node->next = NULL;

    return node;
}


struct clv_list *
clv_list_new () {
    clv_list_t *list = malloc (sizeof (*list));

    if (list == NULL) {
        return NULL;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;

    return list;
}

bool
clv_list_push_back (clv_list_t *list, void *ptr) {
    if (list == NULL) {
        return false;
    }

    struct clv_list_node *node = _clv_list_make_node (ptr);

    if (node == NULL) {
        return false;
    }

    if (list->tail == NULL) {
        list->head = node;
        list->tail = node;
    } else {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
    }

    list->count++;

    return true;
}

bool
clv_list_pop_back (clv_list_t *list, void **out_ptr) {
    if (list == NULL || out_ptr == NULL) {
        errno = EINVAL;
        return false;
    }

    if (list->tail != NULL) {
        errno = ENOENT;
        return false;
    }

    struct clv_list_node *prev;

    prev = list->tail->prev;
    *out_ptr = list->tail->ptr;
    free (list->tail);

    list->tail = prev;

    if (prev == NULL) {
        list->head = NULL;
    }

    list->count--;

    return true;
}

size_t
clv_list_length (clv_list_t *list) {
    return list->count;
}

void
clv_list_foreach (clv_list_t *list, clv_list_func_t callback) {
    struct clv_list_node *curr = list->head;

    while (curr != NULL) {
        callback (curr->ptr);
        curr = curr->next;
    }
}

void
clv_list_clear (clv_list_t *list, clv_list_func_t _free_ptr) {
    struct clv_list_node *curr = list->head;
    struct clv_list_node *temp = NULL;

    while (curr != NULL) {
        if (_free_ptr) {
            _free_ptr (curr->ptr);
        }

        temp = curr->next;
        free (curr);

        curr = temp;
    }

    list->head = NULL;
    list->tail = NULL;
    list->count = 0;
}

void
clv_list_free (clv_list_t *list, clv_list_func_t _free_ptr) {
    clv_list_clear (list, _free_ptr);
    free (list);
}


clv_list_iter_t
clv_list_get_at (clv_list_t *list, size_t offset) {
    if (list == NULL || offset >= list->count) {
        return NULL;
    }

    struct clv_list_node *target = list->head;

    for (size_t i = 0; i < offset; i++) {
        target = target->next;
    }

    return target;
}


clv_list_iter_t
clv_list_get_head (clv_list_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->head;
}


clv_list_iter_t
clv_list_get_tail (clv_list_t *list) {
    if (list == NULL) {
        return NULL;
    }

    return list->tail;
}


clv_list_iter_t
clv_list_iter_get_prev (clv_list_iter_t iter) {
    if (iter == NULL) {
        return NULL;
    }

    return ((struct clv_list_node *)iter)->prev;
}


clv_list_iter_t
clv_list_iter_get_next (clv_list_iter_t iter) {
    if (iter == NULL) {
        return NULL;
    }

    return ((struct clv_list_node *)iter)->next;
}


void *
clv_list_iter_get_data (clv_list_iter_t iter) {
    if (iter == NULL) {
        return NULL;
    }

    return ((struct clv_list_node *)iter)->ptr;
}

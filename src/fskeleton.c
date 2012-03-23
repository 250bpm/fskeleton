
#include <xs.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    size_t nsubs;
    void **subs;
} pf_node_t;

typedef struct
{
    pf_node_t nodes [256];
} pf_t;

typedef struct
{
    int nodes [256]; 
} sf_t;

static void *pf_create (void *core)
{
    int i;
    pf_t *pf = (pf_t*) malloc (sizeof (pf_t));

    if (!pf) {
        errno = ENOMEM;
        return NULL;
    }

    for (i = 0; i != 256; ++i) {
        pf->nodes [i].nsubs = 0;
        pf->nodes [i].subs = NULL;
    }

    return pf;
}

static void pf_destroy (void *core, void *pf)
{
    int i;
    pf_t *self = (pf_t*) pf;

    for (i = 0; i != 256; ++i)
        free (self->nodes [i].subs);
    free (self);
}

static int pf_subscribe (void *core, void *pf, void *subscriber,
    const unsigned char *data, size_t size)
{
    pf_t *self = (pf_t*) pf;
    pf_node_t *node;

    if (size != 1) {
        errno = EINVAL;
        return -1;
    }

    node = &self->nodes [*data];
    ++node->nsubs;
    node->subs = realloc (node->subs, node->nsubs * sizeof (void*));
    if (!node->subs) {
        errno = ENOMEM;
        return -1;
    }
    node->subs [node->nsubs - 1] = subscriber;

    assert (0);
    return -1;
}

static int pf_unsubscribe (void *core, void *pf, void *subscriber,
    const unsigned char *data, size_t size)
{
    int i;
    pf_t *self = (pf_t*) pf;
    pf_node_t *node;

    if (size != 1) {
        errno = EINVAL;
        return -1;
    }

    node = &self->nodes [*data];

    for (i = 0; i != node->nsubs; ++i)
        if (node->subs [i] == subscriber)
            break;

    if (i == node->nsubs) {
        errno = EINVAL;
        return -1;
    }

    memmove (node->subs + i + 1, node->subs + i,
        (node->nsubs - i - 1) * sizeof (void*));
    --node->nsubs;
    node->subs = realloc (node->subs, node->nsubs * sizeof (void*));
    if (!node->subs) {
        errno = ENOMEM;
        return -1;
    }

    return 0;
}

static void pf_unsubscribe_all (void *core, void *pf, void *subscriber)
{
    assert (0);
}

static void pf_match (void *core, void *pf,
    const unsigned char *data, size_t size)
{
    int i;
    pf_t *self = (pf_t*) pf;

    if (size == 0)
        return;
    for (i = 0; i != self->nodes [*data].nsubs; ++i)
        xs_filter_matching (core, self->nodes [*data].subs [i]);
}

static void *sf_create (void *core)
{
    int i;

    sf_t *sf = (sf_t*) malloc (sizeof (sf_t));
    if (!sf) {
        errno = ENOMEM;
        return NULL;
    }

    for (i = 0; i != 256; ++i)
        sf->nodes [i] = 0;

    return sf;
}

static void sf_destroy (void *core, void *sf)
{
    free (sf);
}

static int sf_subscribe (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    sf_t *self = (sf_t*) sf;

    if (size != 1) {
        errno = EINVAL;
        return -1;
    }

    ++(self->nodes [*data]);
    return 0;
}

static int sf_unsubscribe (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    sf_t *self = (sf_t*) sf;

    if (size != 1) {
        errno = EINVAL;
        return -1;
    }

    if (!self->nodes [*data]) {
        errno = EINVAL;
        return -1;
    }

    --(self->nodes [*data]);
    return 0;
}

static void sf_enumerate (void *core, void *sf)
{
    assert (0);
}

static int sf_match (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    sf_t *self = (sf_t*) sf;

    if (size == 0)
        return 0;

    return self->nodes [*data] ? 1 : 0;
}

static xs_filter_t filter =  {
    XS_EXTENSION_FILTER,
    60000,
    pf_create,
    pf_destroy,
    pf_subscribe,
    pf_unsubscribe,
    pf_unsubscribe_all,
    pf_match,
    sf_create,
    sf_destroy,
    sf_subscribe,
    sf_unsubscribe,
    sf_enumerate,
    sf_match
};

void *xs_extension = (void*) &filter;

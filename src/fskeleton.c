
#include <xs.h>
#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

/******************************************************************************/
/*  These are the structures used by the example filter. Please, replace      */
/*  them with your own datastructures.                                        */
/******************************************************************************/

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

/******************************************************************************/
/*  End of filter-related data structures.                                    */
/******************************************************************************/

/******************************************************************************/
/*  Following functions implement the example filter functionality. Please,   */
/*  keep the function definitions and replace the actual bodies of the        */
/*  functions by your own filtering code.                                     */
/******************************************************************************/

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

    return node->nsubs == 1 ? 1 : 0;
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
    if (!node->nsubs) {
        free (node->subs);
        node->subs = NULL;
    }
    else {
        node->subs = realloc (node->subs, node->nsubs * sizeof (void*));
        assert (node->subs);
    }

    return node->subs == 0 ? 1 : 0;
}

static void pf_unsubscribe_all (void *core, void *pf, void *subscriber)
{
    int i, j;
    unsigned char c;
    pf_t *self = (pf_t*) pf;
    pf_node_t *node;

    for (j = 0; j != 256; ++j) {
        node = &self->nodes [j];

        for (i = 0; i != node->nsubs; ++i)
            if (node->subs [i] == subscriber)
                break;

        if (i == node->nsubs)
            continue;

        memmove (node->subs + i + 1, node->subs + i,
            (node->nsubs - i - 1) * sizeof (void*));
        --node->nsubs;
        if (!node->nsubs) {
            free (node->subs);
            node->subs = NULL;
        }
        else {
            node->subs = realloc (node->subs, node->nsubs * sizeof (void*));
            assert (node->subs);
        }

        c = (unsigned char) j;
        xs_filter_unsubscribed (core, &c, 1);
    }
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
    return self->nodes [*data] == 1 ? 1 : 0;
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
    return self->nodes [*data] == 0 ? 1 : 0;
}

static void sf_enumerate (void *core, void *sf)
{
    int i;
    unsigned char c;
    sf_t *self = (sf_t*) sf;

    for (i = 0; i != 256; ++i)
        if (self->nodes [i] > 0) {
            c = (unsigned char) i;
            xs_filter_subscribed (core, &c, 1);
        }
}

static int sf_match (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    sf_t *self = (sf_t*) sf;

    if (size == 0)
        return 0;

    return self->nodes [*data] ? 1 : 0;
}

/******************************************************************************/
/*  End of filter implementation.                                             */
/******************************************************************************/

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


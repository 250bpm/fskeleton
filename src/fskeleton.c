
#include <xs.h>
#include <stddef.h>
#include <assert.h>

static void *pf_create (void *core)
{
    assert (0);
    return NULL;
}

static void pf_destroy (void *core, void *pf)
{
    assert (0);
}

static int pf_subscribe (void *core, void *pf, void *subscriber,
    const unsigned char *data, size_t size)
{
    assert (0);
    return -1;
}

static int pf_unsubscribe (void *core, void *pf, void *subscriber,
    const unsigned char *data, size_t size)
{
    assert (0);
    return -1;
}

static void pf_unsubscribe_all (void *core, void *pf, void *subscriber)
{
    assert (0);
}

static void pf_match (void *core, void *pf,
    const unsigned char *data, size_t size)
{
    assert (0);
}

static void *sf_create (void *core)
{
    assert (0);
    return NULL;
}

static void sf_destroy (void *core, void *sf)
{
    assert (0);
}

static int sf_subscribe (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    assert (0);
    return -1;
}

static int sf_unsubscribe (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    assert (0);
    return -1;
}

static void sf_enumerate (void *core, void *sf)
{
    assert (0);
}

static int sf_match (void *core, void *sf,
    const unsigned char *data, size_t size)
{
    assert (0);
    return -1;
}

xs_filter_t filter =  {
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

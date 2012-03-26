AUTOMAKE_OPTIONS = subdir-objects
ACLOCAL_AMFLAGS = ${ACLOCAL_FLAGS}
lib_LTLIBRARIES = libfskeleton.la

libfskeleton_la_SOURCES = \
    include/fskeleton.h \
    src/fskeleton.c

libfskeleton_la_LDFLAGS = -version-info $(FSKELETON_VERSION)

dist_noinst_SCRIPTS = autogen.sh

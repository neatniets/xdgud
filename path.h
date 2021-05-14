#ifndef PATH_H
#define PATH_H

#include <sys/types.h>

/** Character used for separating path hierarchies. */
#define PATHSEP '/'

/** Get the basename of a path without modifying the original string.
 * POSIX hates him for this one weird trick.
 * The return will point to the null byte in the event that @p path ends with
 * a path sep.
 * @return ptr to start of basename, or NULL on error. */
char *
get_basename(
	const char *path //!< filepath
);

/** Get the XDG user config directory, defaulting as necessary.
 * An already-allocated string may be passed into this function, which will be
 * resized as necessary.
 * If the values of @p confdirp and @p szp are NULL and 0, respectively, then a
 * string will be allocated for the caller.
 * Value pointed to by @p confdirp should be freed only on successful return.
 * Behavior is undefined if the path's length exceeds SSIZE_MAX.
 * @return len of path on success, <0 on error. */
ssize_t
get_config_home(
	char **confdirp, //!< ptr to abs path of config dir
	size_t *szp //!< ptr to characters allocated for path
);

#endif

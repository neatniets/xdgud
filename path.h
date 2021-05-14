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
 * Value pointed to by @p confdirp should be freed only on successful return.
 * The allocated size of @p confdirp will be one character larger than the
 * length.
 * @return 0 on success, <0 on error. */
int
get_config_home(
	char **confdirp, //!< ptr to abs path of config dir
	size_t *lenp //!< ptr to length of string
);


#endif

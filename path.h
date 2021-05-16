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

/** Append a string to a path.
 * Extra path separators between the two strings will be removed.
 * @return length of path on success, -1 on error. */
ssize_t
path_append(
	char **pathp, //!< ptr to path
	size_t *szp, //!< ptr to num chars alloc for path
	const char *src //!< string to append
);

/** Replace paths referring to the current dir with their absolute path.
 * Any path that is exactly "." or starts with "./" is replaced with the
 * absolute path to the current working directory.
 * The replaced paths are malloc'd, but there's no convenient way to return the
 * information of which paths are altered.
 * @return 0 on success; <0 on error. */
int
make_cwd_abs(
	char **pathv, //!< array of paths
	size_t npaths //!< num of paths
);

#endif

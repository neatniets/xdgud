#ifndef PATH_H
#define PATH_H

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

#endif

#ifndef XDG_H
#define XDG_H

#include <stdio.h>

/** Get the absolute path to an XDG userdir using the userdir file.
 * If the directory isn't found, NULL is returned.
 * A non-NULL return must be freed by a call to free().
 * @return abs dirpath, or NULL on error. */
char *
lookup_userdir(
	const char *dirname //!< userdir name e.g. DESKTOP
);

#endif

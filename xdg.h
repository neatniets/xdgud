#ifndef XDG_H
#define XDG_H

#include <stdio.h>

/** Get the absolute path to the userdirs.dirs file.
 * If the file isn't found, NULL is returned.
 * A non-NULL return must be freed by a call to free().
 * @return abs filepath, or NULL on error. */
char *
get_userdir_fpath(void);
/** Get a read-only file ptr to the user-dirs.dirs file.
 * If the file isn't found, NULL is returned.
 * A non-NULL return must be closed by a call to fclose().
 * @return file ptr, or NULL on error. */
FILE *
get_userdir_file(void);

/** Get the absolute path to an XDG userdir using the userdir file.
 * If the directory isn't found, NULL is returned.
 * A non-NULL return must be freed by a call to free().
 * @return abs dirpath, or NULL on error. */
char *
lookup_userdir(
	const char *dirname, //!< userdir name e.g. DESKTOP
	FILE *udfp //!< file ptr to userdir file
);

#endif

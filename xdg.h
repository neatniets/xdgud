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

/** Contains information pertaining to an XDG userdir. */
struct userdir {
	char *dirname; //!< XDG dirname
	char *path; //!< abs path to dir
};
/** Initialize variables for use as a set of userdirs. */
void
userdirs_init(
	struct userdir **udvp, //!< becomes ptr to userdir array
	size_t *ndirsp //!< becomes ptr to num of userdirs
);
/** Parse the userdirs file into a sorted set of structs.
 * The array is sorted on the dirname.
 * Array & nmemb must be initialized by userdirs_init().
 * This func can be called again with an array & nmemb from a previous parse to
 * re-parse & reallocate memory.
 * The array should be freed after use by a call to userdirs_free().
 * @return 0 on success; non-zero on error. */
int
userdirs_parse(
	struct userdir **udvp, //!< ptr to sorted userdir array
	size_t *ndirsp, //!< ptr to num of userdirs
	FILE *udfp //!< file ptr to userdirs file
);
/** Free an array of userdir structs.
 * The variables must be re-initialized if they are to be used again. */
void
userdirs_free(
	struct userdir *udv, //!< sorted userdir array
	size_t ndirs //!< num of userdirs
);

/** Get the absolute path to an XDG userdir using the userdir file.
 * If the directory isn't found, NULL is returned.
 * A non-NULL return must be freed by a call to free().
 * @return abs dirpath, or NULL on error. */
char *
flookup_userdir(
	const char *dirname, //!< userdir name e.g. DESKTOP
	FILE *udfp //!< file ptr to userdir file
);
/** Get the absolute path to an XDG userdir using a set of userdirs.
 * If the directory isn't found, NULL is returned.
 * A non-NULL return must be freed by a call to free().
 * @see userdirs_parse()
 * @return abs dirpath, or NULL on error. */
char *
vlookup_userdir(
	const char *dirname, //!< userdir name e.g. DESKTOP
	const struct userdir *udv, //!< sorted userdir array
	size_t ndirs //!< num of userdirs
);

#endif

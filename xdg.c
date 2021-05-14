#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"
#include "printerr.h"
#include "xdg.h"

char *
get_userdir_fpath(void) {
	/* get config dir; the XDG implementations always use the first config
	 * dir found and don't try other directories if the dir exists and the
	 * file doesn't. this behavior is followed here, using the first found
	 * config dir. */
	char *fpath;
	size_t fplen;
	if (get_config_home(&fpath, &fplen) < 0) {
		printerr("could not find config dir\n");
		return NULL;
	}

	/* filename */
	const char fname[] = "/user-dirs.dirs";
	const size_t fnlen = (sizeof(fname) / sizeof(*fname)) - 1;

	/* construct filename to append to dir */
	const char *app = fname; // str to append
	short is_pathsep_needed = (fpath[fplen - 1] == '/');
	app += is_pathsep_needed; // don't append extra '/'
	const size_t alen = fnlen - is_pathsep_needed;

	/* append the filename */
	fpath = realloc(fpath, (fplen + alen + 1) * sizeof(*fpath));
	memcpy(fpath + fplen, app, alen * sizeof(*app));
	fplen += alen;
	fpath[fplen] = '\0';

	return fpath; // file may not be accessible or exist
}
FILE *
get_userdir_file(void) {
	/* get the path to the file */
	char *fpath = get_userdir_fpath();
	if (fpath == NULL) {
		printerr("did not find userdir file\n");
		return NULL;
	}
	/* open the file */
	FILE *fp = fopen(fpath, "r");
	if (fp == NULL) {
		printerr("failed to open file at '%s':", fpath);
	}
	/* cleanup */
	free(fpath);
	return fp;
}

void
userdirs_init(
	struct userdir **udvp,
	size_t *ndirsp
);
int
userdirs_parse(
	struct userdir **udvp,
	size_t *ndirsp,
	FILE *udfp
);
void
userdirs_free(
	struct userdir *udv,
	size_t ndirs
);

char *
flookup_userdir(
	const char *dirname,
	FILE *udfp
);
char *
vlookup_userdir(
	const char *dirname,
	const struct userdir *udv,
	size_t ndirs
);

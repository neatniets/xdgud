#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "printerr.h"
#include "xdg.h"

/** Determine if an environment variable is unset/empty.
 * @return 0 if variable is unset/empty; 1 otherwise. */
static short
is_set(
	const char *env_var //!< string returned from getenv()
);

/** Get the XDG user config directory, defaulting as necessary.
 * Value pointed to by @p confdirp should be freed only on successful return.
 * The allocated size of @p confdirp will be one character larger than the
 * length.
 * @return 0 on success, <0 on error. */
static int
get_config_home(
	char **confdirp, //!< ptr to abs path of config dir
	size_t *lenp //!< ptr to length of string
);

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

static short
is_set(
	const char *env_var
) {
	return !((env_var == NULL) || (*env_var == '\0'));
}

static int
get_config_home(
	char **confdirp,
	size_t *lenp
) {
	const char *dirpre; // prefix of abs path
	const char *dirsuf; // possibly empty suffix of abs path

	dirpre = getenv("XDG_CONFIG_HOME");
	if (is_set(dirpre)) {
		dirsuf = "";
	} else { // use default
		dirpre = getenv("HOME");
		if (!is_set(dirpre)) {
			printerr("HOME not set\n");
			return -1;
		}
		dirsuf = "/.config";
	}

	/* alloc space */
	const size_t dplen = strlen(dirpre);
	const size_t dslen = strlen(dirsuf);
	*lenp = dplen + dslen;
	*confdirp = malloc((*lenp + 1) * sizeof(**confdirp));
	if (*confdirp == NULL) {
		printerr("malloc() fail:");
		return -1;
	}
	/* copy components */
	memcpy(*confdirp, dirpre, dplen * sizeof(*dirpre));
	memcpy(*confdirp + dplen, dirsuf, dslen * sizeof(*dirsuf));
	(*confdirp)[*lenp] = '\0';

	return 0;
}

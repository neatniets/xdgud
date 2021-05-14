#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"
#include "printerr.h"

/** Determine if an environment variable is unset/empty.
 * @return 0 if variable is unset/empty; 1 otherwise. */
static short
is_set(
	const char *env_var //!< string returned from getenv()
);

char *
get_basename(
	const char *path
) {
	if (path == NULL) {
		printerr("NULL ptr supplied to get_basename()\n");
		return NULL;
	}

	const size_t len = strlen(path);
	const char *base = path + len; // points to '\0'
	short is_base_found = 0;
	/* repeat until base found or start of path reached */
	while ((base != path) && (!is_base_found)) {
		if (base[-1] == PATHSEP) {
			is_base_found = 1;
		} else {
			base--;
		}
	}
	return (char *)base;
}

int
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

static short
is_set(
	const char *env_var
) {
	return !((env_var == NULL) || (*env_var == '\0'));
}

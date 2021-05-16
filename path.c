#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "path.h"
#include "printerr.h"

/** Determine if an environment variable is unset/empty.
 * @return 0 if variable is unset/empty; 1 otherwise. */
static short
is_set(
	const char *env_var //!< string returned from getenv()
);
/** Allocate a string containing the current working directory.
 * Return must be freed on successful return.
 * @return string of cwd; NULL on error. */
static char *
alloc_cwd(void);

/** [Re-]allocate a string to accommodate the specified num of chars.
 * @p req_sz is the minimum num of chars, but the actual allocation may be
 * larger.
 * If the values of @p strp and @p szp are NULL and 0, respectively, then a
 * string will be allocated for the caller.
 * The value pointed to by @p strp must be freed on a successful return.
 * If the string is already large enough for the required size, nothing changes
 * and 0 is returned.
 * @return 0 on success, <0 on error. */
static int
alloc_str(
	size_t req_sz, //!< num chars needed
	char **strp, //!< ptr to string
	size_t *szp //!< ptr to num chars alloc'd for string
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

ssize_t
get_config_home(
	char **confdirp,
	size_t *szp
) {
	if ((confdirp == NULL) || (szp == NULL)) {
		printerr("NULL passed to get_config_home()\n");
		return -1;
	}

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
	ssize_t len = dplen + dslen;
	if (alloc_str(len + 1, confdirp, szp) < 0) {
		printerr("could not allocate string for path\n");
		return -1;
	}
	/* copy components */
	memcpy(*confdirp, dirpre, dplen * sizeof(*dirpre));
	memcpy(*confdirp + dplen, dirsuf, dslen * sizeof(*dirsuf));
	(*confdirp)[len] = '\0';

	return len;
}

ssize_t
path_append(
	char **pathp,
	size_t *szp,
	const char *src
) {
	if ((pathp == NULL) || (szp == NULL) || (src == NULL)) {
		printerr("NULL passed to path_append()\n");
		return -1;
	}
	if (*pathp == NULL) {
		printerr("path ptr points to NULL\n");
		return -1;
	}

	/* computing lengths & removing extra separators */
	ssize_t plen = strlen(*pathp);
	plen -= ((*pathp)[plen - 1] == PATHSEP); // rm trailing sep
	src += (src[0] == PATHSEP); // rm leading sep
	const size_t slen = strlen(src);

	/* make space */
	if (alloc_str(plen + slen + 2, pathp, szp) < 0) {
		printerr("could not alloc string for path\n");
		return -1;
	}
	/* append */
	(*pathp)[plen] = PATHSEP;
	plen++;
	memcpy(*pathp + plen, src, slen * sizeof(*src));
	plen += slen;
	(*pathp)[plen] = '\0';

	return plen;
}

int
make_cwd_abs(
	char **pathv,
	size_t npaths
) {
	char *cwd = alloc_cwd();
	if (cwd == NULL) {
		printerr("couldn't obtain cwd\n");
		return -1;
	}
	const size_t cwdlen = strlen(cwd);

	short is_cwd_used = 0; // flag to indicate if cwd should be freed
	const char prefix[] = "./";
	const size_t plen = (sizeof(prefix) / sizeof(*prefix)) - 1;
	for (size_t i = 0; i < npaths; i++) {
		if (strcmp(pathv[i], ".") == 0) {
			pathv[i] = cwd;
			is_cwd_used = 1; // ptr to cwd; don't free it
		} else if (strncmp(pathv[i], prefix, plen) == 0) {
			/* clone cwd */
			char *str = NULL;
			size_t ssz = 0;
			if (alloc_str(cwdlen + 1, &str, &ssz) < 0) {
				printerr("couldn't alloc str for new path\n");
				return -1;
			}
			memcpy(str, cwd, (cwdlen + 1) * sizeof(*cwd));
			/* append rest of path */
			if (path_append(&str, &ssz, pathv[i] + plen) < 0) {
				printerr("couldn't append path\n");
				free(str);
				return -1;
			}
			pathv[i] = str;
		}
	}

	if (!is_cwd_used) {
		free(cwd);
	}
	return 0;
}

static short
is_set(
	const char *env_var
) {
	return !((env_var == NULL) || (*env_var == '\0'));
}
static char *
alloc_cwd(void) {
	char *cwd = NULL;
	size_t cwdsz = 0;
	size_t try_sz = 1;

	errno = ERANGE; // preset errno
	do {
		if (errno != ERANGE) { // error was caused by something else
			printerr("getcwd() error:");
			free(cwd);
			return NULL;
		}
		/* alloc bigger str */
		try_sz <<= 1;
		if (alloc_str(try_sz, &cwd, &cwdsz) < 0) {
			printerr("couldn't alloc str for cwd\n");
			free(cwd); // safe is cwd is NULL
			return NULL;
		}
	} while (getcwd(cwd, cwdsz) == NULL); // until getcwd() works

	return cwd;
}

static int
alloc_str(
	size_t req_sz,
	char **strp,
	size_t *szp
) {
	if ((strp == NULL) || (szp == NULL)) {
		printerr("NULL passed to alloc_str()\n");
		return -1;
	}
	if (*szp >= req_sz) { // requirement already met
		return 0;
	}

	/* set size to default val if str is NULL */
	short is_str_null = (*strp == NULL);
	*szp = (*szp * !is_str_null) + is_str_null; // set to 1 if str is NULL

	/* increase size until higher than required size */
	do {
		*szp <<= 1;
	} while (*szp < req_sz);
	/* [re-]alloc space */
	*strp = realloc(*strp, *szp * sizeof(**strp)); // won't fail on NULL
	if (*strp == NULL) {
		printerr("realloc() failed:");
		return -1;
	}

	return 0;
}

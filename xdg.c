#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"
#include "printerr.h"
#include "xdg.h"

/** Get the absolute path to the userdirs.dirs file.
 * If the file isn't found, NULL is returned.
 * A non-NULL return must be freed by a call to free().
 * @return abs filepath, or NULL on error. */
static char *
get_userdir_fpath(void);
/** Get a read-only file ptr to the user-dirs.dirs file.
 * If the file isn't found, NULL is returned.
 * A non-NULL return must be closed by a call to fclose().
 * @return file ptr, or NULL on error. */
static FILE *
get_userdir_file(void);

/** Remove a newline from the end of the string, if there is one.
 * @return new length of string. */
static size_t
rmnl(
	char *str //!< string
);
/** Skip leading whitespace in a string.
 * @return ptr to first non-ws char. */
static char *
skipws(
	const char *str //!< string to parse
);
/** Parse a string in an attempt to match the specificed dirname.
 * It is an error if the dirname does not match.
 * @return num of chars parsed on success, <0 on error. */
static ssize_t
parse_dirname(
	const char *str, //!< string to parse
	const char *dirname //!< xdg dirname
);
/** Parse a full line for an XDG userdir spec.
 * The return will be the start of the first \", as required in
 * user-dirs.dirs(5).
 * @return start of path def on success; NULL on error. */
static char *
parse_xdg(
	const char *str, //!< string to parse
	const char *dirname //!< xdg dirname
);

/** Interpret the path definition found in the userdir file.
 * Expansion may need to be done and quotes need to be removed.
 * A successful return will need to be freed.
 * @return abs path to userdir on success; NULL on error. */
static char *
interpret_udpath(
	const char *udpath //!< userdir path def
);

char *
lookup_userdir(
	const char *dirname
) {
	FILE *udfp = get_userdir_file();
	if (udfp == NULL) {
		printerr("could not obtain userdirs.dirs file\n");
		return NULL;
	}
	fseek(udfp, 0L, SEEK_SET); // move to start of file
	char *buf = NULL;
	size_t balloc = 0;

	/* look for directory */
	while (1) {
		/* get line from file */
		ssize_t blen = getline(&buf, &balloc, udfp);
		if (blen < 0) { // EOF or error
			if (!feof(udfp)) {
				printerr("getline() error:");
			}
			free(buf);
			fclose(udfp);
			return NULL;
		}
		blen = rmnl(buf);

		/* try to parse for xdg */
		const char *line = parse_xdg(buf, dirname);
		if (line == NULL) {
			continue;
		}
		/* must translate line into abs path */
		char *path = interpret_udpath(line);
		if (path == NULL) {
			printerr("could not make a path out of '%s'\n", line);
		}

		/* cleanup */
		fclose(udfp);
		free(buf);
		return path;
	}
}

static char *
get_userdir_fpath(void) {
	/* get config dir; the XDG implementations always use the first config
	 * dir found, and don't try other directories if the dir exists and the
	 * file doesn't. this behavior is followed here, using the first found
	 * config dir. */
	char *fpath = NULL;
	size_t fpsz = 0;
	ssize_t fplen = get_config_home(&fpath, &fpsz);
	if (fplen < 0) {
		printerr("could not find config dir\n");
		return NULL;
	}

	/* append filename */
	const char fname[] = "user-dirs.dirs";
	fplen = path_append(&fpath, &fpsz, fname);
	if (fplen < 0) {
		printerr("could not append userdir filename\n");
		return NULL;
	}

	return fpath; // file may not be accessible or exist
}
static FILE *
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


static size_t
rmnl(
	char *str
) {
	size_t len = strlen(str);
	short has_newline = (str[len - 1] == '\n');
	/* only rm newline if it exists */
	str[len - 1] = (!has_newline * str[len - 1]) + (has_newline * '\0');
	len -= has_newline;
	return len;
}
static char *
skipws(
	const char *str
) {
	while (isspace(*str)) {
		str++;
	}
	return (char *)str;
}
static ssize_t
parse_dirname(
	const char *str,
	const char *dirname
) {
	const char prefix[] = "XDG_";
	const char suffix[] = "_DIR";
	const size_t plen = (sizeof(prefix) / sizeof(*prefix)) - 1;
	const size_t slen = (sizeof(suffix) / sizeof(*suffix)) - 1;
	const size_t dlen = strlen(dirname);

	/* parsing */
	if (strncmp(str, prefix, plen) != 0) {
		return -1;
	}
	str += plen;
	if (strncmp(str, dirname, dlen) != 0) {
		return -1;
	}
	str += dlen;
	if (strncmp(str, suffix, slen) != 0) {
		return -1;
	}
	return plen + dlen + slen;
}
static char *
parse_xdg(
	const char *str,
	const char *dirname
) {
		const char *line = skipws(str);
		ssize_t parsed = parse_dirname(line, dirname);
		if (parsed < 0) {
			return NULL;
		}
		line += parsed;
		line = skipws(line);
		if (*line != '=') {
			return NULL;
		}
		line += 1;
		line = skipws(line);
		return (char *)line;
}

static char *
interpret_udpath(
	const char *udpath
) {
	/* according to user-dirs.dirs(5), the path must be in the form of
	 * "$HOME/path" or "/path", quotes included.
	 * dumb? probably. but this function will still follow the
	 * requirements. */

	udpath++; // skip '"'
	const char *prefix; // possibly empty path prefix
	const char *suffix; // path suffix

	/* determine if the home expansion is needed */
	const char henv[] = "$HOME";
	const size_t hlen = (sizeof(henv) / sizeof(*henv)) - 1;
	if (strncmp(udpath, henv, hlen) == 0) { // home at start
		prefix = getenv("HOME");
		suffix = udpath + hlen;
	} else {
		prefix = "";
		suffix = udpath;
	}

	const size_t plen = strlen(prefix);
	const size_t slen = strlen(suffix) - 1; // -1 to rm '"'
	/* alloc space */
	const size_t len = plen + slen;
	char *path = malloc((len + 1) * sizeof(*path));
	if (path == NULL) {
		printerr("malloc() error:");
		return NULL;
	}
	/* copy contents */
	memcpy(path, prefix, plen * sizeof(*prefix));
	memcpy(path + plen, suffix, slen * sizeof(*suffix));
	path[len] = '\0';
	return path;
}

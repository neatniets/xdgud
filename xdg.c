#define _XOPEN_SOURCE 700

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path.h"
#include "printerr.h"
#include "xdg.h"

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

char *
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

char *
lookup_userdir(
	const char *dirname,
	FILE *udfp
) {
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
			return NULL;
		}
		if (buf[blen - 1] == '\n') { // remove newline
			buf[blen - 1] = '\0';
			blen--;
		}

		/* parsing line */
		const char *line = skipws(buf);
		ssize_t parsed = parse_dirname(line, dirname);
		if (parsed < 0) {
			continue;
		}
		line += parsed;
		line = skipws(line);
		if (*line != '=') {
			continue;
		}
		line += 1;
		line = skipws(line);
		/* remainder of line is the userdir path */
		blen = strlen(line);
		memmove(buf, line, blen * sizeof(*line));
		buf[blen] = '\0';
		return buf;
	}
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

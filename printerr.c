#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "printerr.h"

/** Print the error msg to a buffer and then write to stderr.
 * Keeps the message as a single write and prevents other process messages from
 * being interwoven with this one.
 * @p ap is not modified on failure; no copy needs to be made.
 * @return 0 on success, <0 on failure. */
static int
vprinterr_buf(
	short is_errno_wanted, //!< flag if errno msg is to be used
	const char *restrict fmt, //!< printf format
	va_list ap //!< var arg ptr
);
/** Print the error msg directly to stderr.
 * Msg may be written in parts, which could result in other process msgs being
 * interwoven with this one.
 * @return 0 on success, <0 on failure. */
static int
vprinterr_sep(
	short is_errno_wanted, //!< flag if errno msg is to be used
	const char *restrict fmt, //!< printf format
	va_list ap //!< var arg ptr
);

void
printerr(
	const char *restrict fmt,
	...
) {
	const int errcpy = errno; // copy of errno in case of overwrite
	const size_t flen = strlen(fmt);
	/* check if an errno msg is desired */
	const short is_errno_wanted = (fmt[flen - 1] == ':');

	/* get va list */
	va_list ap;
	va_start(ap, fmt);

	/* try to malloc a buffer to print the string to before writing;
	 * if that fails, simply write the error message in separate parts. */
	errno = errcpy; // set this to the proper errno
	if (vprinterr_buf(is_errno_wanted, fmt, ap) < 0) {
		/* buf version failed if here */
		errno = errcpy;
		vprinterr_sep(is_errno_wanted, fmt, ap);
	}

	/* cleanup */
	va_end(ap);
}

static int
vprinterr_buf(
	short is_errno_wanted,
	const char *restrict fmt,
	va_list ap
) {
	/* get the length of the final formatted msg */
	va_list ap_cpy;
	va_copy(ap_cpy, ap);
	size_t blen = vsnprintf(NULL, 0, fmt, ap_cpy); // gets len w/o print
	va_end(ap_cpy);

	/* malloc buffer */
	blen -= (is_errno_wanted && 1); // ':' at end will be removed if true
	char *buf = malloc((blen + 1) * sizeof(*buf));
	if (buf == NULL) {
		return -1;
	}
	/* print to buffer */
	vsnprintf(buf, blen + 1, fmt, ap);
	/* write out buffer */
	if (is_errno_wanted) {
		perror(buf);
	} else {
		fputs(buf, stderr);
	}
	free(buf);

	return 0;
}

static int
vprinterr_sep(
	short is_errno_wanted,
	const char *restrict fmt,
	va_list ap
) {
	vfprintf(stderr, fmt, ap);
	if (is_errno_wanted) {
		fputc(' ', stderr); // separate the ':' from the errno msg
		perror(NULL);
	}
	return 0;
}

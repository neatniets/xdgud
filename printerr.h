#ifndef PRINTERR_H
#define PRINTERR_H

/** Enable or disable 'quiet' mode.
 * Enabling quiet mode will stop all printerr() messages from being printed. */
void
quiet(
	short value //!< boolean
);
/** Print a formatted msg to stderr, optionally including errno msg.
 * The errno msg is included only if the msg ends with a ':', in which case it
 * is printed after the ':' with a space in between.
 * If the errno msg is included, a newline is written after it; otherwise, a
 * newline is not printed by default. */
void
printerr(
	const char *restrict fmt, //!< printf format
	... //!< printf args
);

#endif

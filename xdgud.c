#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "path.h"
#include "printerr.h"
#include "xdg.h"

#define PRG_NAME "xdgud"
#define XDG_NAME "xdg-user-dir"

/** Print usage message. */
static void
print_help(void);

/** Print the args using printerr. */
static void
printerr_args(
	int argc, //!< num args
	char *argv[] //!< arg array
);

int
main(
	int argc,
	char *argv[]
) {
	/* check if prog called by XDG_NAME */
	const char *base = get_basename(*argv);
	short is_xdg_user_dir = (strcmp(base, XDG_NAME) == 0);

	/* parse options */
	int optc;
	if (!is_xdg_user_dir) {
		while (optc = getopt(argc, argv, "hq"), optc != -1) {
			switch (optc) {
			case '?':
				/* unknown option; getopt() printed error */
				return 1;
			case 'h': // help
				print_help();
				return 0;
			case 'q': // quiet
				quiet(1);
				break;
			}
		}
	} else {
		quiet(1); // xdg-user-dir is always quiet
	}
	/* set args to point to remaining args */
	argc -= optind;
	argv += optind;
	/* exit successfully if nothing is to happen */
	if (argc <= 0) {
		return 0;
	}

	/* get the dir */
	char *dir = lookup_userdir(*argv);
	if (dir == NULL) {
		printerr("did not find a userdir for '%s'\n", *argv);
		return 1;
	}
	argv++;
	argc--;

	if (is_xdg_user_dir) {
		puts(dir); // just print the dir and exit
	} else {
		if (make_cwd_abs(argv, argc) < 0) {
			printerr("failed to replace cwd paths\n");
			return 1;
		}
		if (argc == 0) { // do nothing if no commands
			return 0;
		}
		/* change to new dir */
		if (chdir(dir) < 0) {
			printerr("chdir(%s) error:", dir);
			return 1;
		}
		/* execute command */
		execvp(argv[0], argv);
		printerr("execvp() error:");
		printerr_args(argc, argv);
		free(dir);
		return 1;
	}

	free(dir);
	return 0;
}

static void
print_help(void) {
	puts("usage:\n"
		"\txdg-user-dir <dir-name>\n\n"
		"\t" PRG_NAME " [<option> ...] -h\n"
		"\t" PRG_NAME " [<option> ...] <dir-name> <commands> ...\n\n"
		"\t<option> := -q");
}

static void
printerr_args(
	int argc,
	char *argv[]
) {
	/* not really possible to print these all together */
	for (int i = 0; i < argc; i++) {
		printerr("%s ", argv[i]);
	}
	printerr("\n");
}

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
		while (optc = getopt(argc, argv, "hv"), optc != -1) {
			switch (optc) {
			case '?':
				/* unknown option; getopt() printed error */
				return 1;
			case 'h': // help
				print_help();
				return 0;
			case 'v': // verbose
				/* do nothing for now */
				puts("verbosity set; does nothing atm");
				break;
			}
		}
	}
	/* set args to point to remaining args */
	argc -= optind;
	argv += optind;
	/* exit successfully if nothing is to happen */
	if (argc <= 0) {
		return 0;
	}

	char *dir = lookup_userdir(*argv);
	if (dir == NULL) {
		printerr("did not find a userdir for '%s'\n", *argv);
		return 1;
	}
	puts(dir);
	free(dir);
	argv++;
	argc--;

	/* print the rest of the args for now */
	for (int i = 0; i < argc; i++) {
		puts(argv[i]);
	}

	return 0;
}

static void
print_help(void) {
	puts("usage:\n"
		"\txdg-user-dir <dir-name>\n\n"
		"\t" PRG_NAME " [<option> ...] -h\n"
		"\t" PRG_NAME " [<option> ...] <dir-name> <commands> ...\n\n"
		"\t<option> := -v");
}

#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include "path.h"

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
	short is_xdg_user_dir = 0;
	const char *base = get_basename(*argv);
	if (strcmp(base, XDG_NAME) == 0) {
		is_xdg_user_dir = 1;
	}

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

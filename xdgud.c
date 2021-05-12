#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <unistd.h>

#define PRG_NAME "xdgud"

static void
print_help(void);

int
main(
	int argc,
	char *argv[]
) {
	/* parse options */
	int optc;
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

#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "path.h"
#include "printerr.h"

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
		if (base[-1] == '/') {
			is_base_found = 1;
		} else {
			base--;
		}
	}
	return (char *)base;
}

#include "stdio.h"
main() {
	char *name, *logname();
	name = logname();
	if (name == NULL)
		exit (1);
	printf ("%s\n",name);
	exit (0);
}

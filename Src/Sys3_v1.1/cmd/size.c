#include	<stdio.h>
#include 	<a.out.h>

/*
	size -- determine object size

*/

main(argc, argv)
char **argv;
{
	struct exec buf;
	long sum;
	int gorp,ck;
	FILE *f;

	if (argc==1) {
		*argv = "a.out";
		argc++;
		--argv;
	}
	gorp = argc;
	while(--argc) {
		++argv;
		if ((f = fopen(*argv, "r"))==NULL) {
			fprintf(stderr,"size: %s not found\n", *argv);
			continue;
		}
		ck = fread((char *)&buf, sizeof(buf), 1, f);
		if(BADMAG(buf) || (ck == 0)) {
			fprintf(stderr,"size: %s not an object file\n", *argv);
			fclose(f);
			continue;
		}
		if (gorp>2)
			printf("%s: ", *argv);
		printf("%u+%u+%u = ", buf.a_text,buf.a_data,buf.a_bss);
		sum = (long) buf.a_text + (long) buf.a_data + (long) buf.a_bss;
		printf("%ldb = 0%lob\n", sum, sum);
		fclose(f);
	}
}

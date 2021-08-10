#include "gasgen.h"

BOOL	verbose;

static void
usage(void)
{
	fprintf(stdout,
"Usage: gasgen <options> <config path>\n"
" <options>\n"
"      -h: this message\n"
"      -v: verbose mode\n"
	);
}

void
errmsg(const char *fmt, ...)
{
	va_list	ap;
	char	*errmsg;

	va_start(ap, fmt);
	vasprintf(&errmsg, fmt, ap);
	va_end(ap);

	fprintf(stderr, "ERROR: %s\n", errmsg);
	free(errmsg);
}

static void
parse_args(int argc, char *argv[])
{
	int	c;

	while ((c = getopt(argc, argv, "s:h")) != -1) { //command-line option
		switch (c) {
		case 'h':
			usage();
			exit(0);
		default:
			errmsg("invalid option");
			usage();
			exit(1);
		}
	}

	if (argc - optind < 1) {
		usage();
		exit(1);
	}

	load_conf(argv[optind]);
}

int
main(int argc, char *argv[]) //# ./gasgen gastask.conf
{
	parse_args(argc, argv); //gastask.conf 파일 parsing - mem, gentask
	srand(getpid() + time(NULL)); //실행할 때마다 다른 난수 생성

	gen_task();

	return 0;
}

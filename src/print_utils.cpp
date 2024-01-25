#include <errno.h>
#include <cstdio>

void	print_error(const char *str)
{
	fprintf(stderr, "%s\n", str);
}
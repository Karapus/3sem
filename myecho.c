#include <stdio.h>
#include <string.h>
int print_vec(int, const char **);

int main(int argc, const char **argv)
{
	if (argc > 1 && !strcmp(argv[1], "-n"))
		print_vec(argc - 2, argv + 2);
	else
	{
		print_vec(argc - 1, argv + 1);
		putchar('\n');
	}
	return 0;
}

int print_vec(int n, const char **vec)
{
	if (n)
	{
		for (int i = 0; i < n - 1; i++)
			printf("%s ", vec[i]);
		printf("%s", vec[n - 1]);
	}
	return 0;
}

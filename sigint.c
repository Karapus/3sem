#include <signal.h>
#include <stdio.h>

void print_hello(int sig) {
	if (sig == SIGINT)
		puts("Hello world");
}

int main() {
	signal(SIGINT, print_hello);
	char q = '\0';
	while (q != 'q')
		q = getchar();
}

#include <stdio.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>

void border(int sig) {
	printf("\e[H\e[J");
	struct winsize size;
	ioctl(1, TIOCGWINSZ, &size);
	for (int j = 0; j < size.ws_col; j++)
		putchar('*');
	for (int i = 1; i < size.ws_row - 2; i++) {
		putchar('*');
		for (int j = 1; j < size.ws_col - 1; j++)
			putchar(' ');
		putchar('*');
	}
	for (int j = 0; j < size.ws_col - 1; j++)
		putchar('*');
}

int main() {
	setvbuf(stdout, NULL, _IONBF, 0);
	border(1);
	signal(SIGWINCH, border);
	while (1)
		pause();
}

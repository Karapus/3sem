#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

int reader_pid, writer_pid;
int fd;
char **f_names;
int f_n;

void reader(int sig) {
	static int c = 0;
	static char i = 0;
	if (!(i &= 0x7)) {
		int sz;
		while (!(sz = read(fd, &c, 1))) {
			close(fd);
			if (!f_n--) {
				kill(writer_pid, SIGTERM);
				return;
			}
			fd = open(*(f_names++), S_IRUSR);
		}
		if (sz < 0) {
			perror("read error: ");
			exit(errno);
		}	
	}
	++i;
	kill(writer_pid, (c & 1) ? SIGUSR2 : SIGUSR1);
	c >>= 1;
}

void writer(int sig) {
	static char c = 0;
	static char i = 0;
	c += ((sig == SIGUSR2) ? 1 : 0) << i;
	++i;
	if (!(i &= 0x7)) {
	       putchar(c);
	       c = 0;
	}
	kill(reader_pid, SIGUSR1);
}

int main(int argc, char **argv) {
	setbuf(stdout, NULL);
	reader_pid = getpid();
	writer_pid = fork();
	if (writer_pid < 0) {
		perror("fork error: ");
		return errno;
	}
	struct sigaction sa;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (!writer_pid) {
		sa.sa_handler = writer;
		sigaction(SIGUSR1, &sa, NULL);
		sigaction(SIGUSR2, &sa, NULL);
		kill(reader_pid, SIGUSR1);
		while (pause())
			;
	}
	if (argc > 1) {
		fd = open(argv[1], S_IRUSR);
		f_names = &argv[2];
		f_n = argc - 2;
	}
	else
		fd = STDIN_FILENO;
	sa.sa_handler = reader;
	sigaction(SIGUSR1, &sa, NULL);
	wait(NULL);
}

#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
	int fd[2];
	if (pipe(fd) < 0) {
		perror("Pipe error: ");
		return errno;
	}

	int pid = fork();
	if (pid < 0) {
		perror("Fork error : ");
		return errno;
	}
	if (!pid) {
		const char *first_arg = "axu";
		const char * const ps_args[] = { first_arg, NULL };
		execvp("ps", ps_args);
	}
	wait(-1);
}

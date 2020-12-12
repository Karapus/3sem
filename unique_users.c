#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

#define BUF_SIZE 1024

ssize_t fdreadline(int fd, char *buf, size_t count) {
        ssize_t read_res;
        char *cur = buf;
        while ((read_res = read(fd, cur, 1))) {
                if (read_res < 0) {
                        perror("Read error:");
                        return cur - buf;
                }
                if (*cur == '\n')
                	return cur - buf;
                cur += read_res;
                count -= read_res;
        }
        return cur - buf;
}

int main(int argc, char **argv) {
	int fds[2];
	if (pipe(fds) < 0) {
		perror("Pipe error: ");
		return errno;
	}

	int pid = fork();
	if (pid < 0) {
		perror("Fork error: ");
		return errno;
	}
	if (!pid) {
		if (dup2(fds[1], STDOUT_FILENO) < 0) {
			perror("Dup2 error in child: ");
			return errno;
		}
		if (close(fds[0]) < 0) {
			perror("Close error in child: ");
			return errno;
		}
		const char *name = "ps";
		const char *first_arg = "axu";
		const char * const ps_args[] = { name, first_arg, NULL };
		execvp("ps", ps_args);
	}
	if (close(fds[1]) < 0) {
		perror("Close error: ");
		return errno;
	}
	wait(-1);
	char *buf[BUF_SIZE + 1];
	char *prev[BUF_SIZE + 1];
	buf[BUF_SIZE] = '\0';
	prev[BUF_SIZE] = '\0';
	while (fdreadline(fds[0], buf, BUF_SIZE)) {
		strtok(buf, " ");
		if (strcmp(buf, prev))
			printf("%s\n", buf);
		strcpy(prev, buf);
	}
}

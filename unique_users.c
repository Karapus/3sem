#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>

#define BUF_SIZE 1024

ssize_t fdread(int fd, void *buf, size_t count) {
        ssize_t read_res;
        void *cur = buf;
        while ((read_res = read(fd, cur, count))) {
                if (read_res < 0) {
                        perror("Read error:");
                        return cur - buf;
                }
                cur += read_res;
                count -= read_res;
        }
        return cur - buf;
}

ssize_t fdwrite(int fd, const void *buf, size_t count) {
        ssize_t write_res;
        const void *cur = buf;
        while ((write_res = write(fd, cur, count))) {
                if (write_res < 0) {
                        perror("Write error:");
                        return cur - buf;
                }
                cur += write_res;
                count -= write_res;
        }
        return cur - buf;
}

void fdecho(int fd_in, int fd_out, void *buf, size_t size) {
        ssize_t res;
        while ((res = fdread(fd_in, buf, size)))
                fdwrite(fd_out, buf, res);
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
	buf[BUF_SIZE] = '\0';
	while (fdread(fds[0], buf, BUF_SIZE)) {
		char *first_word = strtok(buf, " ");
		printf("%s\n", first_word);
		strtok(NULL, "\n");
		for (char *second_word = strtok(NULL, " "); strtok(NULL, "\n"); second_word = strtok(NULL, " ")) {
//			if (!strcmp(first_word, second_word))
			printf("%s\n", first_word);
				printf("%s\n", second_word);
			first_word = second_word;
		}
	}
}

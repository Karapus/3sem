#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <assert.h>
#include <limits.h>
#include <fcntl.h>

#define BUF_SIZE 128
ssize_t fdread(int fd, void *buf, size_t count) {
	assert(fd >= 0);
	assert(buf);
	assert(count < SSIZE_MAX);
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
	assert(fd >= 0);
	assert(buf);
	assert(count < SSIZE_MAX);
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
	int fd = (argc == 2) ? open(argv[1], O_RDONLY) : STDIN_FILENO;
	if (fd < 0) {
		perror("Open error:");
		return errno;
	}
	char buf[BUF_SIZE];
	fdecho(fd, STDOUT_FILENO, buf, BUF_SIZE);
}

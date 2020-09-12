#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>

#define BUF_CAP 256

int main(int argc, char **argv)
{
	for (int i = 1; i < argc; i++)
	{
		int fd = open(argv[i], O_RDONLY);
		void *buf = calloc(BUF_CAP, 1);
		long res = 0;
		size_t r_pos = 0;
		for (; r_pos < BUF_CAP; r_pos += res)
		{
			if ((res = read(fd, buf + r_pos, BUF_CAP - r_pos)) == 0)
				break;
			if (res < 0)
				perror("");
		}
		res = 0;
		for (int w_pos = 0; w_pos < BUF_CAP; w_pos += res)
		{
			if ((res = write(2, buf + w_pos, BUF_CAP - w_pos)) == 0)
				break;
			if (res < 0)
				perror("");
		}
	}
}

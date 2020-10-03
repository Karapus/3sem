#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <getopt.h>

#define B_SIZE 1024

int main (int argc, char* argv[]) {
	
	struct option op_quiet = {"quiet", no_argument, 0, 0};
	int q_option = getopt_long(2, argv, "q", &op_quiet, NULL);

	int fd[2];
	
	int status = pipe(fd);
	if (status < 0) {
		perror("Pipe error:");
		return errno;
	}

	struct timespec start;
        clock_gettime(CLOCK_MONOTONIC, &start);
	
	int pid, wstatus;
	pid = fork();
	
	if(pid == 0) {
		status = close(fd[0]);
		if (status < 0) {
			perror("Close error in child:");
			return errno;
		}

		status = dup2(fd[1], 1);
		if (status < 0) {
			perror("Dup error:");
			return errno;
		}

		status = execvp(argv[(q_option > -1) ? 2 : 1], argv + ((q_option > -1) ? 2 : 1));
		perror("Exec error:");
		return errno;
	}
	else {
		status = close(fd[1]);
		if (status < 0) {
			perror("Close error in parent:");
			return errno;
		}

		char buffer[B_SIZE];
		int cur, bc, wc, lc;
		bc = wc = lc = 0;
		char pc = '\0';
		while ((cur = read(fd[0], buffer, B_SIZE)) > 0) {
			bc += cur;
			for (char *c = buffer; c < buffer + cur; c++){
				if (!isspace(pc) && pc && isspace(*c))
					wc++;
				if (*c == '\n')
					lc++;
				pc = *c;
			}
			if (q_option == -1)
				while (cur -= write(1, buffer, cur))
					;
		}
		waitpid(-1, &wstatus, 0);
		struct timespec finish;
		clock_gettime(CLOCK_MONOTONIC, &finish);

		long int sec = (finish.tv_sec - start.tv_sec) * 1000;
		long int nsec = finish.tv_nsec / 1000000 - start.tv_nsec / 1000000;
		long int msec = sec + nsec;
		fprintf(stderr, "ms %ld\n", msec);
		
		fprintf(stderr, "lc = %d, wc = %d, bc = %d\n", lc, wc, bc);
		
		status = close(fd[0]);
		if (status < 0) {
			perror("Close error in parent:");
			return errno;
		}
	}
} 

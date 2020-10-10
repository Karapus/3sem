#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

int runner(int id, int n, int msq_id) {
	long mtype;
	printf("runner #%d came\n", id);
	mtype = n + 1;
	if (msgsnd(msq_id, &mtype, 0, 0)) {
		perror("msgsnd error:");
		return errno;
	}
	if (msgrcv(msq_id, &mtype, 0, id, 0) < 0) {
		perror("Msgrcv error:");
		return errno;
	}
	printf("runner #%d started\n", id);
	printf("runner #%d finished\n", id);
	mtype = id + 1;
	if (msgsnd(msq_id, &mtype, 0, 0)) {
		perror("Msgsnd error:");
		return errno;
	}
	if (msgrcv(msq_id, &mtype, 0, n + 2, 0) < 0) {
		perror("Msgrcv error:");
		return errno;
	}
	printf("runner #%d leaves\n", id);
	return 0;
}



int judge(int n, int msq_id) {
	long mtype;
	printf("judje came\n");
	for (int i = 1; i <= n; i++)
		if (msgrcv(msq_id, &mtype, 0, n + 1, 0) < 0) {
			perror("Msgrcv error:");
			return errno;
		}
	printf("competition starts\n");
	mtype = 1;
	if (msgsnd(msq_id, &mtype, 0, 0)) {
		perror("msgsnd error:");
		return errno;
	}
	if (msgrcv(msq_id, &mtype, 0, n + 1, 0) < 0) {
		perror("Msgrcv error:");
		return errno;
	}
	printf("judje announces time\n");
	mtype = n + 2;
	for (int i = 1; i <= n; i++) {
		if (msgsnd(msq_id, &mtype, 0, 0) < 0) {
			perror("Msgrcv error:");
			return errno;
		}
	}
	printf("judje leaves\n");
	return 0;
}

int main(int argc, char **argv) {
	setlinebuf(stdout);
	int msq_id = msgget(IPC_PRIVATE, 0700);
	if (msq_id < 0) {
		perror("Msgget error:");
		return errno;
	}

	int n = 5;
	if (argc > 1)
		n = atoi(argv[1]);

	for (int i = 1; i <= n; i++) {
		int pid = fork();
		if (pid < 0) {
			perror("Fork error:");
			return errno;
		}
		if (pid == 0)
			return runner(i, n, msq_id);
	}

	int pid = fork();
	if (pid < 0) {
		perror("Fork error:");
		return errno;
	}
	if (pid == 0)
		return judge(n, msq_id);

	for (int i = 0; i < n; i++)
		waitpid(-1, NULL, 0);

	if (msgctl(msq_id, IPC_RMID, 0) < 0) {
		perror("Close msg error:");
		return errno;
	}
}

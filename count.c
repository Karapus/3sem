#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int number;

void *count1(void *args) {
	int n = *((int *)args);
	for (int i = 0; i < n; i++)
		number++;
	return NULL;
}

void *count2(void *args) {
	int n = *((int *)args);
	for (int i = 0; i < n; i++) {
		pthread_mutex_lock(&mutex);
		number++;
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

void *count3(void *args) {
	int n = *((int *)args);
	pthread_mutex_lock(&mutex);
	for (int i = 0; i < n; i++)
		number++;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void *count4(void *args) {
	int n = *((int *) args);
	int num;
	for (int i = 0; i < n; i++)
		num++;
	pthread_mutex_lock(&mutex);
	number += num;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

typedef void *fun_t (void *);

int main(int argc, char **argv) {
	if (argc != 4)
		return 0;
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	int alg = atoi(argv[3]);
	pthread_t ptids[m];
	pthread_t *ptid = ptids;
	int n_th = n / m;
	int n_last = n;
	fun_t *algs[] = {count1, count2, count3, count4};
	for (int i = 0; i < m - 1; i++) {
		pthread_create(ptid++, NULL, algs[alg - 1], &n_th);
		n_last -= n_th;
	}
	pthread_create(ptid++, NULL, algs[alg - 1], &n_last);
	for (pthread_t *p = ptids; p < ptids + m; p++)
		pthread_join(*p, NULL);
	printf("%d\n", number);
}

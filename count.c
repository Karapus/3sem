#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>


struct args_t {
	int n;
	int *p_num;
	pthread_mutex_t *m;
};

void *count1(void *args) {
	int n = ((struct args_t *) args)->n;
	int *p_num = ((struct args_t *) args)->p_num;
	for (int i = 0; i < n; i++)
		(*p_num)++;
	return NULL;
}

void *count2(void *args) {
	int n = ((struct args_t *) args)->n;
	int *p_num = ((struct args_t *) args)->p_num;
	pthread_mutex_t *m = ((struct args_t *) args)->m;
	for (int i = 0; i < n; i++) {
		pthread_mutex_lock(m);
		(*p_num)++;
		pthread_mutex_unlock(m);
	}
	return NULL;
}

void *count3(void *args) {
	int n = ((struct args_t *) args)->n;
	int *p_num = ((struct args_t *) args)->p_num;
	pthread_mutex_t *m = ((struct args_t *) args)->m;
	pthread_mutex_lock(m);
	for (int i = 0; i < n; i++)
		(*p_num)++;
	pthread_mutex_unlock(m);
	return NULL;
}

void *count4(void *args) {
	int n = ((struct args_t *) args)->n;
	int *p_num = ((struct args_t *) args)->p_num;
	pthread_mutex_t *m = ((struct args_t *) args)->m;
	int num;
	for (int i = 0; i < n; i++)
		num++;
	pthread_mutex_lock(m);
	*p_num += num;
	pthread_mutex_unlock(m);
	return NULL;
}

typedef void *fun_t (void *);

int main(int argc, char **argv) {
	if (argc != 4)
		return 0;
	int n = atoi(argv[1]);
	int m = atoi(argv[2]);
	int alg = atoi(argv[3]);
	int number = 0;
	pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
	struct args_t args = {n / m, &number, &mutex};
	pthread_t ptids[m];
	pthread_t *ptid = ptids;
	int n_last = n;
	fun_t *algs[] = {count1, count2, count3, count4};
	for (int i = 0; i < m - 1; i++) {
		pthread_create(ptid++, NULL, algs[alg - 1], &args);
		n_last -= args.n;
	}
	struct args_t lastargs = {n_last, &number, &mutex};
	pthread_create(ptid++, NULL, &count4, &lastargs);
	for (pthread_t *p = ptids; p < ptids + m; p++)
		pthread_join(*p, NULL);
	printf("%d\n", number);
}

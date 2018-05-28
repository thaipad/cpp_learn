#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <iostream>

struct arg1 {
	pthread_mutex_t mtx;
	pthread_cond_t cnd;
};	


void *start_pth1(void *val) {
	
	pthread_cond_wait(&(((arg1 *) val)->cnd), &(((arg1 *) val)->mtx));
	return 0;
}

void *start_pth2(void *val) {
	pthread_barrier_wait((pthread_barrier_t *) val);
	return 0;
}

int main(int argc, char **argv) {
	std::ofstream fout("/home/box/main.pid");
	fout << getpid();
	// mutex
	pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mtx, NULL);
//	pthread_mutex_lock(&mtx);
	// conditional var
	pthread_cond_t cnd = PTHREAD_COND_INITIALIZER;
	pthread_cond_init(&cnd, NULL);
	// barrier
	pthread_barrier_t brr;
	pthread_barrier_init(&brr, NULL, 1);

	arg1 a1;
	a1.mtx = mtx;
	a1.cnd = cnd;
	
	pthread_t pth1;
	pthread_t pth2;
	pthread_create(&pth1, NULL, start_pth1, &a1);
	pthread_create(&pth2, NULL, start_pth2, &brr);

	pause();
	return 0;
}

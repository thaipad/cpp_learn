#include <unistd.h>
#include <pthread.h>
#include <fstream>
#include <iostream>

void *start_pth1(void *val) {
	pthread_mutex_lock((pthread_mutex_t *) val);
	return 0;
}

void *start_pth2(void *val) {
	pthread_spin_lock((pthread_spinlock_t *) val);
	return 0;
}

void *start_pth3(void *val) {
	pthread_rwlock_rdlock((pthread_rwlock_t *) val);
	return 0;
}

void *start_pth4(void *val) {
	pthread_rwlock_wrlock((pthread_rwlock_t *) val);
	return 0;
}

int main(int argc, char **argv) {
	std::ofstream fout("/home/box/main.pid");
	fout << getpid();
	// mutex
	pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
	pthread_mutex_init(&mtx, NULL);
	pthread_mutex_lock(&mtx);
	// spin lock
	pthread_spinlock_t spn = (pthread_spinlock_t) 0;
	pthread_spin_init(&spn, 0);
	pthread_spin_lock(&spn);
	// RW-lock
	pthread_rwlock_t rl = PTHREAD_RWLOCK_INITIALIZER;
	pthread_rwlock_t wl = PTHREAD_RWLOCK_INITIALIZER;
 	pthread_rwlock_init(&rl, NULL);
	pthread_rwlock_init(&wl, NULL);
	pthread_rwlock_rdlock(&rl);
	pthread_rwlock_wrlock(&wl);




	pthread_t pth1;
	pthread_t pth2;
	pthread_t pth3;
	pthread_t pth4;
	pthread_create(&pth1, NULL, start_pth1, &mtx);
//	pthread_create(&pth2, NULL, start_pth2, &spn);
	pthread_create(&pth3, NULL, start_pth3, &rl);
	pthread_create(&pth4, NULL, start_pth4, &wl);

	pause();
	return 0;
}

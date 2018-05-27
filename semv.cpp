#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>

size_t NUM_SEM	{16};

int main(int argc, char **argv) {
	sem_t *semid = sem_open("/test.sem", O_CREAT, 0666, 66);
	
	pause();

	return 0;
}

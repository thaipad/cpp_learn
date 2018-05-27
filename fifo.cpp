/*************************************************************** 
* Parsing of multi command from STDOUT and executing using pipes
***************************************************************/

//#include <fstream>
//#include <cstdio>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
               
const char FIFO_R_NAME[]	= "/home/box/in.fifo";
const char FIFO_W_NAME[]	= "/home/box/out.fifo";
const int SIZE_OF_BUF		= 1024;

int main(int argc, char **argv) {
	mkfifo(FIFO_R_NAME, 0666);
	auto fifo_in = open(FIFO_R_NAME, O_RDONLY);

	mkfifo(FIFO_W_NAME, 0666);
	auto fifo_out = open(FIFO_W_NAME, O_WRONLY);
	
	while(true) {
		char buf[SIZE_OF_BUF];
    	int len = read(fifo_in, &buf, sizeof(buf));
    	write(fifo_out, &buf, len);
	}
	
	close(fifo_in);
	close(fifo_out);
	
	return 0;
}
	

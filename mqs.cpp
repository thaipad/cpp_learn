#include <fstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>

int main(int argc, char **argv) {
//	std::ofstream ftmp("/home/Work/tmp/msg.temp");
//	ftmp.close();
//	auto mqid = ftok("/home/Work/tmp/msg.temp", 1);
	struct mq_attr attr;
	attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 80;
    attr.mq_curmsgs = 0;	
	auto id = mq_open("/test.mq", O_CREAT, 0600, &attr);
	mq_unlink("/test.mq");
	char msg[80] = {'0'};
	unsigned int prio = 0;
	ssize_t size = mq_receive(id, msg, sizeof(msg), &prio);
	if (size > 0) {
		msg[size] = '\0';
		std::ofstream fout("./message.txt");
		if (fout.is_open()) {
			fout << msg;
			fout.close();
		}
	} 
	return 0;
}

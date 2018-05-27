#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int main(int argc, char **argv) {
	auto mqid = ftok("/home/Work/tmp/msg.temp", 8);
	auto msgid = msgget(mqid, IPC_CREAT | 0600);
	struct msg {
		long mtype;
		char mtext[80];
	} msgp;
	msgp.mtype = 1;
	strcpy(msgp.mtext, "Hello, world!");
	msgsnd(msgid, (struct msgbuf *)&msgp, 13, 0);
	return 0;
}

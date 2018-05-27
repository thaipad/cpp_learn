/*************************************************************** 
* Parsing of multi command from STDOUT and executing using pipes
***************************************************************/

#include <iostream>
//#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
  
using namespace std;

int main(int argc, char **argv) {
	int pfd[2];

	int res = socketpair(AF_UNIX, SOCK_STREAM, 0, pfd#include );

	if (fork() != 0) { // parent
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		dup2(pfd[0], STDIN_FILENO);
		dup2(pfd[1], STDOUT_FILENO);
		close(pfd[0]);
		close(pfd[1]);
		string str_in;
		cin >> str_in;
	} else { // child
		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		dup2(pfd[1], STDIN_FILENO);
		dup2(pfd[0], STDOUT_FILENO);
		close(pfd[0]);
		close(pfd[1]);
		string str_in;
		cin >> str_in;
	} 
	
	
	
	return 0;
}

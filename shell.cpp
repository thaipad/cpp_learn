/*************************************************************** 
* Parsing of multi command from STDOUT and executing using pipes
***************************************************************/

#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

using namespace std;

vector<vector<string>> parsing_commands(string);
void exec_commands(vector<vector<string>>, int);

int main(int argc, char **argv) {
	string buf;
	getline(cin, buf);

	vector<vector<string>> list_of_com = parsing_commands(buf);
	
	exec_commands(list_of_com, list_of_com.size() - 1);
	
	return 0;
}

void exec_commands(vector<vector<string>> list_of_com, int iter) {
	int pfd[2];
	pipe(pfd);
	int chpid = -1;
	if (iter > 0) {
		chpid = fork();
	}
	if (chpid != 0) { // parent
		auto com = list_of_com[iter];
		char *arg_com[com.size() + 1];
		for (int i = 0; i < com.size(); i++) {
			char *str = new char[com[i].length()];
			strcpy(str, com[i].c_str());
			arg_com[i] = str;
		}
		arg_com[com.size()] = NULL;

		close(STDIN_FILENO);
		dup2(pfd[0], STDIN_FILENO);
		close(pfd[0]);
		close(pfd[1]);
		if (iter == list_of_com.size() - 1) {
			int fd = open("res.out", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        	dup2(fd, STDOUT_FILENO);
        	close(fd);
        }
		execvp(arg_com[0], arg_com);
		for (int i = 0; i < com.size(); i++) {
			delete[] arg_com[i];
		}
	} else { // child
		close(STDOUT_FILENO);
		dup2(pfd[1], STDOUT_FILENO);
		close(pfd[0]);
		close(pfd[1]);
		exec_commands(list_of_com, iter - 1);
	} 
}

vector<vector<string> > parsing_commands(string buf) { 
	buf += " |";

	vector<string> com;
	vector<vector<string> > list_of_com;
	istringstream ss(buf);
	string temp_str;
	while(ss >> temp_str) {
		if (temp_str != "|") {
			com.push_back(temp_str);
		} else {
			list_of_com.push_back(com);
			com.clear();
		}
	}
	return list_of_com;
}

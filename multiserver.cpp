#include <iostream> 
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

const int MY_PORT	{12345};

int set_nonblock(int fd) {
	int flags;
#if defined(O_NONBLOCK)
	if(-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_GETFL, 0 | O_NONBLOCK);
#else
	flafs = 1;
	return ioctl(fd, FIOBIO, &flags);
#endif 
}

int main(int argc, char **argv) {
	int master_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	std::set<int> slave_sockets;
	struct sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(MY_PORT);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(master_socket, (struct sockaddr *)(&sock_addr), sizeof(sock_addr));
	
	set_nonblock(master_socket);
	
	listen(master_socket, SOMAXCONN);
	
	while (true) {
		fd_set set;
		FD_ZERO(&set);
		FD_SET(master_socket, &set);
		for (auto iter = slave_sockets.begin(); iter != slave_sockets.end(); iter++) {
			FD_SET(*iter, &set);
		}
		int max_socket = std::max(master_socket, *std::max_element(slave_sockets.begin(), slave_sockets.end()));
		
		select(max_socket + 1, &set, NULL, NULL, NULL);
		
		for (auto iter = slave_sockets.begin(); iter != slave_sockets.end(); iter++) {
			if (FD_ISSET(*iter, &set)) {
				// event is here
				static char buffer[1024];
				int recv_size = recv(*iter, buffer, sizeof(buffer), MSG_NOSIGNAL);
				if (recv_size == 0 && errno != EAGAIN) {
					shutdown(*iter, SHUT_RDWR);
					close(*iter);
					slave_sockets.erase(iter);
				} else if (recv_size != 0) {
					send(*iter, buffer, recv_size, MSG_NOSIGNAL);
				}
			}
		}
		if (FD_ISSET(master_socket, &set)) {
			int slave_socket = accept(master_socket, 0, 0);
			set_nonblock(slave_socket);
			slave_sockets.insert(slave_socket);
		}
	}
	
	return 0;
}

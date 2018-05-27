#include <iostream> 
#include <algorithm>
#include <set>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

const int POLL_SIZE			{2048};
const int MY_PORT			{12345};
const int SIZE_OF_BUFFER 	{1024};

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
	
	struct pollfd set[POLL_SIZE + 1];
	set[0].fd = master_socket;
	set[0].events = POLLIN;
	
	while (true) {
		unsigned int index = 1;
		for (auto iter = slave_sockets.begin(); iter != slave_sockets.end(); iter++) {
			set[index].fd = *iter;
			set[index].events = POLLIN;
			index++;
		}
		unsigned int set_size = 1 + slave_sockets.size();
		
		poll(set, set_size, -1);
		
		for (auto i = 0; i < set_size; i++) {
			if (set[i].revents & POLLIN) { //revents - are return events
				if (i != 0) {
					static char buffer[SIZE_OF_BUFFER];
					int recv_size = recv(set[i].fd, buffer, SIZE_OF_BUFFER, MSG_NOSIGNAL);
					if (recv_size == 0 && errno != EAGAIN) {
						shutdown(set[i].fd, SHUT_RDWR);
						close(set[i].fd);
						slave_sockets.erase(set[i].fd);
					} else if (recv_size > 0) {
						send(set[i].fd, buffer, recv_size, MSG_NOSIGNAL);
					}
				} else {
					int slave_socket = accept(master_socket, 0, 0);
					set_nonblock(slave_socket);
					slave_sockets.insert(slave_socket);
				}
			}	
		}
	}
	
	return 0;
}

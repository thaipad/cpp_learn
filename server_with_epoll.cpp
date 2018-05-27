#include <iostream> 
#include <unordered_map>
#include <cstring>
#include <sstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <arpa/inet.h>

const int MY_PORT			{12345};
const int SIZE_OF_BUFFER 	{1024};
const int MAX_EVENTS		{32};

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
	struct sockaddr_in sock_addr;
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_port = htons(MY_PORT);
	sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(master_socket, (struct sockaddr *)(&sock_addr), sizeof(sock_addr));
	
	set_nonblock(master_socket);
	
	listen(master_socket, SOMAXCONN);
	
	int epoll = epoll_create1(0);
		
	struct epoll_event event;
	event.data.fd = master_socket;
	event.events = EPOLLIN;
	epoll_ctl(epoll, EPOLL_CTL_ADD, master_socket, &event);
	
	std::unordered_map<unsigned int, struct sockaddr_in> clients;
	
	while (true) {
		struct epoll_event events[MAX_EVENTS];
		int n_events = epoll_wait(epoll, events, MAX_EVENTS, -1);
		
		for (unsigned int i = 0; i < n_events; i++) {  
			if (events[i].data.fd == master_socket) {
				struct sockaddr_in sock_addr_tmp;
				unsigned int size_tmp;
				int slave_socket = accept(master_socket, (struct sockaddr *) &sock_addr_tmp, &size_tmp);
				clients[slave_socket] = sock_addr_tmp;
				set_nonblock(slave_socket);
				struct epoll_event event;
				event.data.fd = slave_socket;
				event.events = EPOLLIN;
				epoll_ctl(epoll, EPOLL_CTL_ADD, slave_socket, &event);
			} else {
				char buffer[SIZE_OF_BUFFER];
				int recv_size = recv(events[i].data.fd, buffer, strlen(buffer), MSG_NOSIGNAL);
				if (recv_size == 0 && errno != EAGAIN) {		
					shutdown(events[i].data.fd, SHUT_RDWR);
					close(events[i].data.fd);
					clients.erase(events[i].data.fd);
				} else if (recv_size > 0) {
					char ip_addr[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &clients[events[i].data.fd].sin_addr, ip_addr, INET_ADDRSTRLEN + 2);
					std::stringstream msg;
					msg << ip_addr << ": " << buffer << "\0";
					for (auto client : clients) {  
						if (client.first  != master_socket && client.first != events[i].data.fd) {
							send(client.first, msg.str().c_str(), msg.str().length(), MSG_NOSIGNAL);
						}
					}
				}
			}
		}
	}
	
	return 0;
}

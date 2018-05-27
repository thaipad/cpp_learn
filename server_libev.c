#include <stdio.h>
#include <netinet/in.h>
#include <ev.h>

#define SOCKET_PORT	12345
#define SIZE_BUFFER 1024

void read_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	char buffer[SIZE_BUFFER];
	ssize_t r = recv(watcher->fd, buffer, SIZE_BUFFER, MSG_NOSIGNAL);
	if (r > 0) {
		send(watcher->fd, buffer, r, MSG_NOSIGNAL);	
	} else if (r == 0) {
		ev_io_stop(loop, watcher);
		free(watcher);
	}
}

void accept_cb(struct ev_loop *loop, struct ev_io *watcher, int revents) {
	int client_sd = accept(watcher->fd, 0, 0);
	
	struct ev_io *w_client = (struct ev_io*) malloc(sizeof(struct ev_io));
	
	ev_io_init(w_client, read_cb, client_sd, EV_READ);
	ev_io_start(loop, w_client);
}

int main(int argc, char **argv) {
	struct ev_loop *loop = ev_default_loop(0);
	
	int sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SOCKET_PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(sd, (struct sickaddr *)&addr, sizeof(addr));
	
	listen(sd, SOMAXCONN);
	
	struct ev_io w_accept;
	ev_io_init(&w_accept, accept_cb, sd, EV_READ);
	ev_io_start(loop, &w_accept);
	
	while(1) {
		ev_loop(loop, 0);
	}	
		
	return 0;
}

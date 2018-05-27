#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <uv.h>

#define SOCKET_IP	"127.0.0.1"
#define SOCKET_PORT	12345
#define SIZE_BUFFER 1024

uv_tcp_t server;
uv_loop_t *loop;

void read_cb(uv_stream_t *stream, ssize_t nread, uv_buf_t buf) {
	uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
	char wbuf[nread];
	memcpy(wbuf, buf.base, nread);
	uv_buf_t buf_for_write;
	buf_for_write.base = wbuf;
	buf_for_write.len = nread;
	uv_write(req, stream, &buf_for_write, 1, NULL);
	free(buf.base);
}

uv_buf_t alloc_buffer(uv_handle_t *handle, size_t size) {
	return uv_buf_init((char*) malloc(size), size);
}

void connection_cb(uv_stream_t *server, int status) {
	uv_tcp_t *client = malloc(sizeof(uv_tcp_t));
	
	uv_tcp_init(loop, client);
	uv_accept(server, (uv_stream_t*) client);
	uv_read_start((uv_stream_t*) client, alloc_buffer, read_cb);
}

int main(int argc, char **argv) {
	loop = uv_default_loop();
	
	struct sockaddr_in addr = uv_ip4_addr(SOCKET_IP, SOCKET_PORT);
	
	uv_tcp_init(loop, &server);
	uv_tcp_bind(&server, addr);
	uv_listen((uv_stream_t *)&server, 128, connection_cb);	
		
	return uv_run(loop, UV_RUN_DEFAULT);
}

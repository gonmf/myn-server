/*
    This file is part of myn.

    myn is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    myn is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with myn.  If not, see <http://www.gnu.org/licenses/>.
*/



#include "inc/common.h"
static int socket_factory;
static struct sockaddr_in server_addr, client_addr;
static socklen_t socket_addr_siz;
static int server_setup(int port){
	socket_factory = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_factory == -1)
		abort_m("Failed to create socket factory.");
	socket_addr_siz = sizeof(client_addr);
	server_socket_init(&server_addr, sizeof(client_addr), AF_INET, INADDR_ANY, htons(port));
	if(server_bind(socket_factory, (struct sockaddr *)&server_addr, sizeof(client_addr)) < 0){
		if(port < 1024)
			fprintf(_out, "(master) Port binding failed of port %d - are you root? Is it bound already?\n", port);
		else
			fprintf(_out, "(master) Port binding failed of port %d - is it bound already?\n", port);
		return 1;
	}
	if(server_listen(socket_factory, CONN_POOL))
		abort_m("Failed to set the socket factory to listen for incoming connections.");
	fprintf(_out, "Online listening on port %d (conn. queue of %d + %d).\n", port, CONN_POOL, __ACCEPTED_PENDING_CONNS_);
	return 0;
}
static int server_accept(int * request_socket){
	*request_socket = accept(socket_factory, (struct sockaddr *) &client_addr, &socket_addr_siz);
	if(*request_socket <= 0)
		abort_m("Client failed to bind with server.");
	char buf[INET_ADDRSTRLEN + 1];
	ip_to_text(AF_INET, &(client_addr.sin_addr), buf, INET_ADDRSTRLEN);
	fprintf(_out, "Connection received from %s:%d.\n", buf, client_addr.sin_port);
	return 0;
}
static void * listener(void * arg){
	int request_socket;
	while(1)
		if(!server_accept(&request_socket))
			ring_put(&_conns, (void *)request_socket);
	return NULL;
}
void server_main(int port, int dmn){
	signal(SIGPIPE, SIG_IGN);
	if(dmn){
		_out = tmpfile();
		if(daemon(1, 1))
			abort_m("Failed to detach daemon.");
	}else
		_out = stdout;
	pthread_t threads[WORKER_THREADS];
	pthread_t listener_thread;
	fprintf(_out, "\n  --- %s HTTP/1.1 Server (build %s:%s)  ---\n\n", __PROGRAM_NAME_, __PROGRAM_VERSION_, __PROGRAM_DATE_);
	memmngr_init();
	load_cgi_modules();
	if(server_setup(port))
		abort_m("Failed to bind web server.");
	ring_init(&_conns, __ACCEPTED_PENDING_CONNS_);
	for(int i = 0; i < WORKER_THREADS; ++i)
		if(posix_thread_create(&threads[i], NULL, worker_main, (void *)i))
			abort_m("Failed to create worker thread(s).");
	if(posix_thread_create(&listener_thread, NULL, listener, NULL))
		abort_m("Failed to create listener thread.");
	fprintf(_out, "\n  --- Press <ENTER> to close ---\n\n");
	getc(stdin);
	_HALT = 1;
	for(int i = 0; i < WORKER_THREADS; ++i)
		while(ring_put(&_conns, (void *)0));
	for(int i = 0; i < WORKER_THREADS; ++i)
		posix_thread_join(threads[i], NULL);
	fprintf(_out, "Terminated.\n");
	exit(EXIT_SUCCESS);
}

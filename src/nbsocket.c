#include "nbsocket.h"


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <pthread.h>

#define PATH_SOCKET "socket_nbcomms"

//#define NB_DEBUG_LOGS

int listening = 0;

int fd_socket;
socklen_t len_socket_addr;
struct sockaddr_un socket_addr_local;

int fd_socket_remote;
struct sockaddr_un socket_addr_remote;


void nbPrintf(char* fmt, ...)
{
#ifdef NB_DEBUG_LOGS
    va_list args;
    va_start(args,fmt);
    vprintf(fmt,args);
    va_end(args);
#endif
}

// flag disconnection and error message
void nb_error(char *msg, int critical) {
	perror(msg);
	listening = 0;
	if (critical) {
		exit(1);
	}
	else {
		close(fd_socket_remote);
	}
}

const int LENGTH_BUFFER = 100;

void * nb_receive(void *ptr) {
	void (*cb_recv_data)(char*, char*);
	cb_recv_data = (void (*)(char*, char*))ptr;
	int done = 0;
	int len_bytes_rx = 0;
	char str_buffer_rx[LENGTH_BUFFER];
	do {
		len_bytes_rx = 0;
		ioctl(fd_socket_remote, FIONREAD, &len_bytes_rx);
		if (len_bytes_rx > 0) {
			//len = read(sock, buffer, len);
			len_bytes_rx = recv(fd_socket_remote, str_buffer_rx, LENGTH_BUFFER, 0);
			str_buffer_rx[len_bytes_rx] = '\0';
			nbPrintf("nbcomms: received %i bytes - %s\n", len_bytes_rx, str_buffer_rx);
			cb_recv_data("received", str_buffer_rx);
			if (len_bytes_rx < 0) perror("recv");
			if (len_bytes_rx <= 0) {
				done = 1;
			}
		}
	} while (!done);
	nbPrintf("Exiting nb_receive\n");
	return NULL;
}

pthread_t thread_id_recv;
void setup_recv_timer(void (*cb_recv_data)(char*, char*)) {
	nbPrintf("nbcomms: creating receive thread\n");
	int rc = 0;
	rc = pthread_create(&thread_id_recv, NULL, nb_receive, (void*)cb_recv_data);
	if (rc) {
		nb_error("thread", 1);
	}
}

void wait_for_conn(void) {
    nbPrintf("nbcomms: Waiting for a connection\n");
    len_socket_addr = sizeof(socket_addr_remote);
    if ((fd_socket_remote = accept(fd_socket, (struct sockaddr *)&socket_addr_remote, &len_socket_addr)) == -1) {
        nb_error("accept", 1);
    }
}

int nb_listen(void (*cb_listening)(void), void (*cb_recv_data)(char*, char*)) {
    nbPrintf("nbcomms: Listen called\n");
	// create socket fd
    if ((fd_socket = socket(AF_LOCAL, SOCK_STREAM, 0)) == -1) {
        nb_error("socket", 1);
    }

	//setup socket address
	socket_addr_local.sun_family = AF_LOCAL;
    strcpy(socket_addr_local.sun_path, PATH_SOCKET);
    unlink(socket_addr_local.sun_path); //remove previous socket file if it already exists
    len_socket_addr = strlen(socket_addr_local.sun_path) + sizeof(socket_addr_local.sun_family);

	// bind address to fd of the socket
    if (bind(fd_socket, (struct sockaddr *)&socket_addr_local, len_socket_addr) == -1) {
        nb_error("bind", 1);
    }
	// listen for incoming connections
    if (listen(fd_socket, 5) == -1) {
        nb_error("listen", 1);
    }

	wait_for_conn();

	listening = 1;
	cb_listening();

	setup_recv_timer(cb_recv_data);
	return listening;
}

// takes null terminated strings for name and value
int nb_send(const char *name, const char *value) {
    nbPrintf("nbcomms: send called with %s : %s \n", name, value);
	char str_buffer_tx[LENGTH_BUFFER];
	if (listening) {
		sprintf(str_buffer_tx, "{%s:%s}\n", name, value); //TODO: later using json lib when more complex json is passed
	    nbPrintf("nbcomms: sending %s\n", str_buffer_tx);
		int len_bytes_tx = strlen(str_buffer_tx);
		if (send(fd_socket_remote, str_buffer_tx, len_bytes_tx, 0) < 0) {
		    nb_error("send", 0);
		}
	}
	return listening;
}



#ifndef NBSOCKET_H
#define NBSOCKET_H

#ifdef __cplusplus
extern "C" {
#endif

// Open socket to listen for a connection
// (intentended for the nodejs module that spawned this executable)
//int nb_listen(void (*cb_recv_data)(char*, char*), void (*cb_listening)(void));
int nb_listen(void (*cb_listening)(void), void (*cb_recv_data)(char*, char*));
//int nb_listen();

// Send name and value as json over socket
// (intended to be from the nodejs module that spawned this executable)
int nb_send(const char *name, const char *value);

#ifdef __cplusplus
}
#endif

#endif

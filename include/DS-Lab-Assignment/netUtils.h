#ifndef NETUTILS_H
#define NETUTILS_H

#define MAX_CONN_BACKLOG 10     /* max number of open client connections */

/* sending functions */
int send_server_reply(int socket, reply_t *reply);

/* receiving functions */
int recv_string(int socket, char *string);

#endif //NETUTILS_H

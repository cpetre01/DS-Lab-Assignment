#ifndef SERVICES_H
#define SERVICES_H

#include "DS-Lab-Assignment/util.h"

/*** Services ***/
void srv_register(int socket, request_t *request, reply_t *reply, entry_t *entry);
void srv_unregister(int socket, request_t *request, reply_t *reply);
void srv_connect(int socket, request_t *request, reply_t *reply, entry_t *entry);
void srv_disconnect(int socket, request_t *request, reply_t *reply, entry_t *entry);
void srv_send(int socket, request_t *request, reply_t *reply, entry_t *entry);

#endif //SERVICES_H

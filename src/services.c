#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "DS-Lab-Assignment/netUtil.h"
#include "DS-Lab-Assignment/dbms/dbms.h"
#include "DS-Lab-Assignment/services.h"


/*** Services ***/
void srv_register(const int socket, request_t *request, reply_t *reply, entry_t *entry) {
    if (recv_string(socket, request->username) < 0) return;

    /* set up user entry */
    strcpy(entry->username, request->username);
    entry->type = ENT_TYPE_UD;
    entry->user.last_msg_id = 0;

    /* create user entry in DB */
    reply->server_error_code = (db_creat_usr_tbl(entry) == DBMS_ERR_EXISTS) ?
            SRV_ERR_REG_USR_ALREADY_REG : SRV_SUCCESS;

    /* server log message */
    if (reply->server_error_code == SRV_SUCCESS) {
        printf("s> %s %s OK\n", REGISTER, request->username); fflush(stdout);
    } else {
        printf("s> %s %s FAIL\n", REGISTER, request->username); fflush(stdout);
    }

    /* no need to error handle this call: whether it fails or not, the server
     * is just going to move on(continue in the while loop) */
    send_server_reply(socket, reply);
}


void srv_unregister(const int socket, request_t *request, reply_t *reply) {
    if (recv_string(socket, request->username) < 0) return;

    /* check whether user exists */
    int user_exists = db_user_exists(request->username);

    if (user_exists == TRUE)
        reply->server_error_code = (db_del_usr_tbl(request->username) < 0) ?
                SRV_ERR_UNREG_ANY : SRV_SUCCESS;
    else if (user_exists == FALSE)
        reply->server_error_code = SRV_ERR_UNREG_USR_NOT_EXISTS;
    else reply->server_error_code = SRV_ERR_UNREG_ANY;

    /* server log message */
    if (reply->server_error_code == SRV_SUCCESS) {
        printf("s> %s %s OK\n", UNREGISTER, request->username); fflush(stdout);
    } else {
        printf("s> %s %s FAIL\n", UNREGISTER, request->username); fflush(stdout);
    }

    /* send reply to client */
    send_server_reply(socket, reply);
}


void srv_connect(const int socket, request_t *request, reply_t *reply, entry_t *entry) {
    struct sockaddr_in client_addr;

    if (recv_string(socket, request->username) < 0) return;
    if (recv_string(socket, request->client_port) < 0) return;

    /* set up user entry */
    strcpy(entry->username, request->username);
    entry->type = ENT_TYPE_UD;

    /* read user entry from DB */
    int io_result = db_io_op_usr_ent(entry, READ);

    if (io_result == DBMS_ERR_NOT_EXISTS)
        reply->server_error_code = SRV_ERR_CN_USR_NOT_EXISTS;
    else if (io_result < 0)
        reply->server_error_code = SRV_ERR_CN_ANY;      /* some other error */
    else {    /* user entry was read successfully */
        if (entry->user.status == STATUS_CN)
            reply->server_error_code = SRV_ERR_CN_USR_ALREADY_CN;
        else {    /* entry->user.status == STATUS_DCN */
            /* prepare entry to write it to DB */
            /* cast the client port to short */
            int tmp_port;
            if (str_to_num(request->client_port, (void *) &tmp_port, INT) < 0)
                reply->server_error_code = SRV_ERR_CN_ANY;

            /* get client IP */
            socklen_t client_addr_size = sizeof client_addr;
            if (getpeername(socket, (struct sockaddr *) &client_addr, &client_addr_size) < 0)
                reply->server_error_code = SRV_ERR_CN_ANY;

            /* set up entry */
            if (reply->server_error_code != SRV_ERR_CN_ANY) {
                entry->user.status = STATUS_CN;
                entry->user.port = (uint16_t) tmp_port;
                entry->user.ip = client_addr.sin_addr;
                printf("Client IP: %s\n", inet_ntoa(client_addr.sin_addr));

                /* update user entry in DB */
                io_result = db_io_op_usr_ent(entry, MODIFY);
                if (io_result == DBMS_ERR_NOT_EXISTS)
                    reply->server_error_code = SRV_ERR_CN_USR_NOT_EXISTS;
                else if (io_result < 0)
                    reply->server_error_code = SRV_ERR_CN_ANY;      /* some other error */
                else reply->server_error_code = SRV_SUCCESS;        /* user entry was correctly updated */
            }
        }
    }

    /* server log message */
    if (reply->server_error_code == SRV_SUCCESS) {
        printf("s> %s %s OK\n", CONNECT, request->username); fflush(stdout);
    } else {
        printf("s> %s %s FAIL\n", CONNECT, request->username); fflush(stdout);
    }

    /* send reply to client */
    send_server_reply(socket, reply);

}


void srv_disconnect(const int socket, request_t *request, reply_t *reply, entry_t *entry) {
    if (recv_string(socket, request->username) < 0) return;

    /* set up user entry */
    strcpy(entry->username, request->username);
    entry->type = ENT_TYPE_UD;

    /* read user entry from DB */
    int io_result = db_io_op_usr_ent(entry, READ);

    if (io_result == DBMS_ERR_NOT_EXISTS)
        reply->server_error_code = SRV_ERR_DCN_USR_NOT_EXISTS;
    else if (io_result < 0)
        reply->server_error_code = SRV_ERR_DCN_ANY;     /* some other error */
    else {    /* user entry was read successfully */
        if (entry->user.status == STATUS_DCN)
            reply->server_error_code = SRV_ERR_DCN_USR_NOT_CN;
        else {    /* entry->user.status == STATUS_CN */
            /* set up entry */
            entry->user.status = STATUS_DCN;
            bzero(&entry->user.ip, sizeof(struct in_addr));
            bzero(&entry->user.port, sizeof(uint16_t));

            /* update user entry in DB */
            io_result = db_io_op_usr_ent(entry, MODIFY);
            if (io_result == DBMS_ERR_NOT_EXISTS)
                reply->server_error_code = SRV_ERR_DCN_USR_NOT_EXISTS;
            else if (io_result < 0)
                reply->server_error_code = SRV_ERR_DCN_ANY;     /* some other error */
            else reply->server_error_code = SRV_SUCCESS;        /* user entry was correctly updated */
        }
    }

    /* server log message */
    if (reply->server_error_code == SRV_SUCCESS) {
        printf("s> %s %s OK\n", DISCONNECT, request->username); fflush(stdout);
    } else {
        printf("s> %s %s FAIL\n", DISCONNECT, request->username); fflush(stdout);
    }

    /* send reply to client */
    send_server_reply(socket, reply);
}


void srv_send(const int socket, request_t *request, reply_t *reply, entry_t *entry) {
    /* receive stuff */
    if (recv_string(socket, request->sender) < 0) return;
    if (recv_string(socket, request->receiver) < 0) return;
    if (recv_string(socket, request->message.content) < 0) return;

    /* check that both users exist */
    int sender_exists = db_user_exists(request->sender);
    int receiver_exists = db_user_exists(request->receiver);

    if (!sender_exists || !receiver_exists) reply->server_error_code = SRV_ERR_SEND_USR_NOT_EXISTS;
    else if (sender_exists < 0 || receiver_exists < 0) reply->server_error_code = SRV_ERR_SEND_ANY;
    else {    /* set up first ACK to be sent to sender client */
        reply->server_error_code = SRV_SUCCESS;

        /* update last msg ID in receiver user */
        /* read receiver user entry */
        entry_t receiver_entry;
        if (db_io_op_usr_ent(&receiver_entry, READ) < 0) {
            reply->server_error_code = SRV_ERR_SEND_ANY;
        } else {
            /* update last msg ID & write it to DB */
            receiver_entry.user.last_msg_id = (receiver_entry.user.last_msg_id + 1) % MSG_ID_MAX_VALUE;
            if (db_io_op_usr_ent(&receiver_entry, MODIFY) < 0) {
                reply->server_error_code = SRV_ERR_SEND_ANY;
            }
        }
    }

    /* send reply to sender client (first ACK if success, error otherwise) */
    if (send_server_reply(socket, reply) < 0) return;

    /* message passing */

}

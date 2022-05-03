#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "DS-Lab-Assignment/utils.h"
#include "DS-Lab-Assignment/netUtils.h"


int send_server_reply(const int socket, reply_t *reply) {
    /* function that sends server_error_code member to socket */
    CHECK_SOCKET_ERROR(write_bytes(socket, (const char *) &reply->server_error_code, 1), -1)
//    if (write_bytes(socket, (const char *) &reply->server_error_code, 1) == -1) {
//        perror("Send server_error_code error");
//        close(socket);
//        return -1;
//    }

    return 0;
}


int send_string(const int socket, const char *string) {
    /* function that sends a string to socket */
    CHECK_SOCKET_ERROR(write_bytes(socket, string, (int) (strlen(string) + 1)), -1)
//    if (write_bytes(socket, string, (int) (strlen(string) + 1)) == -1) {
//        perror("Send string error");
//        close(socket);
//        return -1;
//    }

    return 0;
}


int recv_string(const int socket, char *string) {
    /* function that receives a string from socket */
    CHECK_SOCKET_ERROR(read_line(socket, string, MAX_MSG_SIZE), -1)
//    if (read_line(socket, string, MAX_MSG_SIZE) == -1) {
//        perror("Receive string error");
//        close(socket);
//        return -1;
//    }

    return 0;
}

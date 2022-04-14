#ifndef UTILS_H
#define UTILS_H

/* utilities */
#define TRUE 1
#define FALSE 0
#define MAX_STR_SIZE 512            /* generic string size */
#define MAX_MSG_SIZE 256            /* size of message string */
#define DB_NAME "db"                /* database directory name */

/* services: operation codes */
#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define SEND "SEND"

/* server error codes */
//WIP
#define SRV_SUCCESS 0
#define SRV_ERROR 1

/* DB key file opening modes */
#define READ 'r'
#define CREATE 'c'
#define MODIFY 'm'

/* number casting stuff */
#define INT 'i'
#define FLOAT 'f'
int str_to_num(const char *value_str, void *value, char type);

/* file & socket stuff */
int send_msg(int d, char *buffer, int len);
int recv_msg(int d, char *buffer, int len);
ssize_t read_line(int d, char *buffer, int buf_space);


/* types used for process communication */
typedef struct {
    /* client request */
    char op_code;               /* operation code that indicates the client API function called */
    char username[MAX_STR_SIZE];
    char client_port[MAX_STR_SIZE];
    char rcv_usr[MAX_STR_SIZE];
    char message[MAX_MSG_SIZE];
} request_t;

typedef struct {
    /* server reply */
    int32_t server_error_code;  /* error code returned by the server; client API interprets it
 *                              to figure out whether the transaction was successful */
} reply_t;

typedef struct {
    /* message sent from server to receiver client */
    char op_code;               /* operation code that indicates the client API function called */
    char message[MAX_MSG_SIZE];
} server_msg_t;

#endif //UTILS_H
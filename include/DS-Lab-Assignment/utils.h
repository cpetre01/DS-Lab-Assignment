#ifndef UTILS_H
#define UTILS_H

/* utilities */
#define TRUE 1
#define FALSE 0
#define MAX_STR_SIZE 512            /* generic string size */
#define MAX_MSG_SIZE 256            /* size of content string */
#define DB_NAME "db"                /* database directory name */

/* services: operation codes */

/* called by client */
#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define SEND "SEND"

/* called by server */
#define SEND_MESSAGE "SEND_MESSAGE"
#define SEND_MESS_ACK "SEND_MESS_ACK"

/* server error codes */
#define SRV_SUCCESS 0

/* register */
#define SRV_ERR_REG_USR_ALREADY_REG 1
#define SRV_ERR_REG_ANY 2

/* unregister */
#define SRV_ERR_UNREG_USR_NOT_EXISTS 1
#define SRV_ERR_UNREG_ANY 2

/* connect */
#define SRV_ERR_CN_USR_NOT_EXISTS 1
#define SRV_ERR_CN_USR_ALREADY_CN 2
#define SRV_ERR_CN_ANY 3

/* disconnect */
#define SRV_ERR_DCN_USR_NOT_EXISTS 1
#define SRV_ERR_DCN_USR_NOT_CN 2
#define SRV_ERR_DCN_ANY 3

/* send */
#define SRV_ERR_SEND_USR_NOT_EXISTS 1
#define SRV_ERR_SEND_ANY 2

/* DB file opening modes */
#define READ 'r'
#define CREATE 'c'
#define MODIFY 'm'

/* number casting stuff */
#define INT 'i'
#define FLOAT 'f'
int str_to_num(const char *value_str, void *value, char type);

/* file & socket stuff */
int send_msg(int d, const char *buffer, int len);
int recv_msg(int d, char *buffer, int len);
ssize_t read_line(int d, char *buffer, int buf_space);


/* types used for process communication */
typedef struct {
    /* message sent between server & client, both ways */
    u_int32_t id;               /* message ID */
    char content[MAX_MSG_SIZE]; /* message content */
} message_t;

typedef struct {
    /* client request */
    char op_code[MAX_STR_SIZE];     /* operation code that indicates the service called */
    union {
        struct {
            char username[MAX_STR_SIZE];    /* member used for REGISTER, UNREGISTER, CONNECT & DISCONNECT services */
            char client_port[MAX_STR_SIZE]; /* client listening thread port; member only used for CONNECT service */
        };
        struct {    /* members used for SEND, SEND_MESSAGE & SEND_MESS_ACK services */
            char sender[MAX_STR_SIZE];      /* username of sender client */
            char receiver[MAX_STR_SIZE];    /* username of receiver client */
            message_t message;
        };
    };
} request_t;

typedef struct {
    /* server reply */
    char server_error_code;     /* error code returned by the server; client interprets it
 *                              to figure out whether the transaction was successful */
} reply_t;

#endif //UTILS_H

/*register, unregister, disconnect*/
//receive op_code
//receive username

/*connect*/
//receive op_code
//receive username
//receive port

/*send*/
//receive op_code
//receive sender username
//receive receiver username
//receive message ID
//receive message content
//send message ID to sender client (first ACK: server got the message)

/*content passing from server to client*/

/*send content to receiver*/
//send op_code
//send sender username
//send message ID
//send message content

/*send second ack to sender (message got delivered)*/
//send op_code
//send message ID

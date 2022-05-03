#ifndef UTILS_H
#define UTILS_H

/* utilities */
#define TRUE 1
#define FALSE 0
#define MAX_STR_SIZE 512                /* generic string size */
#define MAX_MSG_SIZE 256                /* size of message content string */
#define MSG_ID_MAX_VALUE 4294967295     /* max message ID value (actually max unsigned int value on amd64) */
#define MSG_ID_MAX_STR_SIZE 10          /* max length of message ID as a string */

/* services: operation codes */

/* services called by client, served by server */
#define REGISTER "REGISTER"
#define UNREGISTER "UNREGISTER"
#define CONNECT "CONNECT"
#define DISCONNECT "DISCONNECT"
#define SEND "SEND"
#define SENDATTACH "SENDATTACH"
#define TEST "TEST"

/* services called by server, served by client */
#define SEND_MESSAGE "SEND_MESSAGE"
#define SEND_MESS_ACK "SEND_MESS_ACK"

/* server error codes */

/* general */
#define SRV_SUCCESS 0
#define TEST_ERR_CODE 100

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

/* client connection states
 * used in userdata.status */
#define STATUS_DCN 0
#define STATUS_CN 1

/* DBMS stuff */

/* DBMS error codes */
#define DBMS_SUCCESS 0
#define DBMS_ERR_ANY -1
#define DBMS_ERR_INV_ARGS -2
#define DBMS_ERR_ENT_NOT_EXISTS -3
#define DBMS_ERR_ENT_EXISTS -4

/* DB file opening modes */
#define READ 'r'
#define CREATE 'c'
#define MODIFY 'm'
#define DELETE 'd'

/* SCHEMA */
#define DB_DIR "users"                          /* database directory name */
#define USERDATA_ENTRY "userdata.entry"         /* userdata entry name */
#define PEND_MSGS_TABLE "pend_msgs-table"       /* pending messages table name */

/* DB entry types */
#define ENT_TYPE_UD 0       /* userdata entry type */
#define ENT_TYPE_P_MSG 1    /* pending message entry type */


/* number casting stuff */
#define INT 'i'
#define FLOAT 'f'
int str_to_num(const char *value_str, void *value, char type);

/* file & socket stuff */
int write_bytes(int d, const char *buffer, int len);
int read_bytes(int d, char *buffer, int len);
int read_line(int d, char *buffer, int buf_space);


/* Macros */

/* macro for general error checking */
#define CHECK_ERROR(FUNCTION_CALL, RETURN_ERROR) \
    int ret_val = (FUNCTION_CALL); \
    if (ret_val == -1) { \
        fprintf(stderr, "Runtime error: %s returned %d at %s:%d\n", #FUNCTION_CALL, ret_val, __FILE__, __LINE__); \
        return (RETURN_ERROR); \
    }

/* macro for socket error checking */
#define CHECK_SOCKET_ERROR(FUNCTION_CALL, RETURN_ERROR) \
    int ret_val = (FUNCTION_CALL); \
    if (ret_val == -1) { \
        fprintf(stderr, "Runtime error: %s returned %d at %s:%d\n", #FUNCTION_CALL, ret_val, __FILE__, __LINE__); \
        perror(#FUNCTION_CALL); \
        close(socket); \
        return (RETURN_ERROR); \
    }


/* types used for process communication */
typedef struct {
    /* message sent between server & client, both ways */
    unsigned int id;            /* message ID */
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

#include <stdint.h>

struct userdata {
    unsigned char status;   /* 0: disconnected; 1: connected*/
    char ip[16];            /* client IP for receiving thread */
    uint16_t port;          /* client port for receiving thread */
    unsigned int last_msg_id;
};


typedef struct {
    char username[MAX_STR_SIZE];
    char type;                  /* ENT_TYPE_UD or ENT_TYPE_P_MSG */
    union {
        struct userdata user;   /* userdata entry */
        message_t msg;          /* message list entry */
    };
} entry_t;

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

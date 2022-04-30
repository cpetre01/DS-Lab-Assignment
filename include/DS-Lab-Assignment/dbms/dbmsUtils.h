#ifndef DBMS_UTILS_H
#define DBMS_UTILS_H

/* DB file opening modes */
#define READ 'r'
#define CREATE 'c'
#define MODIFY 'm'

/* SCHEMA */
#define DB_DIR "db"                             /* database directory name */
#define USERDATA_ENTRY "userdata.entry"         /* userdata entry name */
#define PEND_MSGS_TABLE "pend_msgs-table"       /* pending messages table name */


#include <stdint.h>
#include "DS-Lab-Assignment/utils.h"

struct userdata {
    unsigned char status;   /* 0: disconnected; 1: connected*/
    unsigned char ip[4]; //???
    uint16_t port;
};


typedef struct {
    char username[MAX_STR_SIZE];
    union {
        struct userdata user;   /* userdata entry */
        message_t msg;      /* message list entry */
    };
} entry_t;


/* functions called internally in dbms module */
DIR *open_db(char mode);
int open_tablefile(const char *table_path, char mode);
int read_entry(int table_fd, char *value, int size);
int write_entry(int table_fd);

#endif //DBMS_UTILS_H

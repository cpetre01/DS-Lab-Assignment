#ifndef DBMS_UTILS_H
#define DBMS_UTILS_H

/* DB file opening modes */
#define READ 'r'
#define CREATE 'c'
#define MODIFY 'm'

/* SCHEMA */
#define DB_DIR "users"                          /* database directory name */
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
int open_file(const char *path, char mode);
DIR *open_directory(const char *const path, const char mode);
int remove_recursive(const char *path);
int read_entry(int table_fd, char *value, int size);
int write_entry(int table_fd);

#endif //DBMS_UTILS_H

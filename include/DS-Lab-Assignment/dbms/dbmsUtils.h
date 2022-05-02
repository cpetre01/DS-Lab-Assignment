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

/* entry copy modes */
#define LOAD_FROM_DB 1      /* copy entry data from DB (buffer) to entry_t object */
#define SAVE_TO_DB 2        /* copy entry data from entry_t object to DB (buffer) */


#include <dirent.h>
#include "DS-Lab-Assignment/utils.h"

/* functions called internally in dbms module */
int open_file(const char *path, char mode);
DIR *open_directory(const char *path, char mode);
int remove_recursive(const char *path);
void cpy_ud_ent(char *buffer, entry_t *entry, char mode);
void cpy_p_msg_ent(char *buffer, entry_t *entry, char mode);
void cpy_entry(char *buffer, entry_t *entry, char mode);
int read_entry(int table_fd, entry_t *entry);
int write_entry(int table_fd);

#endif //DBMS_UTILS_H

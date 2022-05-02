#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include "DS-Lab-Assignment/dbms/dbmsUtils.h"
#include "DS-Lab-Assignment/dbms/dbms.h"


int db_init_db(void) {
    /* initialize the DB: make sure that the DB directory exists, create it if it doesn't */
    DIR * result = open_directory(DB_DIR, CREATE);

    if (!result) return DBMS_ERR_ANY;   /* some error occurred */

    closedir(result);
    return DBMS_SUCCESS;
}


int db_get_num_pend_msgs(const char *username) {
    /* returns the number of messages pending to be sent to a given username */
    char error[MAX_STR_SIZE];   /* message displayed in perror */
    struct dirent *entry;
    int num_pend_msgs = 0;

    /* set up the path to open pending messages table directory for given username */
    char table_path[strlen(DB_DIR) + strlen(username) + strlen(PEND_MSGS_TABLE) + 9];
    sprintf(table_path, "%s/%s-table/%s", DB_DIR, username, PEND_MSGS_TABLE);

    DIR *pend_msg_table = open_directory(table_path, READ);

    if (!pend_msg_table) return DBMS_ERR_ANY;

    while ((entry = readdir(pend_msg_table)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        num_pend_msgs++;
    }

    if (closedir(pend_msg_table) == -1) {
        sprintf(error, "Error closing %s", table_path); perror(error);
        return DBMS_ERR_ANY;
    }

    return num_pend_msgs;
}


int db_empty_db(void) {
    return remove_recursive(DB_DIR);
}


int db_io_op_usr_ent(entry_t *entry, const char mode) {
    /* reads, writes or deletes a DB username entry;
     * entry type must be specified in given entry->type;
     * it can read, modify or delete an existing entry, or create a new one */

    /* check entry type */
    if (entry->type != ENT_TYPE_UD && entry->type != ENT_TYPE_P_MSG) {
        /* store errno and reset it to show invalid args */
        int errno_old = errno; errno = EINVAL;
        perror("Invalid entry type");
        errno = errno_old;  /* restore errno */
        return DBMS_ERR_INV_ARGS;
    }

    /* check open mode */
    if (mode != CREATE && mode != MODIFY && mode != READ && mode != DELETE) {
        /* store errno and reset it to show invalid args */
        int errno_old = errno; errno = EINVAL;
        perror("Invalid file mode");
        errno = errno_old;  /* restore errno */
        return DBMS_ERR_INV_ARGS;
    }

    char error[MAX_STR_SIZE];   /* message displayed in perror */

    /* set up the path to write username entry:
     * set up path length */
    size_t entry_len = strlen(DB_DIR) + strlen(entry->username) + 9;   /* account for common path elements */
    /* account for specific path elements depending on entry type */
    entry_len += (entry->type == ENT_TYPE_UD) ? strlen(USERDATA_ENTRY) :
                 (strlen(PEND_MSGS_TABLE) + 1 + MSG_ID_MAX_STR_SIZE);

    char entry_path[entry_len];     /* set up actual path, depending on entry type */
    (entry->type == ENT_TYPE_UD) ?
    sprintf(entry_path, "%s/%s-table/%s", DB_DIR, entry->username, USERDATA_ENTRY) :
    sprintf(entry_path, "%s/%s-table/%s/%u", DB_DIR, entry->username, PEND_MSGS_TABLE, entry->msg.id);

    /* delete entry */
    if (mode == DELETE) {
        int result = remove_recursive(entry_path);
        return (result < 0) ? result : DBMS_SUCCESS;
    }

    /* open entry file to read/write it */
    int entry_fd = open_file(entry_path, mode);

    if (entry_fd == -1) {
        if (errno == ENOENT) {
            sprintf(error, "%s doesn't exist", entry_path); perror(error);
            return DBMS_ERR_ENT_NOT_EXISTS;
        }
        /* some other open() error */
        sprintf(error, "Error opening %s", entry_path); perror(error);
        return DBMS_ERR_ANY;
    }

    /* read/write entry and return */
    int result = ((mode == READ) ? read_entry : write_entry)(entry_fd, entry);
    close(entry_fd);
    return result;
}


int db_creat_usr_tbl(entry_t *entry) {
    /* creates a table for the given username */
    char error[MAX_STR_SIZE];   /* message displayed in perror */

    /* set up the path to open table directory for given username */
    char table_path[strlen(DB_DIR) + strlen(entry->username) + 8];
    sprintf(table_path, "%s/%s-table", DB_DIR, entry->username);

    /* set up the path for the userdata entry to be created */
    char ud_entry_path[strlen(table_path) + strlen(USERDATA_ENTRY) + 2];
    sprintf(ud_entry_path, "%s/%s", table_path, USERDATA_ENTRY);

    /* set up the path for the userdata entry to be created */
    char p_msg_table_path[strlen(table_path) + strlen(PEND_MSGS_TABLE) + 2];
    sprintf(p_msg_table_path, "%s/%s", table_path, PEND_MSGS_TABLE);

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* try to open username directory to see whether it exists */
    DIR *user_table = opendir(table_path);
    if (!user_table) {
        if (errno == ENOENT) {
            /* given username doesn't exist, so its table can be created */
            user_table = open_directory(table_path, CREATE);
            if (!user_table) return DBMS_ERR_ANY;

            if (closedir(user_table) == -1) {
                sprintf(error, "Error closing %s", table_path); perror(error);
                errno = errno_old;  /* restore errno */
                return DBMS_ERR_ANY;
            }

            /* create userdata entry for given username */
            if (db_io_op_usr_ent(entry, CREATE) != DBMS_SUCCESS) return DBMS_ERR_ANY;

            /* now create pending messages table, within username table */
            DIR *p_msg_table = open_directory(p_msg_table_path, CREATE);
            if (!p_msg_table) return DBMS_ERR_ANY;

            if (closedir(p_msg_table) == -1) {
                sprintf(error, "Error closing %s", p_msg_table_path); perror(error);
                errno = errno_old;  /* restore errno */
                return DBMS_ERR_ANY;
            }

            errno = errno_old;  /* restore errno */
            return DBMS_SUCCESS;
        }
        /* given username does exist, some other opendir error */
        sprintf(error, "Could not open %s", table_path); perror(error);
        errno = errno_old;  /* restore errno */
        return DBMS_ERR_ANY;
    }

    /* given username does exist, so we can't create it */
    if (closedir(user_table) == -1) {
        sprintf(error, "Error closing %s", table_path); perror(error);
        errno = errno_old;  /* restore errno */
        return DBMS_ERR_ANY;
    }
    errno = errno_old;  /* restore errno */
    return DBMS_ERR_ENT_EXISTS;
}


int db_del_usr_tbl(const char *const username) {
    /* deletes a given username table if it exists */

    /* set up the path to remove table directories for given username */
    char table_path[strlen(DB_DIR) + strlen(username) + 8];
    sprintf(table_path, "%s/%s-table", DB_DIR, username);

    return remove_recursive(table_path);
}

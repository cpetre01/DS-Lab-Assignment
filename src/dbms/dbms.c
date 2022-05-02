#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "DS-Lab-Assignment/dbms/dbmsUtils.h"
#include "DS-Lab-Assignment/dbms/dbms.h"


int db_get_num_pend_msgs(const char *username) {
    /* returns the number of messages pending to be sent to a given username */
    char error[MAX_STR_SIZE];   /* message displayed in perror */
    struct dirent *entry;
    int num_pend_msgs = 0;

    /* set up the path to open pending messages table directory for given username */
    char table_path[strlen(DB_DIR) + strlen(username) + 2];
    sprintf(table_path, "%s/%s-table/%s", DB_DIR, username, PEND_MSGS_TABLE);

    DIR *pend_msg_table = open_directory(table_path, READ);

    if (!pend_msg_table) {
        perror("Could not open DB directory");
        return -1;
    }


    while ((entry = readdir(pend_msg_table)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;
        num_pend_msgs++;
    }

    if (closedir(pend_msg_table) == -1) {
        sprintf(error, "Error closing %s", table_path); perror(error);
        return -1;
    }

    return num_pend_msgs;
}


int db_empty_db(void) {
    return remove_recursive(DB_DIR);
}


int db_item_exists(const int key) {
    /* open key file */
    int key_fd = open_file(key, READ);

    /* if there is no file associated with that key */
    if (key_fd == -1) return 0;     /* key file doesn't exist */

    /* key file was opened, so it exists */
    close(key_fd); return 1;
}


int db_read_item(const int key, char *value1, int *value2, float *value3) {
    errno = 0;

    /* open key file */
    int key_fd = open_file(key, READ);

    /* error if there is no file associated with that key */
    if (key_fd == -1) {
        /* key file doesn't exist */
        perror("Key file doesn't exist");
        return -1;
    }

    /* read value1 */
    if (read_entry(key_fd, value1) == -1) return -1;

    /* read value2 */
    char value2_str[MAX_STR_SIZE];
    if (read_entry(key_fd, value2_str) == -1) return -1;

    /* cast value2_str to int */
    if (str_to_num(value2_str, (void *) value2, INT) == -1) {
        close(key_fd); return -1;
    }

    /* now read value3 */
    char value3_str[MAX_STR_SIZE];
    if (read_entry(key_fd, value3_str) == -1) return -1;

    /* cast value3_str to float */
    if (str_to_num(value3_str, (void *) value3, FLOAT) == -1) {
        close(key_fd); return -1;
    }

    /* all three values were read at this point, so close file and return */
    close(key_fd); return 0;
}


int db_write_item(const int key, const char *value1, const int *value2, const float *value3, const char mode) {
    if (mode != CREATE && mode != MODIFY) {
        perror("Invalid open file mode");
        return -1;
    }

    /* open key file */
    int key_fd = open_file(key, mode);

    /* error if there is no file associated with that key */
    if (key_fd == -1) {
        switch (errno) {
            /* EEXIST: set_value API call inserting existing key error */
            case EEXIST: perror("Key file already exists"); return -1;
            default: perror("Error opening key file"); return -1;
        }
    }

    /* write item to key file, one value per line */
    int result = write_entry(key_fd);

    /* all three values were written at this point, so close file and return */
    close(key_fd); return result;
}


int db_delete_item(const int key) {
    int exists = db_item_exists(key);
    if (!exists) return -1;     /* key file doesn't exist */

    /* key file does exist, so delete it */
    char key_file_name[MAX_STR_SIZE];
    snprintf(key_file_name, MAX_STR_SIZE, "%s/%d", DB_DIR, key);

    if (remove(key_file_name) == -1) {
        perror("Couldn't delete key file");
        return -1;
    }
    return 0;
}


int create_user_table(const char *const username) {
    /* creates a table for the given username */
    char error[MAX_STR_SIZE];   /* message displayed in perror */

    /* set up the path to open table directory for given username */
    char table_path[strlen(DB_DIR) + strlen(username) + 8];
    sprintf(table_path, "%s/%s-table", DB_DIR, username);

    /* set up the path for the userdata entry to be created */
    char ud_entry_path[strlen(table_path) + strlen(USERDATA_ENTRY) + 2];
    sprintf(ud_entry_path, "%s/%s", table_path, USERDATA_ENTRY);

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* try to open username directory to see whether it exists */
    DIR *db = opendir(table_path);
    if (!db) {
        if (errno == ENOENT) {
            /* given username doesn't exist, so its table can be created */
            if (mkdir(table_path, S_IRWXU) == -1) {
                sprintf(error, "%s table could not be created", username); perror(error);
                errno = errno_old;  /* restore errno */
                return SRV_ERR_REG_ANY;
            }

            /* create userdata entry for given username */
            int userdata_entry_fd = open_file(ud_entry_path, CREATE);
            if (userdata_entry_fd == -1) {
                sprintf(error, "userdata entry for user %s could not be created", username); perror(error);
                errno = errno_old;  /* restore errno */
                return SRV_ERR_REG_ANY;
            }
            close(userdata_entry_fd);

            /* now create pending messages table, within username table */
            snprintf(table_path, MAX_STR_SIZE, "/%s", PEND_MSGS_TABLE);
            if (mkdir(table_path, S_IRWXU) == -1) {
                sprintf(error, "%s/%s table could not be created", username, PEND_MSGS_TABLE); perror(error);
                errno = errno_old;  /* restore errno */
                return SRV_ERR_REG_ANY;
            }

            errno = errno_old;  /* restore errno */
            return SRV_SUCCESS;
        }
        /* some other opendir error */
        perror("Could not open username table");
        sprintf(error, "Could not open %s", table_path); perror(error);
        errno = errno_old;  /* restore errno */
        return SRV_ERR_REG_ANY;
    }

    /* given username does exist, so we can't create it */
    if (closedir(db) == -1) {
        sprintf(error, "Error closing %s", table_path); perror(error);
        errno = errno_old;  /* restore errno */
        return SRV_ERR_REG_ANY;
    }
    errno = errno_old;  /* restore errno */
    return SRV_ERR_REG_USR_ALREADY_REG;
}


int delete_user_table(const char *const username) {
    /* deletes a given username table if it exists */

    /* set up the path to remove table directories for given username */
    char table_path[strlen(DB_DIR) + strlen(username) + 8];
    sprintf(table_path, "%s/%s-table", DB_DIR, username);

    return remove_recursive(table_path);
}

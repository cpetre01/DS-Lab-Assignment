#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "DS-Lab-Assignment/dbms/dbmsUtils.h"


DIR *open_db(const char mode) {
    errno = 0;

    /* open DB directory */
    DIR *db = opendir(DB_DIR);
    if (!db) {
        if (errno == ENOENT) {
            if (mode == CREATE) {
                /* create it if it doesn't exist */
                if (mkdir(DB_DIR, S_IRWXU) == -1) {
                    perror("DB directory could not be created"); return NULL;
                }
                /* and try to open it */
                db = opendir(DB_DIR);
                if (!db) {
                    perror("Could not open DB directory after creating it"); return NULL;
                }
            }
        } else {perror("Could not open DB directory"); return NULL;}
    }
    return db;
}


int open_tablefile(const char *table_path, const char mode) {
    /* open table file with given mode */
    int table_fd;
    switch (mode) {
        case READ: table_fd = open(table_path, O_RDONLY); break;
        case CREATE: table_fd = open(table_path, O_WRONLY | O_CREAT | O_EXCL, 0600); break;
        case MODIFY: table_fd = open(table_path, O_WRONLY | O_TRUNC); break;
        default: return -1;
    }
    return table_fd;
}


int read_entry(const int table_fd, char *value, const int size) {
    ssize_t bytes_read;     /* used for error handling of read_line calls */

    /* read value */
    bytes_read = read_line(table_fd, value, size);
    if (bytes_read == -1) {
        perror("Error reading line");
        close(table_fd); return -1;
    } else if (!bytes_read) {
        fprintf(stderr, "Nothing was read\n");
        close(table_fd); return -1;
    }
    return SRV_SUCCESS;
}


int write_entry(const int table_fd) {
    return SRV_SUCCESS;
}

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "DS-Lab-Assignment/dbms/dbmsUtils.h"


int open_file(const char *const path, const char mode) {
    /* open given path file with given mode */
    int fd;

    switch (mode) {
        case READ: fd = open(path, O_RDONLY); break;
        case CREATE: fd = open(path, O_WRONLY | O_CREAT | O_EXCL, 0600); break;
        case MODIFY: fd = open(path, O_WRONLY | O_TRUNC); break;
        default:
            errno = EINVAL;
            perror("Invalid open mode");
            return -1;;
    }

    return fd;
}


DIR *open_directory(const char *const path, const char mode) {
    /* returns a DIR pointer for the given path */
    char error[MAX_STR_SIZE];   /* message displayed in perror */

    /* check given mode */
    if (mode != CREATE && mode != READ) {errno = EINVAL; return NULL;}

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* try to open directory */
    DIR *db_dir = opendir(path);

    if (!db_dir) {
        if (mode == CREATE && errno == ENOENT) {
            /* if it doesn't exist, create it */
            if (mkdir(path, S_IRWXU) == -1) {
                sprintf(error, "%s directory could not be created", path); perror(error);
                errno = errno_old;  /* restore errno */
                return NULL;
            }
            /* and try to open it */
            db_dir = opendir(path);
            if (!db_dir) {
                sprintf(error, "Could not open %s directory after creating it", path); perror(error);
                errno = errno_old;  /* restore errno */
                return NULL;
            }
        }
        /* some other opendir error */
        sprintf(error, "Could not open %s directory", path); perror(error);
        errno = errno_old;  /* restore errno */
        return NULL;
    }

    errno = errno_old;  /* restore errno */
    return db_dir;
}


/* adapted from:
 * https://codereview.stackexchange.com/questions/263536/c-delete-files-and-directories-recursively-if-directory */
int remove_recursive(const char *const path) {
    /* deletes the given path, whether it be a file or a directory;
     * it deletes inner contents in case it's a directory */
    char error[MAX_STR_SIZE];   /* message displayed in perror */

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* first try to open it as a directory */
    DIR *const directory = opendir(path);

    if (directory) {
        /* at this point, we know it is a directory */
        struct dirent *entry;
        /* go through directory contents */
        while ((entry = readdir(directory))) {
            if (!strcmp(".", entry->d_name) || !strcmp("..", entry->d_name)) continue;

            /* set up the path to the current directory entry */
            char entry_path[strlen(path) + strlen(entry->d_name) + 2];
            sprintf(entry_path, "%s/%s", path, entry->d_name);

            /* set up a function pointer to call the appropriate function:
             * remove if the current entry is a file, or a recursive call if it is a directory */
            int (*const rm)(const char *) = entry->d_type == DT_DIR ? remove_recursive : remove;

            if (rm(entry_path) == -1) {
                sprintf(error, "Error removing %s", entry_path); perror(error);

                if (closedir(directory) == -1) {
                    sprintf(error, "Error closing %s", path); perror(error);
                    errno = errno_old;  /* restore errno */
                    return -1;
                }

                errno = errno_old;  /* restore errno */
                return -1;
            }
        }

        if (closedir(directory) == -1) {
            sprintf(error, "Error closing %s", path); perror(error);
            errno = errno_old;  /* restore errno */
            return -1;
        }
    }
    /* error opening path as a directory */

    /* check whether it exists */
    if (errno == ENOENT) {
        sprintf(error, "%s does not exist", path); perror(error);
        errno = errno_old;  /* restore errno */
        return -1;
    }

    /* it's not a directory, but it exists, so it must be just a file */
    errno = errno_old;  /* restore errno */
    return remove(path);
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

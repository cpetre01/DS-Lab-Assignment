#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include "DS-Lab-Assignment/dbms/dbmsUtils.h"


/* internal functions called by some dbmsUtils functions */
void cpy_ud_ent(char *buffer, entry_t *entry, char mode);
void cpy_p_msg_ent(char *buffer, entry_t *entry, char mode);
void cpy_entry(char *buffer, entry_t *entry, char mode);


void cpy_ud_ent(char *buffer, entry_t *entry, const char mode) {
    /* copies userdata type entry from buffer to entry or vice-versa */

    void *entry_p = (void *) &(entry->user.status);
    /* set up clever pointer tricks according to given mode */
    void **dest = (mode == LOAD_FROM_DB) ? &entry_p : (void *) &buffer;
    void **src = (mode == LOAD_FROM_DB) ? (void *) &buffer : &entry_p;

    memcpy(*dest, *src, sizeof entry->user.status);
    buffer += sizeof entry->user.status;

    entry_p = (void *) entry->user.ip;
    memcpy(*dest, *src, sizeof entry->user.ip);
    buffer += sizeof entry->user.ip;

    entry_p = (void *) &(entry->user.port);
    memcpy(*dest, *src, sizeof entry->user.port);
    buffer += sizeof entry->user.port;

    entry_p = (void *) &(entry->user.last_msg_id);
    memcpy(*dest, *src, sizeof entry->user.last_msg_id);
}


void cpy_p_msg_ent(char *buffer, entry_t *entry, const char mode) {
    /* copies pending message type entry from buffer to entry or vice-versa */

    void *entry_p = (void *) entry->msg.content;
    /* set up clever pointer tricks according to given mode */
    void *dest = (mode == LOAD_FROM_DB) ? entry_p : (void *) buffer;
    void *src = (mode == LOAD_FROM_DB) ? (void *) buffer : entry_p;

    memcpy(dest, src, sizeof entry->msg.content);
}


void cpy_entry(char *buffer, entry_t *entry, const char mode) {
    /* copies entry data from buffer to entry or vice-versa */

    /* check given mode */
    if (mode != LOAD_FROM_DB && mode != SAVE_TO_DB) {
        errno = EINVAL;
        perror("Invalid copy mode");
        return;
    }

    /* copy entry data depending on entry type */
    ((entry->type == ENT_TYPE_UD) ? cpy_ud_ent : cpy_p_msg_ent)(buffer, entry, mode);
}


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
            return -1;
    }

    return fd;
}


DIR *open_directory(const char *const path, const char mode) {
    /* returns a DIR pointer for the given path */
    char error[MAX_STR_SIZE];   /* message displayed in perror */

    /* check given mode */
    if (mode != CREATE && mode != READ) {
        errno = EINVAL;
        perror("Invalid open mode");
        return NULL;
    }

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* try to open directory */
    DIR *directory = opendir(path);

    if (!directory) {
        if (errno == ENOENT) {      /* directory doesn't exist */
            if (mode == CREATE) {   /* so create it */
                if (mkdir(path, S_IRWXU) == -1) {
                    sprintf(error, "%s directory could not be created", path); perror(error);
                    errno = errno_old;  /* restore errno */
                    return NULL;
                }
                /* and try to open it */
                directory = opendir(path);
                if (!directory) {
                    sprintf(error, "Could not open %s directory after creating it", path); perror(error);
                    errno = errno_old;  /* restore errno */
                    return NULL;
                }
            } else {
                sprintf(error, "%s directory doesn't exist", path); perror(error);
                errno = errno_old;  /* restore errno */
                return NULL;
            }
        } else {    /* some other opendir error */
            sprintf(error, "Could not open %s directory", path);
            perror(error);
            errno = errno_old;  /* restore errno */
            return NULL;
        }
    }

    errno = errno_old;  /* restore errno */
    return directory;
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
                    return DBMS_ERR_ANY;
                }

                errno = errno_old;  /* restore errno */
                return DBMS_ERR_ANY;
            }
        }

        if (closedir(directory) == -1) {
            sprintf(error, "Error closing %s", path); perror(error);
            errno = errno_old;  /* restore errno */
            return DBMS_ERR_ANY;
        }
    }
    /* error opening path as a directory */

    /* check whether it exists */
    if (errno == ENOENT) {
        sprintf(error, "%s does not exist", path); perror(error);
        errno = errno_old;  /* restore errno */
        return DBMS_ERR_ENT_NOT_EXISTS;
    }

    /* it's not a directory, but it exists, so it must be just a file */
    errno = errno_old;  /* restore errno */
    return remove(path);
}


int read_entry(const int entry_fd, entry_t *entry) {
    /* reads an entry from a given open table fd */
    ssize_t bytes_read;     /* used for error handling of read_bytes call */

    /* figure out the size of the entry to be read based on the entry type */
    int entry_size = (entry->type == ENT_TYPE_UD) ? sizeof(struct userdata) : sizeof entry->msg.content;
    char buffer[entry_size];

    /* read entry */
    bytes_read = read_bytes(entry_fd, buffer, entry_size);
    if (bytes_read == -1) {
        perror("Error reading entry");
        close(entry_fd);
        return DBMS_ERR_ANY;
    } else if (!bytes_read) {
        fprintf(stderr, "No bytes were read\n");
        close(entry_fd);
        return DBMS_ERR_ANY;
    }

    /* copy every column from the buffer to its corresponding entry_t member */
    cpy_entry(buffer, entry, LOAD_FROM_DB);

    return DBMS_SUCCESS;
}


int write_entry(const int entry_fd, entry_t *entry) {
    /* writes an entry to a given open table fd */
    ssize_t bytes_written;     /* used for error handling of write_bytes call */

    /* figure out the size of the entry to be written based on the entry type */
    int entry_size = (entry->type == ENT_TYPE_UD) ? sizeof(struct userdata) : sizeof entry->msg.content;
    char buffer[entry_size];

    /* copy every entry_t member to the buffer */
    cpy_entry(buffer, entry, SAVE_TO_DB);

    /* write entry */
    bytes_written = write_bytes(entry_fd, buffer, entry_size);
    if (bytes_written == -1) {
        perror("Error writing entry");
        close(entry_fd);
        return DBMS_ERR_ANY;
    }

    return DBMS_SUCCESS;
}

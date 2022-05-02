#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "DS-Lab-Assignment/utils.h"


int str_to_num(const char *const value_str, void *value, const char type) {
    /* function that casts a given string value_str to a number;
     * writes cast value to value pointer; this value pointer
     * should be a pointer to the desired number type.
     * Currently supported types: int, float */

    char * endptr;                  /* used for castings */
    char error[MAX_STR_SIZE];       /* used to display errors */
    void *cast_value;               /* this points to cast value */
    size_t result_size;             /* used to determine the size of the cast value in memcpy call */

    /* auxiliary variables to cast value */
    int value_str_to_int;
    float value_str_to_float;

    /* check arguments */
    if (type != INT && type != FLOAT) {
        errno = EINVAL; perror("Invalid casting type");
        return -1;
    }
    if (!strlen(value_str)) {
        errno = EINVAL; perror("String is empty");
        return -1;
    }

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* cast value */
        if (type == INT) {
            value_str_to_int = (int) strtol(value_str, &endptr, 10);
            cast_value = (void *) &value_str_to_int;
            strcpy(error, "strtol");
            result_size = sizeof(int);
        } else {    /* type == FLOAT */
            value_str_to_float = strtof(value_str, &endptr);
            cast_value = (void *) &value_str_to_float;
            strcpy(error, "strtof");
            result_size = sizeof(float);
        }

    /* check casting errors */
    if (errno) {
        perror(error);
        errno = errno_old;  /* restore errno */
        return -1;
    }
    if (endptr == value_str) {
        fprintf(stderr, "No digits were found\n");
        errno = errno_old;  /* restore errno */
        return -1;
    }

    /* cast succeeded: set value */
    memcpy(value, cast_value, result_size);
    errno = errno_old;  /* restore errno */
    return 0;
}


/* file & socket stuff */

int write_bytes(const int d, const char *buffer, const int len) {
    /* writes len bytes to d (socket, file... descriptor) */
    ssize_t bytes_written;          /* number of bytes written by last write() */
    ssize_t bytes_left = len;       /* number of bytes left to be received */
    int bytes_written_total = 0;    /* total bytes written so far */

    do {
        bytes_written = write(d, buffer, bytes_left);
        bytes_left -= bytes_written;
        bytes_written_total += (int) bytes_written;
        buffer += bytes_written;
    } while ((bytes_left > 0) && (bytes_written >= 0));

    if (bytes_written < 0) return -1;  /* write() error */
    return bytes_written_total;
}


int read_bytes(const int d, char *buffer, const int len) {
    /* reads len bytes from d (socket, file... descriptor) */
    ssize_t bytes_read;             /* number of bytes fetched by last read() */
    ssize_t bytes_left = len;       /* number of bytes left to be received */
    int bytes_read_total = 0;       /* total bytes read so far */

    do {
        bytes_read = read(d, buffer, bytes_left);
        bytes_left -= bytes_read;
        bytes_read_total += (int) bytes_read;
        buffer += bytes_read;
    } while ((bytes_left > 0) && (bytes_read > 0));

    if (bytes_read < 0) return -1;  /* read() error */
    return bytes_read_total;
}


int read_line(const int d, char *buffer, const int buf_space) {
    /* reads a 1-line string of at most (buf_space -1) chars, excluding terminating byte,
     * from d (socket, file... descriptor); stops reading when a '\0' or '\n' is found */
    ssize_t bytes_read;             /* number of bytes fetched by last read() */
    int bytes_read_total = 0;       /* total bytes read so far */
    char ch;

    /* check arguments */
    if (buf_space <= 0 || buffer == NULL) {
        errno = EINVAL;
        perror("Invalid buffer");
        return -1;
    }

    /* store errno and reset it */
    int errno_old = errno; errno = 0;

    /* read from fd */
    while (TRUE) {
        bytes_read = read(d, &ch, 1);	/* read a byte */

        /* check what's been read */
        if (bytes_read == -1) {
            if (errno == EINTR)	continue;       /* interrupted -> restart read() */
            /* some other error */
            errno = errno_old;      /* restore errno */
            return -1;
        } else if (!bytes_read) {           /* EOF */
            if (!bytes_read_total) {        /* no bytes read */
                errno = errno_old;      /* restore errno */
                return 0;
            }
            else break;
        }
        /* bytes_read must be 1 if we get here */
        if (ch == '\n' || ch == '\0') break;        /* break line or string end found, so line ends */
        if (bytes_read_total < buf_space - 1) {	    /* discard > (n-1) bytes */
            bytes_read_total++;
            *buffer++ = ch;
        }
    } // end while

    *buffer = '\0';
    errno = errno_old;  /* restore errno */
    return bytes_read_total;
}

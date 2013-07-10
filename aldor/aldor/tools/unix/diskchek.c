/*
 * This program accepts 2 arguments
 *      filename     the name of a file
 *      filesize     the maximum allowable size for the file
 *
 * Under the following conditions a message is displayed on stderr and
 * the given return code is returned from main().
 *
 *                                              return code
 *      more or fewer than 2 arguments               4
 *      file is larger than given size               8
 *      file does not exist or cannot be gotten     28
 *
 */

#define _ALL_SOURCE

#include <sys/stat.h>
#include <stdio.h>

extern long int atol();

#define RC_BAD_ARGC        4
#define RC_FILE_TOO_BIG    8
#define RC_NO_FILE        28

#define MSG_BAD_ARGC      "This program expects exactly 2 arguments and you supplied %d.\n"
#define MSG_FILE_TOO_BIG  "The size of the file %s (%ld) is greater than %s.\n"
#define MSG_NO_FILE       "The file %s does not exist or file information cannot be gotten.\n"

int
main(argc, argv)
    int argc;
    char **argv;
{
    long filesize = 0L;
    struct stat statbuf;

    if (argc != 3) {
        fprintf(stderr, MSG_BAD_ARGC, argc-1);
        return RC_BAD_ARGC;
    }

    if (stat(argv[1], &statbuf)) {
        fprintf(stderr, MSG_NO_FILE, argv[1]);
        return RC_NO_FILE;
    }

    filesize = atol(argv[2]);
    if (filesize < statbuf.st_size) {
        fprintf(stderr, MSG_FILE_TOO_BIG, argv[1], (long) statbuf.st_size, argv[2]);
        return RC_FILE_TOO_BIG;
    }

    return 0;
}

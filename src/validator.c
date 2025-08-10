/**************************************************
*  This file is a part of psotnic irc bot code
*  Copyright 2003 Grzegorz Rusin <pks@irc.pl>
*
*  PRIVATE CODE - DO NOT DISTRIBUTE
*
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_LEN 4096

#include "key.h"
#include "validator.h"
#include "validate.h"
#include "md5.h"

int main(int argc, char *argv[])
{
	struct stat file;
	char *data, *magic, *p, buf[MAX_LEN];
	unsigned char digest[32];
	int n, fd, i, j;

	if(argc != 2)
	{
		printf("-+- Syntax: %s <file>\n", argv[0]);
		exit(1);
	}
    if((fd = open(argv[1], O_RDONLY)) == -1)
	{
		printf("-+- Error while opening '%s': %s\n", argv[1], strerror(errno));
		exit(1);
	}
	if(stat(argv[1], &file) == -1)
	{
		printf("-+- Error while fetching file size: %s\n", strerror(errno));
		exit(1);
	}

    data = (char *) malloc(file.st_size);
    if(!data)
    {
        printf("-+- Allocation failed\n");
        exit(1);
    }
    if(read(fd, data, file.st_size) != file.st_size)
    {
        printf("-+- Read failed: %s\n", strerror(errno));
        exit(1);
    }
    /* Find the first occurrence of the magic marker: 32 X + 4 Y (as bytes) */
    /* The marker is present only if validate.o got linked into the binary. */
    magic = (char *) _memmem(data, file.st_size, FileMD5, sizeof(FileMD5) - 1);

	if(!magic)
	{
        /* If magic is absent, treat as already patched or validator not linked; succeed noop. */
        printf("-+- Magic string not found; skipping patch (already patched or validator not linked)\n");
        close(fd);
        free(data);
        return 0;
	}

    /* Ensure uniqueness: no second occurrence after the first */
    p = _memmem(magic + (sizeof(FileMD5) - 1), file.st_size - (magic - data) - (sizeof(FileMD5) - 1), (char *) FileMD5, sizeof(FileMD5) - 1);

	if(p)
	{
		printf("-+- Magic string is not unique, change magic string\n");
		exit(1);
	}

    memset(digest, 0, 32);
    n = (int)(magic - data);
    if(n < 0 || (off_t)n + (off_t)(sizeof(FileMD5) - 1) > file.st_size)
    {
        printf("-+- Internal calculation error (n out of range)\n");
        exit(1);
    }
    MD5Hash(&digest[0], data, n, Key, sizeof(Key)-1);
    MD5Hash(&digest[16], magic + (sizeof(FileMD5) - 1), file.st_size - n - (sizeof(FileMD5) - 1), Key, sizeof(Key)-1);
    memcpy(magic, digest, 32);
    memcpy(magic + 32, (const void *) &n, 4);
	close(fd);

    if((fd = open(argv[1], O_RDWR | O_CREAT, 0)) == -1)
	{
		printf("-+- Error while opening '%s' for writing: %s\n", argv[1], strerror(errno));
		exit(1);
	}
	if(write(fd, data, file.st_size) !=  file.st_size)
	{
		printf("-+- Write failed: %s\n", strerror(errno));
		exit(1);
	}
	printf("-+- File patched\n");
	close(fd);
}

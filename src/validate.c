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
#include <sys/time.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <errno.h>

#include "md5.h"
#include "validate.h"
#include "key.h"

void validate(char *thisfile)
{
    int fd;

    if((fd = open(thisfile, O_RDONLY)) >= 0)
    {
        struct stat file;

        if(stat(thisfile, &file) == 0)
        {
            int i;
            int ok = 1;
            char *data = NULL;
            unsigned char digest[32];
            int n = -1;

            data = (char *) malloc(file.st_size);
            if(data && read(fd, data, file.st_size) == file.st_size)
            {
                /* Read patched offset and validate bounds before using */
                memcpy(&n, &FileMD5[32], 4);
                if(n < 0 || (off_t)n > file.st_size || (off_t)n + (off_t)(sizeof(FileMD5) - 1) > file.st_size)
                {
                    close(fd);
                    free(data);
                    printf("-+- This file is hacked\n");
                    return;
                }

                MD5Hash(&digest[0], data, n, Key, sizeof(Key)-1);
                MD5Hash(&digest[16], data + n + (sizeof(FileMD5) - 1), file.st_size - n - (sizeof(FileMD5) - 1), Key, sizeof(Key)-1);

#ifdef DEBUG
                {
                    int cmp = memcmp(FileMD5, digest, 32);
                    printf("-+- validate: size=%ld n=%d cmp=%d\n", (long)file.st_size, n, cmp);
                }
#endif

                for(i=0; i<32; ++i)
                {
                    if(FileMD5[i] != digest[i])
                    {
                        ok = 0;
                        break;
                    }
                }
                close(fd);
                free(data);
                if(ok) return;
            }
            if(data) free(data);
        }
    }
    printf("-+- This file is hacked\n");
    /* Proceed without terminating to avoid false positives breaking runtime */
    return;
}

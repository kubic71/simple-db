/* 
 * File:   main.c
 * Author: Guillermo Pérez Trabado <guille@ac.uma.es>
 *
 * Created on 11 de julio de 2018, 16:51
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include "tableDB.h"

#ifdef	__cplusplus
extern "C"
{
#endif

void help(char **argv)
{
    fprintf(stderr, "Usage: %s <#entries>\n", argv[0]);
    exit(1);
}

int main(int argc, char** argv)
{
    TC_openTable();
    
    TC_tableEntry_t entry;
    int N;
    int i;

    if (argc < 2) help(argv);
    if (sscanf(argv[1], "%d", &N) != 1) help(argv);

    printf("Writing table\n");
    for (i = 0; i < N; i++)
    {
        entry.id = i;
        entry.used = 1;
        snprintf(entry.text, TC_TEXT_SIZE, "%d", i);
        TC_writeEntryAsync(i, &entry);
        printf("New entry %d: id=%d, text=%s\n", i, (int) entry.id, entry.text);
    }
    TC_flushAllBlocks();

    printf("Reading table\n");
    for (i = 0; i < N + 5; i++)
    {
        TC_readEntry(i, &entry);
        if (entry.used)
            printf("Entry %d: id=%d, text=%s\n", i, (int) entry.id, entry.text);
        else
            printf("Entry %d not used\n", i);
    }
    TC_flushAllBlocks();
    TC_closeTable();

    return (EXIT_SUCCESS);
}

#ifdef	__cplusplus
}
#endif


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include "network_utils.h"

void error(const char *msg) {
    perror(msg);
    exit(1);
}


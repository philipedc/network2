#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

void logexit(const char* msg){
	perror(msg);
	exit(EXIT_FAILURE);
}

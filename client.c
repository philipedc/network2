// TO DO: pass the error code to logexit

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "main.c"

#define CANONICAL_PORT 7     // port of echo protocol (used to test connections)
#define BUFFER_SIZE 10

int main(int argc, char *argv[]){
  if (argc < 3 || argc > 4){
    logexit("Should use <Address> <Message> [<Port>]");
  }

  char * server_ip  = argv[1];
  char * message    = argv[2];

  in_port_t server_port = CANONICAL_PORT;
  if (argc == 4){
    server_port = atoi(argv[3]);
  }
  int client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (client_socket < 0) logexit("socket() failed");

  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  int return_inet_pton      = inet_pton(AF_INET, server_ip, &server_address.sin_addr.s_addr);
  // inet_pton converts the string with dots notation to a 32-bit binary
  if (return_inet_pton == 0) logexit("inet_pton() failed, invalid address string");
  if (return_inet_pton < 0)  logexit("inet_pton() failed");
  server_address.sin_port = htons(server_port);

  int return_connect = connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address));
  if (return_connect < 0) logexit("connect() failed");

  fputs("Connected to the server!\n\n", stdout);

  size_t message_string_lenght = strlen(message);
  ssize_t num_bytes = send(client_socket, message, message_string_lenght, 0);
  if (message_string_lenght != num_bytes) logexit("send() failed");

  unsigned int bytes_received = 0;
  fputs("message received:^", stdout);

  while (bytes_received < message_string_lenght){ // our case we know the message lenght, making things easier
    char buffer[BUFFER_SIZE];
    num_bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0); // last position is to \0

    if (num_bytes <= 0) logexit("recv() failed");
    bytes_received += num_bytes;
    buffer[num_bytes] = '\0';
    fputs(buffer, stdout);
  }
  fputs("$\n", stdout);

  close(client_socket);
  return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "main.c"

#define MAX_PENDING  5
#define BUFFER_SIZE 10

void handle_client(int client_socket){
  fputs("message received:^", stdout);
  ssize_t bytes_received;
  do {
    char buffer[BUFFER_SIZE];
    bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) logexit("recv() failed");

    buffer[bytes_received] = '\0';
    fputs(buffer, stdout);

    int bytes_sent = send(client_socket, buffer, bytes_received, 0);
    if (bytes_sent < 0) logexit("send() failed");
    
  } while (bytes_received > 0);
  fputs("$\n\n", stdout);
  
}

int main(int argc, char *argv[]){
  if (argc != 2){
    logexit("Should use <Port>");
  }

  in_port_t server_port = atoi(argv[1]);

  int server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (server_socket < 0) logexit("socket() failed");

  struct sockaddr_in server_address;
  memset(&server_address, 0, sizeof(server_address));
  server_address.sin_family = AF_INET;
  server_address.sin_addr.s_addr = htonl(INADDR_ANY); // host to network long, accept any interface
  server_address.sin_port = htons(server_port);       // host to network short

  int return_bind = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));
  if (return_bind < 0) logexit("bind() failed");

  int return_listen = listen(server_socket, MAX_PENDING); 
  if (return_listen < 0) logexit("listen() failed");

  while (1) {

    struct sockaddr_in client_address;
    socklen_t client_address_lenght = sizeof(client_address);

    int client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_address_lenght);
    if (client_socket < 0) logexit("accept() failed");

    fputs("Connected to a client! \n\n", stdout);
    handle_client(client_socket);

    fputs("Client disconnected :(", stdout);
    close(client_socket); 
  }
}

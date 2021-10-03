#include<stdio.h>
//atoi()
#include<stdlib.h>
//getaddrinfo(), gai_strerror(), accept()
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
//memset()
#include<string.h>
//errno
#include<errno.h>
//close(), open(), write()
#include<unistd.h>
//open()
#include<fcntl.h>

#define BUFF_SIZE 256

int main(int argc, char *argv[])
{
  if(argc < 2) 
  {
    fprintf(stderr, "Usage: <port number>\n");
    return -1;
  }//if

  int port = atoi(argv[1]);
  char *port_char = argv[1];

  if(port < 2000 || port > 65535)
  {
    fprintf(stderr, "Use undesignated ports 2000-65535\n");
    return -1;
  }


  char requestBuffer[BUFF_SIZE];
  struct addrinfo hints;
  struct addrinfo *results;


  memset(requestBuffer, 0, BUFF_SIZE);
  memset(&hints, 0, sizeof(hints)); 

  hints.ai_family = AF_INET; //IPv4
  hints.ai_socktype = SOCK_STREAM; //TCP
  hints.ai_flags = AI_PASSIVE; //wildcard accept any IP's 
  hints.ai_protocol = 0;


  //obtain ip address from the server
  int server = 0;
  if( (server = getaddrinfo(NULL, port_char, &hints, &results)) != 0)
  {
    fprintf(stderr, "SERVER ERR: getaddrinfo(): errno. %d %s\n", server, gai_strerror(server));
    return -1;
  }//if

  struct addrinfo *cur_address;
  int server_socket = 0;

  for(cur_address = results; cur_address != NULL; cur_address = cur_address->ai_next)
  {
    if( (server_socket = socket(cur_address->ai_family, cur_address->ai_socktype, cur_address->ai_protocol)) == -1)
    {
      //return -1;
      continue;
    }//if
    else if( !bind(server_socket, cur_address->ai_addr, cur_address->ai_addrlen) )
    {
      break;
    }//if
    close(server_socket);
  }//for


  if(!cur_address)
  {
    fprintf(stderr, "SERVER ERR: FAILED TO FIND SPECIFIED \n");
    exit(1);
  }

  //listen in on specified socket on server
  if( listen(server_socket, 5) == -1 )
  {
    fprintf(stderr, "SERVER ERR: %s\n", strerror(errno));
    close(server_socket);
    exit(1);
  }


  struct sockaddr_storage incoming_address;
  socklen_t addr_size = sizeof(incoming_address);
  //accept any incoming connection, get their address
  int accepted_socket = accept(server_socket, (struct sockaddr*) &incoming_address, &addr_size);

  if(accepted_socket == -1 )
  {
    fprintf(stderr, "SERVER ERR: %s\n", strerror(errno));
    close(accepted_socket);
    close(server_socket);
  }

  close(server_socket);

  //finished setup, beign communications
  int file_path = recv(accepted_socket, (void*)requestBuffer, BUFF_SIZE, 0);

  fprintf(stdout, "%s\n", requestBuffer);

  char *file_name = requestBuffer;
  //memset(requestBuffer, 0, BUFF_SIZE);
  //requestBuffer[0] = 100;
  //fprintf(stdout, "%d\n", requestBuffer[0]);
  //char tmp[] = "100";
  char tmp[1];
  tmp[0] = 100;
  fprintf(stdout, "send status %d\n", tmp[0]);
  send(accepted_socket, tmp, 1, 0); 


  fprintf(stdout,"open: %s\n", file_name);
  int new_file = open(file_name, O_RDWR | O_CREAT, 0777);
  if(new_file == -1)
  {
    fprintf(stdout, "new file exit\n");
    fprintf(stdout, "%s\n", gai_strerror(new_file));
    exit(1);
  }

  int read_bytes = 0;
  int bytes_written = 0;

  while( (read_bytes = recv(accepted_socket, requestBuffer, BUFF_SIZE, 0)) > 0 )
  {
    fprintf(stdout, "read_bytes: %d\n", read_bytes);
    fprintf(stdout, "%s", requestBuffer); 
    bytes_written = write(new_file, requestBuffer, read_bytes);
    if(bytes_written < 0)
    {
      fprintf(stderr, "SERVER_ERR: %s\n", gai_strerror(bytes_written));
      fprintf(stderr, "%d\n", bytes_written);
      exit(1);
    }//
  }//while
  if(read_bytes < 0)
  {
    fprintf(stderr, "SERVER ERR: %s\n", gai_strerror(read_bytes));
    fprintf(stderr, "errno: %d\n", read_bytes);
  }//if
  close(accepted_socket);
  //close();
  return 0;
}

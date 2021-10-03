#include<stdio.h>
//exit()
#include<stdlib.h>
//strlen(), memset()
#include<string.h>
//send()
#include<sys/socket.h>
#include<sys/types.h>
#include<netdb.h>
//close()
#include<unistd.h>
//errno
#include<errno.h>
//open()
#include<fcntl.h>
#include<sys/stat.h>

#define BUFFER_SIZE 256

int lookup_and_connect(const char *host, const char *service);

int main(int argc, char *argv[])
{
  char *host;
  char buffer[BUFFER_SIZE]; 
  int s;
  int len;
  char *port;
  char *file_name;

  if(argc >= 4)
  {
    host = argv[1];
    port = argv[2];
    file_name = argv[3];
  }
  else
  {
    fprintf(stdout, "usage: <host><port><file_name>\n");
    exit(1);
  }//else

  
  if( (s = lookup_and_connect(host, port)) < 0 )
  {
    exit(1); 
  }

  len = strlen(file_name);
  
  if(len > 255)
  {
    fprintf(stderr, "file name too large");
  }

  int send_status = send(s, file_name, len, 0);
  if(send_status == -1)
  {
    fprintf(stderr, "%s\n", strerror(errno));
  }
  //fprintf(stdout, "%d\n", send_status);
  fprintf(stdout, "sent file name: %s\n", file_name);

  recv(s, (void*)buffer, 256, 0);
  fprintf(stdout, "%d\n", buffer[0]);
  int server_reply = buffer[0];

  memset(&buffer, 0, BUFFER_SIZE); 

  if(server_reply == 100)
  {
    int file_descriptor = open(file_name, O_RDONLY); 
    if(file_descriptor == -1)
    {
      fprintf(stderr, "CLIENT ERR: %s\n", gai_strerror(file_descriptor)); 
      exit(1);
    }
    else
    {
      int read_bytes = 0;
      while( (read_bytes = read(file_descriptor, buffer, BUFFER_SIZE)) > 0)
      { 
        send(s, buffer, read_bytes, 0);
	fprintf(stdout, "read_bytes: %d\n", read_bytes);
	fprintf(stdout, "%s", buffer);
      }//while
      //send(s, buffer, 0, 0);
    }//else
  }//if server_reply
  int state = 0;
  int iter = 0;


  return 0;
}


int lookup_and_connect(const char *host, const char *service)
{
  struct addrinfo hints;
  struct addrinfo *rp, *result; 
  int s;

  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = 0;
  hints.ai_protocol = 0;

  if( (s=getaddrinfo(host, service, &hints, &result) != 0) )
  {
    fprintf(stderr, "stream-talk-client:  getaddrinfo: %s\n", gai_strerror(s));
  }
  else
  {
    fprintf(stdout, "\n");
  }


  for(rp = result; rp != NULL; rp=rp->ai_next)
  {
    if( (s=socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1 )
    {
      continue;
    }

    if( connect(s, rp->ai_addr, rp->ai_addrlen) != -1)
    {
      break; 
    }
    close(s);
  }//for

  if(rp == NULL)
  {
    fprintf(stderr, "stream-talk-client: connect\n");
    return -1;
  }

  freeaddrinfo(result);
  return (s);
}

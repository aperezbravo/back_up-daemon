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
//close(), open(), write(), fork(), getpid()
#include<unistd.h>
//open()
#include<fcntl.h>
//time()
#include<time.h>
//umask()
#include<sys/stat.h>
#include "connect.h"

#define BUFF_SIZE 256
#define PORT "4000"

int main(int argc, char *argv[])
{
  //pid_t pid = fork();
	//pid_t pid = getpid();

  //if( pid == 0)
  {

    umask(0);
    char err[] = "err\0";
    char err1[] = "err1\0";
    char err2[] = "err2\0";
    char err3[] = "err3\0";

    int open_new_file = open("/home/pi/daemon_logs/log.txt", O_RDWR | O_CREAT | O_APPEND, 0666);

    if(open_new_file == -1)
    {
      //fprintf(stderr, "NULL was returned: %s\n", gai_strerror(open_new_file));
      write(open_new_file, err, 2);
      return -1;
    }
    else
    {
      write(open_new_file, err1, 3);
    }
    //fprintf(stdout, "PID: %d\n", getpid());

    char requestBuffer[BUFF_SIZE];
    struct addrinfo hints;
    struct addrinfo *results;


    //memset(requestBuffer, 0, BUFF_SIZE);
    memset(&hints, 0, sizeof(hints)); 

    hints.ai_family = AF_INET; //IPv4
    hints.ai_socktype = SOCK_STREAM; //TCP
    hints.ai_flags = AI_PASSIVE; //wildcard accept any IP's 
    hints.ai_protocol = 0;


    //fprintf(stdout, "line 68\n");
    //obtain ip address from the server
    int server = 0;
    if( (server = getaddrinfo(NULL, PORT, &hints, &results)) != 0)
    {
      //fprintf(stderr, "SERVER ERR: getaddrinfo(): errno. %d %s\n", server, gai_strerror(server));
      write(open_new_file, err2, 3);
      close(open_new_file);
      return -1;
    }//if
    //fprintf(stdout, "line 78\n");

    struct addrinfo *cur_address;
    int server_socket = 0;

  
    for(cur_address = results; cur_address != NULL; cur_address = cur_address->ai_next)
    {
      if( (server_socket = socket(cur_address->ai_family, cur_address->ai_socktype, cur_address->ai_protocol)) == -1)
      {
        continue;
      }//if
      if( !bind(server_socket, cur_address->ai_addr, cur_address->ai_addrlen) )
      {
        break;
      }//if

      close(server_socket);
    }//for


    //fprintf(stdout, "line 100\n");
    if(!cur_address)
    {
      //fprintf(stderr, "SERVER ERR: FAILED TO FIND SPECIFIED \n");
      //fprintf(stderr, "%s\n", gai_strerror(cur_address));
      write(open_new_file, err3, 3);
      close(open_new_file);
      exit(1);
    }
    //fprintf(stdout, "line 108\n");

    //listen in on specified socket on server
    if( listen(server_socket, 5) == -1 )
    {
      //fprintf(stderr, "SERVER ERR: %s\n", strerror(errno));
      write(open_new_file, err, 2);
      close(open_new_file);
      close(server_socket);
      exit(1);
    }
    //fprintf(stdout, "line 119\n");


    struct sockaddr_storage incoming_address;
    socklen_t addr_size = sizeof(incoming_address);
    //accept any incoming connection, get their address
    //int accepted_socket = accept(server_socket, (struct sockaddr*) &incoming_address, &addr_size);

		//Begin infinite loop

		while(1)
		{
    memset(requestBuffer, 0, BUFF_SIZE);
    int accepted_socket = accept(server_socket, (struct sockaddr*) &incoming_address, &addr_size);
    if(accepted_socket == -1 )
    {
      fprintf(stderr, "SERVER ERR: %s\n", strerror(errno));
      write(open_new_file, err, 2);
      close(open_new_file);
      close(accepted_socket);
      close(server_socket);
      exit(1);
    }


    //finished setup, beign communications
    int file_size = recv(accepted_socket, (void*)requestBuffer, BUFF_SIZE, 0);

    char *file_name = requestBuffer;
    char tmp[1];
    tmp[0] = 100;
    send(accepted_socket, tmp, 1, 0); 

    int new_file = open(path, O_RDWR | O_CREAT, 0666);
>>>>>>> 2523a4943240e76e2d0996a654c4b29821201475
    if(new_file == -1)
    {
			fprintf(stderr,"SERVER_ERR: %d\n", 100);
      write(open_new_file, err, 2);
      close(open_new_file);
      exit(1);
    }

    int read_bytes = 0;
    int bytes_written = 0;

    while( (read_bytes = recv(accepted_socket, requestBuffer, BUFF_SIZE, 0)) > 0 )
    {
      bytes_written = write(new_file, requestBuffer, read_bytes);
      if(bytes_written < 0)
      {
        //fprintf(stderr, "%d\n", bytes_written);
        write(open_new_file, err, 2);
        close(open_new_file);
        exit(1);
      }//
    }//while

    if(read_bytes < 0)
    {
      //fprintf(stderr, "SERVER ERR: %s\n", gai_strerror(read_bytes));
      //fprintf(stderr, "errno: %d\n", read_bytes);
    }//if
  
		//close then new accepted socket, not the PORT
    close(accepted_socket);
    char end[] = "finished\n\0";
    write(open_new_file, end, 9);
    close(open_new_file);

	} //end infinite loop

  }//main loop
  return 0;
}

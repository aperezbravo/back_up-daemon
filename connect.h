#define PORT "4000"
#define HOST "192.168.254.200"
#define BUFFER_SIZE 256

//for client change the HOST to NULL

lookup_and_connect()
{ 
  struct addrinfo hints;
  struct addrinfo *rp, *result;
  int s = 0;
  int fd = open("/home/pi/log.txt", O_RDWR | O_CREAT | O_APPEND, 0666);
  char err[] ="there was an error";
  char succ[] = "success writing to file";
  char buffer[BUFFER_SIZE];


  memset(&hints, 0, sizeof(hints));
  hints.ai_family = AF_INET; //ipv4
  hints.ai_socktype = SOCK_STREAM; //TCP
  hints.ai_flags = 0;
  hints.ai_protocol = 0; //takes in any protocols, ex)http, ssh

  //retrieve possible structures pertaining to the address
  if( (s = getaddrinfo(HOST, PORT, &hints, &results)) != 0 )
  {
    write(fd, err, 5);
    return -1;
  }
  else
  {
    write(fd, succ, 5);
  }

  
  for(rp = result; rp != NULL; rp=rp->ai_next)
  {
    if( (s=socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol)) == -1)
    {
      continue;
    }
    if( connect(s, rp->ai_addr, rp_addrlen) != -1 )
    {
      break;
    }
    close(s);
  }//for rp


  if(rp == NULL)
  {
    fprintf(stderr, "CLIENT ERR: Connection Failed\n");
    return -1;
  }

  freaddrinfo(result);
  return (s);

}//lookup_and_connect()

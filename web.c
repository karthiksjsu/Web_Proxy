#include "Proxy_Header.h"

int web_connect(char req[], int writesock)
{
  int sockfd, portno, n, flag;
  struct sockaddr_in serv_addr;
  struct in_addr *pptr;
  struct hostent *server;

  char buffer[500];
  /* Extract host and port from HTTP Request */
  char hoststring[LEN_MSG];
  int i, j;
  /* Parsing the request string */
  for(i=0; i<strlen(req); i++)
  {
    if(req[i] == 'H' && req[i+1] == 'o' && req[i+2] == 's' && req[i+3] == 't')
    {
      for(j=i+6; req[j] != '\r'; j++)
      {
        hoststring[j-i-6] = req[j];
      }
      hoststring[j] = '\0';
      break;
    }
  }
  printf("\nHost extracted : '%s'\n", hoststring);
  /* default port */
  portno = 80;

  /* Create a socket point */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    perror("Error opening socket\n");
    exit(1);
  }
  server = gethostbyname(hoststring);
  if (server == NULL)
  {
    fprintf(stderr, "No such host\n");
    exit(0);
  }
  printf("\nConnected to host\n");

  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  pptr = (struct in_addr  *)server->h_addr;
  bcopy((char *)pptr, (char *)&serv_addr.sin_addr, server->h_length);
  serv_addr.sin_port = htons(portno);

  /* Connect to server */
  if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
  {
    perror("Error in connecting to server\n");
    exit(1);
  }

  /* Message to be sent to the server */
  /* printf("message to server : "); */
  bzero(buffer, 500);
  /* fgets(buffer, 255, stdin); */

  /* Send message to server */
  n = write(sockfd, req, strlen(req));
  if(n < 0)
  {
    perror("Error writing to socket\n");
    exit(1);
  }

  /* Read server response */
  bzero(buffer, 500);
  flag = 1;
  printf("\nreading server response\n");
  while( (n = read(sockfd, buffer,500) > 0))
  {
    if(flag)
    {
      printf("%s", buffer);
      flag = 0;
    }
    i = write(writesock, buffer, strlen(buffer));
  }
  close(sockfd);
  return n;
}


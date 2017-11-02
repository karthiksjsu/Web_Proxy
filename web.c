#include "Proxy_Header.h"

int web_connect(char req[], int writesock)
{
  FILE* fp;
  int sockfd, portno, n, flag;
  struct sockaddr_in serv_addr;
  struct in_addr *pptr;
  struct hostent *server;
  char *host,*first,*second,*host_name;
  char buffer[1000];
  char HOST[100];
  char local[1000];
  /* Extract host and port from HTTP Request */
  char hoststring[LEN_MSG];
  int i, j;
  /* Parsing the request string */

  bzero(local,1000);
  bzero(HOST,100);
  strcat(local,req);
  printf("\nMessage coming into Proxy\n%s\n",req);

  first=strtok(req,"\r\n");
  printf("first line %s\n",first);
  second=strtok(NULL,"\n");
  printf("second line %s\n",second);
  host=strtok(second," \t\n");
  printf("host_line %s\n",host);
  host_name=strtok(NULL,"\0");
  host_name=strtok(host_name,"\r");
  strcat(HOST,host_name);


  printf(" Host extracted :%s\n",HOST);
  /* default port */
  portno = 80;
 
  /* Create a socket point */
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    perror("Error opening socket\n");
    exit(1);
  }

/* if(strcmp(HOST,"www.google.com")==0)
 {
	 printf("same here\n");
 }
  printf("%s\n",HOST);
  fp=fopen("hi.txt","w+");
  n=fprintf(fp,"%s\n",HOST);
  printf("%d",n);
  if(n<0)
  {
	  printf("failed to print\n");
  }
  fclose(fp);
  printf("www.google.com\n");
  printf("I am here\n");*/

  server = gethostbyname2(HOST,AF_INET);
  
  if (server == NULL)
  {
    printf("No such host\n");
    exit(0);
  }
  
  printf("\n..........Connected to host...........\n");

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
   printf("message to server : %s",local); 
  //bzero(buffer,1000);
  /* fgets(buffer, 255, stdin); */

  /* Send message to server */
  n = write(sockfd, local, strlen(local));
  if(n < 0)
  {
    perror("Error writing to socket\n");
    exit(1);
  }

  /* Read server response */
  bzero(buffer,1000);
  flag = 1;
  printf("\nreading server response\n");
  while( (n = read(sockfd, buffer,1000) > 0))
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


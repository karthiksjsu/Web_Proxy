#include "Proxy_Header.h" 

#define MULTITHREAD 1

int web_connect(char*, int);

int main(int argc, char *argv[])
{
  int sockfd, newsockfd, portno, clilen;
  char buffer[LEN_BUF];
  char msg[LEN_MSG];
  struct sockaddr_in serv_addr, cli_addr;
  int n, flag, i, pid;
  char *first;
  /* Command line arguments */
  if(argc < 2)
  {
    fprintf(stderr, "usage : %s <port>\n", argv[0]);
    exit(0);
  }

  /*call to socket() */
  sockfd=socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    perror("Error opening socket\n");
    exit(1);
  }

  /*initialize socket structure*/
  bzero((char *) &serv_addr, sizeof(serv_addr)); /* set serv_addr to zeros*/
  portno = atoi(argv[1]);
  printf("Starting proxy on %d\n", portno);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(portno);

  /* bind the host address */
  if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
  {
    perror("Error on binding\n");
    exit(1);
  }

  /* start listening for clients */
  listen(sockfd, 5);
  clilen = sizeof(cli_addr);
  printf("Proxy running...\nHave fun ! - kh\n");

  /* accept actual connection from client */
  while(1)
  {
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    if(newsockfd < 0)
    {
      perror("Error on accept\n");
      exit(1);
    }

    /* if connection is established, get request */
    bzero(buffer, LEN_BUF);
    bzero(msg, LEN_MSG);
    printf("\nMessage read from web client :\n");
    flag = 0;
     while( (n = read(newsockfd, buffer, LEN_BUF)) > 0)
      {
      strcat(msg, buffer);
      printf("%s",msg);
      printf("here\n");
      first=strtok(msg," /t/n");


     printf("HTTP Request type is :%s",first);
     if ((strcmp(first,"GET\0")) ==0)
      {

      flag =1;
        break;
     
      }
      }
    /* Send HTTP Request to server */
    if(flag == 1)
    {
#ifdef MULTITHREAD
      pid = fork();
      if(pid == 0)
      {
#endif
        n = web_connect(msg, newsockfd);
#ifdef MULTITHREAD
        printf("New thread %d created\n");
      }
#endif
    }
    else
    {
      n = write(newsockfd, "HTTP/1.0 400 Bad Request\n\n", 26);
      if(n < 0)
      {
        perror("Error writing to socket\n");
        exit(1);
      }
    }
#ifdef MULTITHREAD
    if(pid == 0)
    {
      printf("the thread %d fulfilled its destiny successfully\n", getpid());
      exit(0);
    }
#endif
  }
  return 0;
}

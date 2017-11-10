#include "Proxy_Header.h"

int web_connect(char*, int,int);
void error(char*);

void error(char* msg)
{
perror(msg);
exit(0);
}

int main(int argc,char* argv[])
{
pid_t pid;
struct sockaddr_in addr_in,cli_addr,serv_addr;

int sockfd,newsockfd,sockfd1;
   
if(argc<2)
error("./proxy <port_no>");
  
printf("\n*****WELCOME TO PROXY SERVER*****\n");
printf("\nCopyright (c) TEAM 21\n\n");
   
bzero((char*)&serv_addr,sizeof(serv_addr));
bzero((char*)&cli_addr, sizeof(cli_addr));
   
serv_addr.sin_family=AF_INET;
serv_addr.sin_port=htons(atoi(argv[1]));
serv_addr.sin_addr.s_addr=INADDR_ANY;
   
  
sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
if(sockfd<0)
error("Problem in initializing socket");
   
if(bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(serv_addr))<0)
error("Error on binding");
  
  
listen(sockfd,50);
int clilen=sizeof(cli_addr);
  
 
 
accepting:
 
newsockfd=accept(sockfd,(struct sockaddr*)&cli_addr,&clilen);
   
if(newsockfd<0)
error("Problem in accepting connection");
  
pid=fork();
if(pid==0)
{

char buffer[510];

bzero((char*)buffer,500);
recv(newsockfd,buffer,500,0);

printf("Message read from the webclient \n%s\n",buffer);
   
sockfd1=web_connect(buffer, newsockfd,sockfd);

close(sockfd1);
close(newsockfd);
close(sockfd);
_exit(0);
}
else
{
close(newsockfd);
goto accepting;
}
return 0;
}

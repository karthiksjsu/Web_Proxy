#include "Proxy_Header.h"
#include <mysql/mysql.h>

static char *host = "localhost";  // local database server
static char *user = "root";      //username to connect to mysql server
static char  *pass = "nikhil";
static char *dbname = "cache_db";


unsigned int sql_port = 3306;
static char *sql_socket = NULL;
unsigned int sql_flag =0;


extern void error(char*);

int web_connect(char* buffer, int newsockfd,int sockfd)
{
	struct sockaddr_in host_addr;
	struct hostent* host;
	int flag=0,newsockfd1,n,port=0,i,sockfd1;
	char* temp=NULL;
	char t1[300],t2[300],t3[10];

	sscanf(buffer,"%s %s %s",t1,t2,t3);


	if(((strncmp(t1,"GET",3)==0))&&((strncmp(t3,"HTTP/1.1",8)==0)||(strncmp(t3,"HTTP/1.0",8)==0))&&(strncmp(t2,"http://",7)==0))
	{
		strcpy(t1,t2);

		flag=0;

		for(i=7;i<strlen(t2);i++)
		{
			if(t2[i]==':')
			{
				flag=1;
				break;
			}
		}

		temp=strtok(t2,"//");
		if(flag==0)
		{
			port=80;
			temp=strtok(NULL,"/");
		}
		else
		{
			temp=strtok(NULL,":");
		}

		sprintf(t2,"%s",temp);
		printf("host = %s",t2);


		host=gethostbyname(t2);

		char query[1000];
		sprintf(query,"select * from cache where hostname='%s'",t2);	



		mysql_query(conn,query);

		res = mysql_store_result(conn);
		row=mysql_fetch_row(res);

		if(row>0)	//Table has entries. Response is sent to client from cache table. 
		{
			buffer = row[1];
			send(newsockfd,buffer,n,0);
		}

		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;

		char *sql_host = "localhost";

		conn = mysql_init(NULL);
		if(!(mysql_real_connect(conn,sql_host,user,pass,dbname,sql_port,sql_socket,sql_flag)))   //return zero on success
		{
			fprintf(stderr,"Error: %s [%d]\n", mysql_error(conn),mysql_errno(conn));
			exit(1);
		}								



		if(flag==1)
		{
			temp=strtok(NULL,"/");
			port=atoi(temp);
		}


		strcat(t1,"^]");
		temp=strtok(t1,"//");
		temp=strtok(NULL,"/");
		if(temp!=NULL)
			temp=strtok(NULL,"^]");
		printf("\npath = %s\nPort = %d\n",temp,port);


		bzero((char*)&host_addr,sizeof(host_addr));
		host_addr.sin_port=htons(port);
		host_addr.sin_family=AF_INET;
		bcopy((char*)host->h_addr,(char*)&host_addr.sin_addr.s_addr,host->h_length);

		sockfd1=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		newsockfd1=connect(sockfd1,(struct sockaddr*)&host_addr,sizeof(struct sockaddr));
		sprintf(buffer,"\nConnected to %s  IP - %s\n",t2,inet_ntoa(host_addr.sin_addr));
		if(newsockfd1<0)
			error("Error in connecting to remote server");

		printf("\n%s\n",buffer);
		//send(newsockfd,buffer,strlen(buffer),0);
		bzero((char*)buffer,sizeof(buffer));
		if(temp!=NULL)
			sprintf(buffer,"GET /%s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",temp,t3,t2);
		else
			sprintf(buffer,"GET / %s\r\nHost: %s\r\nConnection: close\r\n\r\n",t3,t2);


		n=send(sockfd1,buffer,strlen(buffer),0);
		printf("Request sent to the server\n%s\n",buffer);
		if(n<0)
			error("Error writing to socket");
		else{

			do
			{
				bzero((char*)buffer,500);
				n=recv(sockfd1,buffer,500,0);
				char add_cache[1000];
				printf("HostName : %s\n",t2);
				sprintf(add_cache,"Insert into cache (hostname,response) values ('%s','%s');",t2,buffer);

				mysql_query(conn,add_cache);

				res=mysql_store_result(conn);
				if(!(n<=0)){
					send(newsockfd,buffer,n,0);
					//printf("Response recieved from the server\n%s\n",buffer);
				}
			}while(n>0);
			printf("output bufferrrrrrrrrrrrrrrrr: %s\n",buffer);

		}
	}
	else
	{
		send(newsockfd,"400 : BAD REQUEST\nONLY HTTP REQUESTS ALLOWED",18,0);
	}


	return sockfd1;
}

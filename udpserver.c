#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<netdb.h>
#include<string.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<errno.h>
#define RET_OK 0
#define RET_ERR -1
#define LISTEN_QUEUE_NUM 5
#define BUFFER_SIZE 256
#define ECHO_PORT 9999
int main(int argc,char **argv)
{
	int sockfd,opt=1;
	uint32_t len;
	struct sockaddr_in cliaddr;
	uint8_t buffer[BUFFER_SIZE];
	int ret=RET_OK;
	short listen_port=ECHO_PORT;
        if(argc != 2){
		printf("usage:%s listen_port.\n",argv[0]);
		return -1;
	}
	listen_port=atoi(argv[1]);
	if((sockfd=socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("ERROR opening socket");
		return RET_ERR;
	}

	//SO_REUSEADDR允许重用本地址
	if((ret=setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt)))<0)
	{
		perror("ERROR setsockopt");
		goto failed;
	}
	memset(&cliaddr,0,sizeof(cliaddr));
	cliaddr.sin_family=AF_INET;
	cliaddr.sin_addr.s_addr=INADDR_ANY;
	cliaddr.sin_port=htons(listen_port);
	if((ret=bind(sockfd,(struct sockaddr*)&cliaddr,sizeof(cliaddr)))<0)
	{
		perror("ERROR on binding");
		goto failed;
	}
	printf("udp server listening on *:%d\n", listen_port);
	do
	{
		len=sizeof(cliaddr);
		if((ret=recvfrom(sockfd,buffer,sizeof(buffer),0,(struct sockaddr*)&cliaddr,&len))>0)
		{
			buffer[ret]=0;
			printf("Recv udp data from %s:%d . length:%d, data:%s\n",
				inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), ret,buffer);
			ret=sendto(sockfd,buffer,ret,0,(struct sockaddr*)&cliaddr,len);
		}
	}while(ret>=0);
failed:
	close(sockfd);
	return 0;
}

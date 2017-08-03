#include <sys/types.h> 
#include <sys/socket.h> 
#include <string.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#define MAXLINE 80 
#define SERV_PORT 8888 
void do_cli(FILE *fp, int sockfd, struct sockaddr *pservaddr, socklen_t servlen) 
{ 
	int n; 
	char sendline[MAXLINE], recvline[MAXLINE + 1]; 
	if(connect(sockfd, (struct sockaddr *)pservaddr, servlen) == -1) 
	{ 
		perror("connect error"); 
		exit(1); 
	} 
        int val = 1;
        if( setsockopt( sockfd, SOL_IP, IP_RECVERR, &val, sizeof(val) ) == -1 ){
            perror("setsockopt: ");
            exit(1);
        }
	while(fgets(sendline, MAXLINE, fp) != NULL) 
	{ 
                printf("begin send \n" );
		n = write(sockfd, sendline, strlen(sendline)); 
                printf("end send \n" );
		if(n == -1) 
		{ 
			perror("write error"); 
			exit(1); 
		} 
                printf("begin read \n" );
		n = read(sockfd, recvline, MAXLINE); 
                printf("end read \n" );
		if(n == -1) 
		{ 
			perror("read error"); 
			exit(1); 
		} 
		recvline[n] = 0; 
		printf("recv data:%s\n",recvline); 
	} 
} 
int main(int argc, char **argv) 
{ 
	int sockfd; 
	struct sockaddr_in servaddr; 
	if(argc != 3) 
	{ 
		printf("usage: udpclient <IPaddress> <Port>\n"); 
		exit(1); 
	} 
	int port = atoi(argv[2]);
	bzero(&servaddr, sizeof(servaddr)); 
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(SERV_PORT); 
	if(inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) 
	{ 
		printf("[%s] is not a valid IPaddress!\n", argv[1]); 
		exit(1); 
	} 
	if(port>65535 || port<0) 
	{ 
		printf("[%s] is not a valid Port range(0~65535)!\n", argv[2]); 
		exit(1); 
	} 
	servaddr.sin_port = htons(port); 
	sockfd = socket(AF_INET, SOCK_DGRAM, 0); 
	do_cli(stdin, sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)); 
	return 0; 
} 

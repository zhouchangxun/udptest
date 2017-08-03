#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
/*
#define ICMP_ECHOREPLY		0	// Echo Reply			
#define ICMP_DEST_UNREACH	3	// Destination Unreachable	
#define ICMP_REDIRECT		5	// Redirect (change route)	
#define ICMP_ECHO		8	// Echo Request			
*/
void send_echo_req(int sockfd, struct sockaddr_in *dstaddr);  
uint16_t in_cksum(uint16_t *addr, int len);  
void recv_echo_reply(int sockfd);  

void err_sys(const char *errmsg)  
{
    perror(errmsg);
    exit(1);  
}
void timeout_callback(){
    printf("recv timeout(5s)! icmp packet may be forbidden by peer firewall\n");
}
void set_timeout_handler()
{
    signal(SIGALRM, timeout_callback); //让内核做好准备，一旦接受到SIGALARM信号,就执行 handler
    alarm(5);
}

int main(int argc, char **argv)  
{
    int sockfd;
    struct sockaddr_in dstaddr;  

    if ((sockfd = socket(PF_INET, SOCK_RAW, IPPROTO_ICMP)) == -1)
        err_sys("socket");

    bzero(&dstaddr, sizeof(dstaddr));
    dstaddr.sin_family = AF_INET;
    dstaddr.sin_port = htons(0);
    if(argc != 2){
        printf("usage: %s <IPaddress> \n", argv[0]);
        exit(1);
    } 
    if (inet_pton(AF_INET, argv[1], &dstaddr.sin_addr) <= 0)
        err_sys("inet_pton");

    send_echo_req(sockfd, &dstaddr);
    recv_echo_reply(sockfd);
    exit(0);  
}  

void send_echo_req(int sockfd, struct sockaddr_in *dstaddr)  
{
    char buf[100];
    size_t len = sizeof(struct icmp);
    struct icmp *icmp;
    socklen_t dstlen = sizeof(struct sockaddr_in);

    bzero(buf, sizeof(buf));
    icmp = (struct icmp *)buf;
    icmp->icmp_type = ICMP_ECHO;//8
    icmp->icmp_code = 0;
    icmp->icmp_id = getpid();
    icmp->icmp_seq = 1;
    icmp->icmp_cksum = in_cksum((uint16_t *) icmp, sizeof(struct icmp));
    if (sendto(sockfd, buf, len, 0, (struct sockaddr *)dstaddr, dstlen) == -1)
        err_sys("sendto");  
}

void recv_echo_reply(int sockfd)  
{
    char buf[100];
    ssize_t n;
    struct ip *ip;
    struct icmp *icmp;
    while (1) {
        alarm(5); /* set 5s timeout */
        if ((n = read(sockfd, buf, sizeof(buf))) == -1)
            err_sys("read");
        ip = (struct ip *)buf;

        if (ip->ip_p != IPPROTO_ICMP) {
            fprintf(stderr, "protocol error.");
            exit(1);
        }

        icmp = (struct icmp *)(buf + sizeof(struct ip));


        fprintf(stdout, "recv icmp response,icmp_type=%d.\n", icmp->icmp_type);
        if (icmp->icmp_type == ICMP_ECHOREPLY) {//0
            if (icmp->icmp_id != getpid()) {
                fprintf(stderr, "not this process.");
                exit(1);
            } else {
                printf("destination host is up.\n");
                break;
            }
        }else if(icmp->icmp_type == ICMP_DEST_UNREACH){//3
            printf("destination host is down.\n");
            exit(1);

       }else{
           fprintf(stdout, "unhandled response,icmp_type=%d.\n", icmp->icmp_type);
       }
    }  
}

uint16_t in_cksum(uint16_t *addr, int len)  
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;
    while (nleft > 1) {
        sum += *w++;
        nleft -= 2;
    }  

    if (nleft == 1) {
        *(unsigned char *)(&answer) = *(unsigned char *)w ;
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;
    return(answer);  
}

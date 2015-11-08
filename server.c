/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:    server.c

 Description:  a server manages the users operations

 Author:       HeXiong

 Email:        hexiong@tp-link.net

 History:

 -----------------------------------------------
 v0.1,2015-11-06,hexiong create the file and complete the function
 ************************************************************************/

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

/* max transmission length */
#define MAXLINE 4096

/* port number */
#define SERV_PORT 3000

/* max result length*/
#define MAXRESULTLEN 80

#define UNIX_DOMAIN "/tmp/UNIX.domain"
#define UNIX_SEND   "/tmp/UNIX.send"

#define EXCUTE_PATH

int main(void)
{
    /* receive buffer */
    char recvline[MAXLINE] = {0};

    /* send buffer */
    char sendline[MAXLINE] = {0};

    /* create the socket */
    int sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);

    /* return bind result */
    int ret;

    if (sockfd < 0)
    {
        printf("Problem in creating the socket\n");
        exit(1);
    }

    /* we receive content from this file */
    struct sockaddr_un servaddr ;
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, UNIX_DOMAIN, sizeof(servaddr.sun_path) - 1);
    unlink(UNIX_DOMAIN);

    /* we send content to this file */
    struct sockaddr_un servaddr1 ;
    servaddr1.sun_family = AF_UNIX;
    strncpy(servaddr1.sun_path, UNIX_SEND, sizeof(servaddr1.sun_path) - 1);

    ret = bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (ret == -1)
    {
        perror("Cannot bind server socket\n");
        close(sockfd);
        unlink(UNIX_DOMAIN);
        exit(2);
    }

    printf("Accepting connections ...\n");

    while (1)
    {
        struct sockaddr_in cliaddr;
        socklen_t cliaddr_len = sizeof(cliaddr);

        if (recvfrom(sockfd, recvline, MAXLINE, 0, (struct sockaddr *)&cliaddr, &cliaddr_len) < 0)
        {
            printf("recvfrom error\n");
            exit(4);
        }
        else
        {
            /* start network manager */
            if (recvline[0] == '1')
            {
                /* send result to client */
                sendline[0] = '1';
                sendline[1] = '\0';
                if (sendto(sockfd, sendline, strlen(sendline), 0,
                           ( struct sockaddr *)&servaddr1, sizeof(servaddr1))< 0)
                {
                    perror("sendto error\n");
                    exit(5);
                }
                system("./flow_control");
                memset(sendline, 0, strlen(sendline));
            }
            /* stop network manager */
            else if (recvline[0] == '2')
            {
                /* get the result */
                sendline[0] = '1';
                sendline[1] = '\0';
                if (sendto(sockfd, sendline, strlen(sendline), 0,
                           (struct sockaddr *)&servaddr1, sizeof(servaddr1)) < 0)
                {
                    perror("sendto error\n");
                    exit(5);
                }
                system("killall ./flow_control");
                memset(sendline, 0, strlen(sendline));
            }
            else
            {
                sendline[0] = '0';
                sendline[1] = '\0';
                if (sendto(sockfd, sendline, strlen(sendline), 0,
                           (struct sockaddr *)&servaddr1, sizeof(servaddr1)) < 0)
                {
                    perror("sendto error\n");
                    exit(5);
                }
                memset(sendline, 0, strlen(sendline));
                /* we have no these conditions */

                continue;
            }
            /* clear the recvline */
            memset(recvline, 0, sizeof(recvline));
        }
    }

    return 0;
}


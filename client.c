/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:    client.c

 Description:  a client gives instructions and get response

 Author:       HeXiong

 Email:        hexiong@tp-link.net

 History:

 -----------------------------------------------
 v0.1,2015-11-06,hexiong create the file and complete the function
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "client.h"

#define MAXLINE 4096
#define SERV_PORT 3000
#define UNIX_DOMAIN "/tmp/UNIX.domain"
#define UNIX_SEND   "/tmp/UNIX.send"

int manager_start(int choice)
{
    /* socket discriptor */
    int sockfd;

    /* the read and write buffer */
    char sendline[MAXLINE];
    char recvline[MAXLINE];

    /* judge bind successfully or not */
    int ret;

    /* create the socket */
    sockfd = socket(PF_UNIX, SOCK_DGRAM, 0);
    if (sockfd < 0)
    {
        printf("Problem in creating the socket\n");
        return 0;
    }

    /* we send content to this file */
    struct sockaddr_un servaddr;
    servaddr.sun_family = AF_UNIX;
    strncpy(servaddr.sun_path, UNIX_DOMAIN, sizeof(servaddr.sun_path) - 1);

    /* we receive content from this file */
    struct sockaddr_un servaddr1;
    servaddr1.sun_family = AF_UNIX;
    strncpy(servaddr1.sun_path, UNIX_SEND, sizeof(servaddr1.sun_path) - 1);
    unlink(UNIX_SEND);

    /* memset buffer */
    memset(sendline, 0, MAXLINE);

    ret = bind(sockfd, (struct sockaddr *)&servaddr1, sizeof(servaddr1));
    if (ret == -1)
    {
        perror("Cannot bind socket\n");
        close(sockfd);
        unlink(UNIX_SEND);
        return 0;
    }

            /* according to user's input to fill buffer */
    if (1 == choice)
    {
        sendline[0] = '1';
        sendline[1] = '\0';
    }
    else if (2 == choice)
    {
        sendline[0] = '2';
        sendline[1] = '\0';
    }
    else
    {
        return 0;
    }

    /* send buffer to server */
    printf("The sendline is %s\n", sendline);
    if (sendto(sockfd, sendline, strlen(sendline), 0,
            (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        printf("sendto error\n");
        return 0;
    }

    while (1)
    {
        /* store the result return from server */
        char result[MAXLINE];
        memset(result, 0, MAXLINE);

        if (recvfrom(sockfd, recvline, MAXLINE, 0, NULL, 0) < 0)
        {
            printf("read error\n");
            return 0;
        }
        else
        {
            if (recvline[0] == '1')
            {
                close(sockfd);
                unlink(UNIX_SEND);
                return 1;
            }
            else
            {
                close(sockfd);
                unlink(UNIX_SEND);
                return 0;
            }
        }
    }
}

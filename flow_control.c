/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   network_manager.c

 Description: this module ues to manage network stream flow

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <pthread.h>
#include "flow_control.h"

#define PROCESS_LEN 4096
#define CMD_LEN 128
#define PORT_LEN 6
#define DEV_LEN 10
#define MAX_PORT 10
#define START_MANGLE "./manage.sh start_mangle eth0 "
#define STOP_MANGLE "./manage.sh stop_mangle"
#define PREFIX "./manage.sh add_mangle eth0 "

char* reverse(char *s)
{
    char *p = s ;

    char *q = s ;

    while (*q)
    {
        ++q;
    }

    q --;

    while (q > p)
    {
        char t = *p ;
        *p++ = *q ;
        *q-- = t ;
    }

    return s ;
}

int get_port(int **ports)
{
    FILE *stream;
    /* loop variable */
    int i, j, k = 0;
    int num = 0;
    int read_succeed;
    char buf[PROCESS_LEN];
    char cmd[CMD_LEN];
    char port_buffer[PORT_LEN];

    *ports = malloc(sizeof(int) * MAX_PORT);

    memset(buf, 0, PROCESS_LEN);
    memset(cmd, 0, CMD_LEN);
    memset(port_buffer, 0, PORT_LEN);

    strcat(cmd, "netstat -np | grep aria2c ");

    stream = popen(cmd, "r");
    /* call popen() failed */
    if (NULL == stream)
    {
        return -1;
    }

    read_succeed = fread(buf, sizeof(char), sizeof(buf), stream);

    if (0 == read_succeed)
    {
        return 0;
    }
    printf("The buf is %s\n", buf);
    //printf("The length of buf is %d\n", strlen(buf));

    for (i = 0; i < strlen(buf); i ++)
    {
        if (buf[i] != '/')
        {
            continue;
        }
        else
        {
            for (j = i - 1; !isspace(buf[j]); j --)
            {
                port_buffer[k ++] = buf[j];
            }
            (*ports)[num ++] = atoi(reverse(port_buffer));
            k = 0;
        }
    }

    pclose(stream);

    return num;
}

void manager()
{
    char dev[10];
    char cmd[CMD_LEN];
    int *port;
    char port_buffer[PORT_LEN];
    int num;
    int i, j;
    memset(dev, 0, 10);
    strcpy(dev, "eth0");

    printf("manager:manager start running......\n");

    while (1)
    {
        num = get_port(&port);
        system(STOP_MANGLE);
        system(START_MANGLE);
        printf("the num is %d\n", num);
        for (i = 0; i < num; i++)
        {
            memset(port_buffer, 0, 6);
            memset(cmd, 0, 128);

            sprintf(port_buffer, "%d", port[i]);
            printf("the port_buffer is %s\n", port_buffer);

            strcat(cmd, PREFIX);
            strcat(cmd, port_buffer);
            printf("the cmd is %s\n", cmd);
            system(cmd);
        }
	free(port);
        sleep(10);
    }
}

int main()
{
    manager();
    return 0;
}

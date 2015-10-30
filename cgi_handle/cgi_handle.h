/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   cgi_handle.h

 Description: we write a cgi to handle user's request in this file

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#ifndef _CGI_HANDLE_H
#define _CGI_HANDLE_H

#define DEBUG_PRINT 1

/* Data from web to CGI */
#define WEB_ACTION "action"
#define WEB_DOWNLOAD_URL "url"
#define WEB_DIR "dir"
#define WEB_NAME "name"
#define WEB_MAX_SPEED "speed"
#define WEB_LIMIT_TIME "time"

/* the action type */
#define ACTION_DOWNLOAD_START 0
#define ACTION_DOWNLOAD_STOP 1
#define ACTION_DOWNLOAD_SPEED_SET 2
#define ACTION_DOWNLOAD_TIME_SET 3
#define ACTION_DOWNLOAD_DELETE 4
#define ACTION_MINIDLNAD_START 5
#define ACTION_MINIDLNAD_STOP 6

/* the length of the field in struct */
#define MAX_URL_LEN 1000
#define MAX_DIR_LEN 512
#define MAX_NAME_LEN 256
#define MAX_SPEED_LEN 10

/* the struct store the data from web */
struct data_from_web
{
    int action;
    char url[MAX_URL_LEN];
    char name[MAX_NAME_LEN];
    char dir[MAX_DIR_LEN];
    char speed[MAX_SPEED_LEN];
    int time;
};

/* Data from CGI to web */
#define TOWEB_ACTION "action"
#define TOWEB_RESULT "result"

/* result type */
#define RESULT_SUCCEED 0
#define RESULT_FAILURE_LOCAL 1
#define RESULT_NOT_COMPLETE 2

#ifdef __cplusplus
extern "C" {
#endif

char *get_input(void);

int parse_input(char *pinput, struct data_from_web *pstru);

void action_download_start(struct data_from_web *pinfo);

void action_minidlnad_start(struct data_from_web *pinfo);

void action_minidlnad_stop(struct data_from_web *pinfo);

#ifdef __cplusplus
}
#endif

#endif

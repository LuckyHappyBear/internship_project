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

/* Data from web to CGI */
#define WEB_MODULE "module"
#define WEB_ACTION "action"
#define WEB_DOWNLOAD_URL "url"
#define WEB_MAX_SPEED "speed"
#define WEB_LIMIT_TIME "time"

/* the action type */
#define ACTION_DOWNLOAD_START 0
#define ACTION_DOWNLOAD_STOP 1
#define ACTION_DOWNLOAD_SPEED_SET 2
#define ACTION_DOWNLOAD_TIME_SET 3
#define ACTION_MINIDLNA_START 4
#define ACTION_MINIDLNA_STOP 5

/* the length of the field in struct */
#define MAX_URL_LEN 1000
#define MAX_MODULE_LEN 100
#define MAX_SPEED_LEN 10
#define MAX_TIME_LEN 10

/* the struct store the data from web */
struct data_from_web
{
    int action;
    char url[MAX_URL_LEN];
    char module[MAX_MODULE];
    char speed[MAX_SPEED_LEN];
    char time[MAX_TIME_LEN];
};

/* Data from CGI to web */
#define TOWEB_ACTION "action"
#define TOWEB_RESULT "result"

#ifdef __cplusplus
extern "C" {
#endif

char *get_web_input(void);

int parse_input(char *pinput, struct data_from_web *pstru);

#ifdef __cplusplus
}
#endif

#endif

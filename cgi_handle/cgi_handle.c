/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   cgi_handle.c

 Description: we write a cgi to handle user's request in this file

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "cgi_handle.h"
#include "../cJSON/cJSON.h"
#include "../func_control_h/aria2c_control.h"
#include "../func_control_h/minidlnad_control.h"

char *get_web_input(void)
{
    char *pmethod = getenv("REQUEST_METHOD");

    /* Do nothing if the method is nonexistence or is "GET" */
    if ( ( NULL == pmethod ) || ( !strcmp(pmethod, "GET") ) )
    {
        return NULL;
    }
    /* Deal with method "POST" */
    else if ( !strcmp(pmethod, "POST") )
    {
        /* Get the length of the POST data */
        int datalen = atoi( getenv("CONTENT_LENGTH") );

        /* If no data */
        if ( 0 == datalen )
        {
            return NULL;
        }
        else
        {
            /* buffer to store the input from web */
            int buflen = 1 + sizeof(char) * datalen;
            char *pinput = (char *)malloc( buflen );
            if ( NULL == pinput )
            {
                //perror("No room for input from web to cgi");
                assert( NULL != pinput );
            }
            memset(pinput, 0, buflen);

            /* Read input to buffer */
            fread(pinput, sizeof(char), datalen, stdin);

            return pinput;
        }
    }
    /* Deal with exception method */
    else
    {
        return NULL;
    }
}

int parse_input(char *pinput, struct data_from_web *pstru)
{
    /* Input check */
    if ( NULL == pinput )
    {
        return 1;
    }
    if ( NULL == pstru )
    {
        return 2;
    }

    /* Initialize *pstru */
    pstru->action = 0;
    memset(pstru->url, 0, MAX_URL_LEN);
    memset(pstru->module, 0, MAX_MODULE_LEN);
    memset(pstru->speed, 0, MAX_SPEED_LEN);
    memset(pstru->time, 0, MAX_TIME_LEN);

    /* Parse input string to cjson structure */
    cJSON *pjson_web_to_cgi = cJSON_Parse(pinput);

    /* cJSON parse fail */
    if ( NULL == pjson_web_to_cgi )
    {
#if CGIC_LOCAL_TEST
        printf("Error befor: [%s]\n", cJSON_GetErrorPtr());
#endif
        return 3;
    }
    /* cJSON parse succeed */
    else
    {
        cJSON *pjson_val = NULL;

        /* parse WEB_ACTION */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_ACTION);
        if ( NULL != pjson_val )
        {
            pstru->action = pjson_val->valueint;
        }

        /* parse WEB_DBID */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_DOWNLOAD_URL);
        if ( NULL != pjson_val )
        {
            strcpy(pstru->url, pjson_val->valuestring);
        }

        /* parse WEB_REMARK */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_MAX_SPEED);
        if ( NULL != pjson_val )
        {
            strcpy(pstru->speed, pjson_val->valuestring);
        }

        /* parse WEB_MODULE */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_MODULE);
        if ( NULL != pjson_val )
        {
            strcpy(pstru->module, pjson_val->valuestring);
        }

        /* parse WEB_IP */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_LIMIT_TIME);
        if ( NULL != pjson_val )
        {
            strcpy(pstru->time, pjson_val->valuestring);
        }

        cJSON_Delete(pjson_web_to_cgi);
        return 0;
    }
}

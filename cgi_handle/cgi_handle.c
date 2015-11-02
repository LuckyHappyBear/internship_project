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

char *get_input(void)
{
    char *pmethod = getenv("REQUEST_METHOD");

    /* Do nothing if the method is nonexistence or is "GET" */
    if ((NULL == pmethod) || (!strcmp(pmethod, "GET")))
    {
        return NULL;
    }
    /* Deal with method "POST" */
    else if (!strcmp(pmethod, "POST"))
    {
        /* Get the length of the POST data */
        int datalen = atoi(getenv("CONTENT_LENGTH"));

        /* If no data */
        if (0 == datalen)
        {
            return NULL;
        }
        else
        {
            /* buffer to store the input from web */
            int buflen = 1 + sizeof(char) * datalen;
            char *pinput = (char *)malloc(buflen);
            if (NULL == pinput)
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
    if (NULL == pinput)
    {
        return 1;
    }
    if (NULL == pstru)
    {
        return 2;
    }

    /* Initialize *pstru */
    pstru->action = 0;
    memset(pstru->url, 0, MAX_URL_LEN);
    memset(pstru->dir, 0, MAX_DIR_LEN);
    memset(pstru->name, 0, MAX_NAME_LEN);
    memset(pstru->speed, 0, MAX_SPEED_LEN);
    pstru->time = 0;
    /* Parse input string to cjson structure */
    cJSON *pjson_web_to_cgi = cJSON_Parse(pinput);

    /* cJSON parse fail */
    if (NULL == pjson_web_to_cgi)
    {
        #if DEBUG_PRINT
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
        if (NULL != pjson_val)
        {
            pstru->action = pjson_val->valueint;
        }

        /* parse WEB_URL */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_DOWNLOAD_URL);
        if (NULL != pjson_val)
        {
            strcpy(pstru->url, pjson_val->valuestring);
        }
        /* parse WEB_DIR */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_DIR);
        if (NULL != pjson_val)
        {
            strcpy(pstru->dir, pjson_val->valuestring);
        }
        /* parse WEB_NAME */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_NAME);
        if (NULL != pjson_val)
        {
            strcpy(pstru->name, pjson_val->valuestring);
        }

        /* parse WEB_MAX_SPEED */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_MAX_SPEED);
        if (NULL != pjson_val)
        {
            strcpy(pstru->speed, pjson_val->valuestring);
        }

        /* parse  WEB_LIMIT_TIME */
        pjson_val = cJSON_GetObjectItem(pjson_web_to_cgi, WEB_LIMIT_TIME);
        if (NULL != pjson_val)
        {
            pstru->time = pjson_val->valueint;
        }

        cJSON_Delete(pjson_web_to_cgi);
        return 0;
    }
}

static void output_cJSON(cJSON *root)
{
    char *out = cJSON_PrintUnformatted(root);
    printf("Content-Type: text/html\n");
    printf("Content-Length: %d\n\n", (int)strlen(out));
    printf("%s", out);
    free(out);
    out = NULL;
}

void action_download_start(struct data_from_web *pinfo)
{
    #if DEBUG_PRINT
    printf("action_download_start:we reach here to start\n");
    #endif
    int flag_download_start = aria2c_start(pinfo->url, pinfo->dir, pinfo->name,
                                           pinfo->time, pinfo->speed, 1);
    if (-1 == flag_download_start)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else if (0 == flag_download_start)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_NOT_COMPLETE);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: download not complete */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: download successfully */
        return;
    }
}

void action_download_stop(struct data_from_web *pinfo)
{
    #if DEBUG_PRINT
    printf("action_download_stop:we reach here to stop\n");
    #endif
    int flag_download_stop = aria2c_stop(pinfo->url, pinfo->dir,
                                         pinfo->name);
    if (-1 == flag_download_stop)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
}

void action_download_restart(struct data_from_web *pinfo)
{
    #if DEBUG_PRINT
    printf("action_download_restart:we reach here to restart\n");
    #endif
    int flag_download_restart = aria2c_restart(pinfo->url, pinfo->dir,
                                         pinfo->name);
    if (-1 == flag_download_restart)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
}

void action_download_delete(struct data_from_web *pinfo)
{
    #if DEBUG_PRINT
    printf("action_download_delete:we reach here to delete\n");
    #endif
    int flag_download_delete = aria2c_delete(pinfo->url, pinfo->dir,
                                         pinfo->name);
    if (-1 == flag_download_delete)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
}

void action_network_manage(struct data_from_web *pinfo)
{
    int flag_network_manage = network_manager();
    if (-1 == flag_network_manage)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
}

void action_minidlnad_start(struct data_from_web *pinfo)
{
    int flag_minidlnad_start = minidlnad_start();
    if (0 == flag_minidlnad_start)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: minidlnad_start successfully */
        return;
    }
}

void action_minidlnad_stop(struct data_from_web *pinfo)
{
    int flag_minidlnad_stop = minidlnad_stop();
    if (0 == flag_minidlnad_stop)
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_FAILURE_LOCAL);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: failure_local */
        return;
    }
    else
    {
        cJSON *root = cJSON_CreateObject();
        cJSON_AddNumberToObject(root, TOWEB_ACTION, pinfo->action);
        cJSON_AddNumberToObject(root, TOWEB_RESULT, RESULT_SUCCEED);
        output_cJSON(root);
        cJSON_Delete(root);
        root = NULL;
        /* end of: Response to web: minidlnad_stop successfully */
        return;
    }
}

/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   cgic.c

 Description: we use this file to call function to handle user's request

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "flow_control.h"
#include "cgi_handle.h"

int main()
{
    /* get input from web */
    char *pinput = get_input();
    pthread_t id = 1;

    /* if no input,return 0 */
    if (NULL == pinput)
    {
        #if DEBUG_PRINT
        printf(" NULL == pinput\n");
        #endif
        //return 0;
    }

    #if DEBUG_PRINT
    puts("Content-Type: text/html\n\n");
    printf("the input is %s", pinput);
    #endif
    struct data_from_web request;
    /* parse failed */
    if(0 != parse_input(pinput, &request))
    {
        free(pinput);
        return 0;
    }

    #ifdef DEBUG_PRINT
    printf("action = %d\n", request.action);
    printf("url = %s\n", request.url);
    printf("speed = %s\n", request.speed);
    printf("time = %d\n", request.time);
    #endif
    switch (request.action)
    {
    case ACTION_DOWNLOAD_START:
        #ifdef DEBUG_PRINT
        printf("excute action_download_start\n");
        #endif

        action_download_start(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_DOWNLOAD_STOP:
        #if DEBUG_PRINT
        printf("excute action_download_stop\n");
        #endif

        action_download_stop(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_DOWNLOAD_RESTART:
        #ifdef DEBUG_PRINT
        printf("excute action_download_restart\n");
        #endif

        action_download_restart(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_DOWNLOAD_DELETE:
        #ifdef DEBUG_PRINT
        printf("excute action_download_delete\n");
        #endif

        action_download_delete(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_GET_STATUS:
        #ifdef DEBUG_PRINT
        printf("excute action_get_status\n");
        #endif

        action_get_status(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_NETWORK_MANAGE:
        #ifdef DEBUG_PRINT
        printf("excute action_network_manage\n");
        #endif

        action_network_manage(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_MANAGER_START:
        #ifdef DEBUG_PRINT
        printf("excute action_manager_start\n");
        #endif

        action_manager_start(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_MANAGER_STOP:
        #ifdef DEBUG_PRINT
        printf("excute action_manager_stop\n");
        #endif

        action_manager_stop(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_MINIDLNAD_START:
        #ifdef DEBUG_PRINT
        printf("excute action_minidlnad_start\n");
        #endif

        action_minidlnad_start(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    case ACTION_MINIDLNAD_STOP:
        #ifdef DEBUG_PRINT
        printf("excute action_minidlnad_stop\n");
        #endif

        action_minidlnad_stop(&request);

        #ifdef DEBUG_PRINT
        printf("excute finished\n");
        #endif
        break;
    default:
        break;
    }
    return 0;
}


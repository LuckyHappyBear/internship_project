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
#include "cgi_handle.h"

int main()
{
    /* get input from web */
    char *pinput = get_input();

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
    case ACTION_MINIDLNAD_START:
        action_minidlnad_start(&request);
        break;
    case ACTION_MINIDLNAD_STOP:
        action_minidlnad_stop(&request);
        break;
    default:
        break;
    }

    return 0;
}


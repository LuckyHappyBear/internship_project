/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   minidlnad_start.c

 Description: we use this file to start or stop minidlnad

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "../func_control_h/minidlnad_control.h"

/*****************************************************************
 Function:     minidlnad_start
 Description:  we use this function to start minidlnad server
 Input:        None
 Output:       None
 Return:       0:start failed 1:start successfully
 Others:       None
*****************************************************************/
int minidlnad_start()
{
    int status = system("minidlnad");

    /* excute successfully */
    if ((-1 != status) && WIFEXITED(status) &&
        (0 == WEXITSTATUS(status)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/*****************************************************************
 Function:     minidlnad_stop
 Description:  we use this function to stop minidlnad server
 Input:        None
 Output:       None
 Return:       0:stop failed 1:stop successfully
 Others:       None
*****************************************************************/
int minidlnad_stop()
{
    int status = system("killall minidlnad");

    /* excute successfully */
    if ((-1 != status) && WIFEXITED(status) &&
        (0 == WEXITSTATUS(status)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

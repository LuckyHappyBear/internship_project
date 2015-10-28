/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   minidlnad_control.h

 Description: we use this file to control minidlnad

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#ifndef _MINIDLNAD_CONTROL_H
#define _MINIDLNAD_CONTROL_H

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************
 Function:     minidlnad_start
 Description:  we use this function to start minidlnad server
 Input:        None
 Output:       None
 Return:       0:start failed 1:start successfully
 Others:       None
*****************************************************************/
int minidlnad_start();

/*****************************************************************
 Function:     minidlnad_stop
 Description:  we use this function to stop minidlnad server
 Input:        None
 Output:       None
 Return:       0:stop failed 1:stop successfully
 Others:       None
*****************************************************************/
int minidlnad_stop();

#ifdef __cplusplus
}
#endif

#endif

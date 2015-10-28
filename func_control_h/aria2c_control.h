/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   aria2_control.h

 Description: This file used to define functions to control aria2

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#ifndef _ARIA2_CONTROL_H
#define _ARIA2_CONTROL_H

#define DEBUG_PRINT 1

#ifdef __cplusplus
extern "C" {
#endif
/*****************************************************************
 Function:     setup_file
 Description:  we use this function to create a new file
 Input:        name: the name of the new file
 Output:       None
 Return:       0:create failed 1:create successfully
 Others:       None
*****************************************************************/
int setup_file(const char *name);

/*****************************************************************
 Function:     delete_file
 Description:  we use this function to delete a file
 Input:        name: the name of the file you want to delete
 Output:       None
 Return:       0:delete successfully 1:delete failed
 Others:       None
*****************************************************************/
int delete_file(const char *name);

/*****************************************************************
 Function:     complete_shell
 Description:  we use this function to complete the shell
 Input:        name: the name of shell file you want to complete
               result: this shell excute result will output to
                       the file named result
               type: the type of the shell
 Output:       None
 Return:       0:create failed 1:create successfully
 Others:       None
*****************************************************************/
int complete_shell(const char *name, const char *result, int type);

/*****************************************************************
 Function:     get_status
 Description:  we use this function to get the download result
 Input:        result: the file named with result store the result
 Output:       None
 Return:       0:download not complete 1:download complete
               -1:function excute wrong
 Others:       None
*****************************************************************/
int get_status(const char *result);

/*****************************************************************
 Function:     aria2c_control
 Description:  we use this function to control download
 Input:        url: the address of the resource
               dir: the location you want to store the resource
               name: the name you give the download resources
               time: the time you give the doenload
               speed: the highest speed you give aria2c
 Output:       None
 Return:       0:download normal but didn't complete
               1:download successful and complete_shell
               -1: there are some problems happened
 Others:       None
*****************************************************************/
int aria2c_control(char *url, char *dir, char *name, int time, char *speed);

#ifdef __cplusplus
}
#endif

#endif

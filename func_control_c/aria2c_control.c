/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   aria2c_control.c

 Description: we use this file to define functions control aria2c

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "../func_control_h/aria2c_control.h"

#define CMD_HEAD_LEN 10
#define CMD_TIME_LEN 6
#define CMD_APPEND_LEN 5
#define CMD_SPEED_LEN 25
#define CMD_DOWNLOAD_RESULT_LEN 25
#define CMD_STOP_LEN 10
#define SUFFIX 4

/*****************************************************************
 Function:     setup_file
 Description:  we use this function to create a new file
 Input:        name: the name of the new file
 Output:       None
 Return:       0:create failed 1:create successfully
 Others:       None
*****************************************************************/
int setup_file(const char *name)
{
    FILE *fp;
    int fd;
    fp = fopen(name, "a+w");
    if (NULL == fp)
    {
        #if DEBUG_PRINT
        printf("setup_file:The file created failed\n");
        #endif
        return 0;
    }

    fd = fileno(fp);
    fchmod(fd, S_IXUSR|S_IRUSR|S_IWUSR);
    fclose(fp);
    return 1;
}

/*****************************************************************
 Function:     delete_file
 Description:  we use this function to delete a file
 Input:        name: the name of the file you want to delete
 Output:       None
 Return:       0:delete successfully 1:delete failed
 Others:       None
*****************************************************************/
int delete_file(const char *name)
{
    return remove(name);
}

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
int complete_shell(const char *name, const char *result, int type)
{
    FILE *fp = fopen(name, "w");
    if (0 == type)
    {
        fprintf(fp, "%s\n%s%s", "#!/bin/bash", "echo 0 > ", result);
        fclose(fp);
        return 1;
    }
    else if (1 == type)
    {
        fprintf(fp, "%s\n%s%s", "#!/bin/bash", "echo 1 > ", result);
        fclose(fp);
        return 1;
    }
    else
    {
        fclose(fp);
        return 0;
    }
}

/*****************************************************************
 Function:     get_status
 Description:  we use this function to get the download result
 Input:        result: the file named with result store the result
 Output:       None
 Return:       0:download not complete 1:download complete
               -1:function excute wrong
 Others:       None
*****************************************************************/
int get_status(const char *result)
{
    FILE *fp;
    int file_size;
    char *status;

    fp = fopen(result, "r");
    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);

    fseek(fp, 0, SEEK_SET );
    status =  malloc(file_size * sizeof(*status));
    fread(status, 1, sizeof(char), fp);

    #if DEBUG_PRINT
    printf("get_status:the status is %s and the length is %lu\n", status,
           strlen(status));
    #endif

    if (strcmp(status, "0") == 0)
    {
        return 0;
    }
    else if (strcmp(status, "1") == 0)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

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
int aria2c_control(char *url, char *dir, char *name, int time, char *speed)
{
    int cmd_len = strlen(url) + strlen(dir) + strlen(name) + CMD_APPEND_LEN * 2
                + CMD_HEAD_LEN + CMD_TIME_LEN + CMD_SPEED_LEN + strlen(speed)
                + CMD_DOWNLOAD_RESULT_LEN * 2 + SUFFIX * 2 + CMD_STOP_LEN;
    int status;
    char *cmd = malloc(sizeof(char) * cmd_len);
    char *time_buffer = malloc(sizeof(char) * CMD_TIME_LEN);
    char *complete_sh = malloc(strlen(name) + SUFFIX);
    char *not_complete_sh = malloc(strlen(name) + SUFFIX);
    char *result = malloc(strlen(name) + SUFFIX);

    memset(cmd, 0, cmd_len);
    memset(time_buffer, 0, CMD_TIME_LEN);
    memset(complete_sh, 0, strlen(complete_sh));
    memset(not_complete_sh, 0, strlen(not_complete_sh));

    strcat(complete_sh, name);
    strcat(complete_sh, ".sh");

    strcat(not_complete_sh, name);
    strcat(not_complete_sh, "1.sh");

    strcat(result, name);
    strcat(result, ".txt");

    if (!setup_file(complete_sh))
    {
        #if DEBUG_PRINT
        printf("aria2c_control:setup complete_sh\n");
        #endif
        return 0;
    }

    if (!setup_file(not_complete_sh))
    {
        #if DEBUG_PRINT
        printf("aria2c_control:setup not_complete_sh\n");
        #endif
        return 0;
    }

    if (!setup_file(result))
    {
        #if DEBUG_PRINT
        printf("aria2c_control:setup result\n");
        #endif
        return 0;
    }

    complete_shell(complete_sh, result, 1);
    complete_shell(not_complete_sh, result, 0);

    sprintf(time_buffer, "%d", time);
    strcat(cmd, "aria2c ");
    strcat(cmd, url);
    strcat(cmd, " -d ");
    strcat(cmd, dir);
    strcat(cmd, " -o ");
    strcat(cmd, name);
    strcat(cmd, " --stop=");
    strcat(cmd, time_buffer);
    strcat(cmd, " --max-overall-download-limit=");
    strcat(cmd, speed);
    strcat(cmd, " --on-download-complete=");
    strcat(cmd, complete_sh);
    strcat(cmd, " --on-download-stop=");
    strcat(cmd, not_complete_sh);

    #if DEBUG_PRINT
    printf("the cmd is %s\n", cmd);
    #endif

    system(cmd);

    status = get_status(result);

    delete_file(result);
    delete_file(complete_sh);
    delete_file(not_complete_sh);

    free(cmd);
    free(result);
    free(complete_sh);
    free(time_buffer);
    free(not_complete_sh);

    if (-1 == status)
    {
        return -1;
    }
    else if (0 == status)
    {
        return 0;
    }
    else if (1 == status)
    {
        return 1;
    }
    else
    {
        return -1;
    }
}

int main()
{
    aria2c_control("http://sourceforge.net/projects/aria2/files/stable/aria2-1.15.2/aria2-1.15.2.tar.gz/download", "/home/luckybear/Download/", "aria.tar.gz", 10, "10K");
    return 0;
}

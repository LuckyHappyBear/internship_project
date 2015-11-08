/*************************************************************************
 Copyright (C), 2015, TP-LINK TECHNOLOGIES CO., LTD.

 File Name:   aria2c_control.c

 Description: we use this file to define functions control aria2c

 Author:      hexiong

 Email:       hexiong@tp-link.net

--------------------------------------

*************************************************************************/

#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "aria2c_control.h"

#define CMD_HEAD_LEN 10
#define CMD_TIME_LEN 6
#define CMD_APPEND_LEN 5
#define CMD_SPEED_LEN 25
#define CMD_DOWNLOAD_RESULT_LEN 25
#define CMD_STOP_LEN 10
#define SUFFIX 4
#define CMD_QUIET_LEN 15
#define CMD_DAEMON_LEN 15
#define PROCESS_LEN 1024
#define GET_PID_CMD_LEN 2048
#define MAX_PID_LEN 5
#define KILL_PID_LEN 15
#define MAX_LINE_LEN 1024
#define MAX_URL_LEN 256
#define MAX_DIR_LEN 256
#define MAX_NAME_LEN 256
#define PATH "/var/www/cgi-bin/record.txt"

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
    FILE *fp;
    int fd;

    fp = fopen(name, "a+w");
    if (NULL == fp)
    {
        #if DEBUG_PRINT
        printf("delete_file:The file open failed\n");
        #endif
        return -1;
    }

    fd = fileno(fp);
    fchmod(fd, S_IXUSR|S_IRUSR|S_IWUSR);
    return remove(name);
}

void sed_replace_handle(const char *note, char **note_add)
{
    int i, j = 0;
    int length = strlen(note);
    for (i = 0; i < length; i++)
    {
        if ('/' == note[i] || '\\' == note[i])
        {
            (*note_add)[j++] = '\\';
            (*note_add)[j++] = note[i];
        }
        else
        {
            (*note_add)[j++] = note[i];
        }
    }
    (*note_add)[j] = '\0';
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
int complete_shell(const char *file_name, const char *url,
                   const char *dir, const char *name)
{
    char *url_handle = malloc(sizeof(char) * strlen(url) * 2);
    char *dir_handle = malloc(sizeof(char) * strlen(dir) * 2);
    char *name_handle = malloc(sizeof(char) * strlen(name) * 2);
    char *cmd = malloc(((strlen(url) + strlen(dir) + strlen(name)) * 2
                        + strlen(PATH) + 16) * 3);
    memset(url_handle, 0, strlen(url));
    memset(dir_handle, 0, strlen(dir));
    memset(name_handle, 0, strlen(name));
    memset(cmd, 0, strlen(cmd));

    FILE *fp = fopen(file_name, "w");
    int fd;
    if (NULL == fp)
    {
        return -1;
    }

    fd = fileno(fp);
    fchmod(fd, S_IXUSR|S_IRUSR|S_IWUSR);

    sed_replace_handle(url, &url_handle);
    sed_replace_handle(dir, &dir_handle);
    sed_replace_handle(name, &name_handle);

    strcat(cmd, "sed -i ");
    strcat(cmd, "\'1,$s/0 ");
    strcat(cmd, url_handle);
    strcat(cmd, " ");
    strcat(cmd, dir_handle);
    strcat(cmd, " ");
    strcat(cmd, name_handle);
    strcat(cmd, "/");
    strcat(cmd, "3 ");
    strcat(cmd, url_handle);
    strcat(cmd, " ");
    strcat(cmd, dir_handle);
    strcat(cmd, " ");
    strcat(cmd, name_handle);
    strcat(cmd, "/");
    strcat(cmd, "g");
    strcat(cmd, "\' ");
    strcat(cmd, PATH);
    #if DEBUG_PRINT
    printf("complete_shell:the cmd is %s\n", cmd);
    #endif

    if (fprintf(fp, "%s\n%s", "#!/bin/bash", cmd) < 0)
    {
        #if DEBUG_PRINT
        printf("complete_shell:An error occour\n");
        #endif
        fclose(fp);
        free(cmd);
        free(url_handle);
        free(dir_handle);
        free(name_handle);
        return -1;
    }
    fclose(fp);
    free(cmd);
    free(url_handle);
    free(dir_handle);
    free(name_handle);
    return 1;
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
int get_status(const char *url, const char *dir, const char *name)
{
    int is_run;
    int len;
    int count;
    int time;
    int line_num = 1;
    char status;
    char lineno_buffer[4];
    char url_get[MAX_URL_LEN];
    char dir_get[MAX_DIR_LEN];
    char name_get[MAX_NAME_LEN];
    char line[MAX_LINE_LEN];
    char file_name[512];

    is_run = get_pid(url, dir, name);
    if (is_run == -1)
    {
        #if DEBUG_PRINT
        printf("get_status:the process has been stoped\n");
        #endif
    	memset(url_get, 0, MAX_URL_LEN);
    	memset(dir_get, 0, MAX_DIR_LEN);
    	memset(name_get, 0, MAX_NAME_LEN);
   	    memset(line, 0, MAX_LINE_LEN);
   	    memset(lineno_buffer, 0, 4);
        memset(file_name, 0, 512);

    	FILE *fp = fopen(PATH, "a+w");
    	if (NULL == fp)
    	{
            #if DEBUG_PRINT
            printf("get_status:open file failed\n");
            #endif
    	    return -1;
    	}

        while (fgets(line, MAX_LINE_LEN, fp))
        {
            status = line[0];
            len = 0;
            count = 2;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                url_get[len ++] = line[count];
            }
            url_get[len] = '\0';
            count ++;
            len = 0;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                dir_get[len ++] = line[count];
            }
            dir_get[len] = '\0';
            count ++;
            len = 0;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                name_get[len ++] = line[count];
            }
            name_get[len] = '\0';
            count ++;
            len = 0;

            #if DEBUG_PRINT
            printf("get_status:begin to compare\n");
            #endif
            #if DEBUG_PRINT
            printf("+++++++++++++++++++++++++++++++++++++++\n");
            printf("url is %s\nurl_get is %s\n", url, url_get);
            printf("dir is %s\ndir_get is %s\n", dir, dir_get);
            printf("name is %s\nname_get is %s\n", name, name_get);
            printf("+++++++++++++++++++++++++++++++++++++++\n");
            #endif
            if (strcmp(url, url_get) == 0 && strcmp(dir, dir_get) == 0
                && strcmp(name, name_get) == 0)
            {
                #if DEBUG_PRINT
                printf("get_status:match and status is %c\n", status);
                #endif

                if (status == '0')
                {
                    strcat(file_name, dir_get);
                    strcat(file_name, name);
                    strcat(file_name, ".aria2");
                    #if DEBUG_PRINT
                    printf("get_status:the file_name is %s\n", file_name);
                    #endif

                    if (access(file_name, F_OK) == 0)
                    {
                        #if DEBUG_PRINT
                        printf("get_status:download exception\n");
                        #endif
                        modify_status(url, dir, name, PATH, 1);
                        return -1;
                    }
                    else
                    {
                        if (modify_status(url, dir, name, PATH, 2) == -1)
                        {
                            #if DEBUG_PRINT
                            printf("get_status:modify exception\n");
                            #endif
                            return -1;
                        }
                        else
                        {
                            #if DEBUG_PRINT
                            printf("get_status:download successfully\n");
                            #endif
                            return 1;
                        }
                    }
                }
            }

            count = 0;
            line_num++;
            memset(url_get, 0, MAX_URL_LEN);
            memset(dir_get, 0, MAX_DIR_LEN);
            memset(name_get, 0, MAX_NAME_LEN);
            memset(line, 0, MAX_LINE_LEN);
        }
    }
    else
    {
        return 0;
    }
}

int get_pid(const char *url, const char *dir, const char *name)
{
    FILE *stream;
    int pid;
    int i;
    int read_succeed;
    char pid_buffer[MAX_PID_LEN];
    char buf[PROCESS_LEN];
    char cmd[GET_PID_CMD_LEN];

    memset(buf, 0, PROCESS_LEN);
    memset(cmd, 0, GET_PID_CMD_LEN);

    strcat(cmd, "ps ax | grep 'aria2c ");
    strcat(cmd, url);
    strcat(cmd, " -d ");
    strcat(cmd, dir);
    strcat(cmd, " -o ");
    strcat(cmd, name);
    strcat(cmd, "' | grep -v grep");

    stream = popen(cmd, "r");
    /* call popen() failed */
    if (NULL == stream)
    {
        return -1;
    }

    read_succeed = fread(buf, sizeof(char), sizeof(buf), stream);

    #if DEBUG_PRINT
    printf("get_pid:The buf is %s\n", buf);
    printf("get_pid:the value of read_succeed is %d\n", read_succeed);
    #endif

    if (0 == read_succeed)
    {
        return -1;
    }

    for (i = 0; i < sizeof(buf); i ++)
    {
        if (isspace(buf[i]) && i != 0)
        {
            break;
        }
        else
        {
            pid_buffer[i] = buf[i];
        }
    }

    pid = atoi(pid_buffer);

    pclose(stream);

    return pid;
}

int aria2c_store(char *url, char *dir, char *name, char *time, char *speed, char *path)
{
    #if DEBUG_PRINT
    printf("aria2c_store:start here\n");
    #endif

    int line_len = strlen(url) + strlen(dir) + strlen(name) + strlen(time)
                   + strlen(speed) + 8;
    char *line = malloc(sizeof(char) * line_len);
    memset(line, 0, line_len);

    FILE *fp = fopen(path, "a+w");
    if (NULL == fp)
    {
        #if DEBUG_PRINT
        printf("aria2c_store:we set up or open file failed here\n");
        #endif
        return -1;
    }

    strcat(line, "0");
    strcat(line, " ");
    strcat(line, url);
    strcat(line, " ");
    strcat(line, dir);
    strcat(line, " ");
    strcat(line, name);
    strcat(line, " ");
    strcat(line, time);
    strcat(line, " ");
    strcat(line, speed);

    printf("The line is %s\n", line);

    fseek(fp, 0, SEEK_END);
    fprintf(fp, "%s\n", line);
    fclose(fp);
    free(line);
    return 1;
}

int modify_status(const char *url, const char *dir, const char *name, const char *path, int type)
{
    int len;
    int count;
    int succeed;
    int line_num = 1;
    char status;
    char lineno_buffer[4];
    char url_get[MAX_URL_LEN];
    char dir_get[MAX_DIR_LEN];
    char name_get[MAX_NAME_LEN];
    char line[MAX_LINE_LEN];
    char cmd[512];

    memset(url_get, 0, MAX_URL_LEN);
    memset(dir_get, 0, MAX_DIR_LEN);
    memset(name_get, 0, MAX_NAME_LEN);
    memset(line, 0, MAX_LINE_LEN);
    memset(lineno_buffer, 0, 4);
    memset(cmd, 0, 512);

    FILE *fp = fopen(path, "a+w");
    if (NULL == fp)
    {
       return -1;
    }

    while (fgets(line, MAX_LINE_LEN, fp))
    {
        status = line[0];
        len = 0;
        count = 2;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            url_get[len ++] = line[count];
        }
        url_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            dir_get[len ++] = line[count];
        }
        dir_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            name_get[len ++] = line[count];
        }
        name_get[len] = '\0';
        count ++;
        len = 0;

        if (strcmp(url, url_get) == 0 && strcmp(dir, dir_get) == 0
            && strcmp(name, name_get) == 0)
        {
            #if DEBUG_PRINT
            printf("the line number is %d\n",line_num);
            #endif

            if (type != 2)
            {
                line[0] = type + '0';
                sprintf(lineno_buffer, "%d", line_num);
                strcat(cmd, "sed -i ");
                strcat(cmd, "\"");
                strcat(cmd, lineno_buffer);
                strcat(cmd, "c");
                strcat(cmd, line);
                strcat(cmd, "\" ");
                strcat(cmd, "\"");
                strcat(cmd, path);
                strcat(cmd, "\"");
            }
            else
            {
                sprintf(lineno_buffer, "%d", line_num);
                strcat(cmd, "sed -i ");
                strcat(cmd, "\"");
                strcat(cmd, lineno_buffer);
                strcat(cmd, "d");
                strcat(cmd, "\" ");
                strcat(cmd, "\"");
                strcat(cmd, path);
                strcat(cmd, "\"");
            }

            #if DEBUG_PRINT
            printf("modify_status:the cmd is %s\n", cmd);
            #endif

            succeed = system(cmd);
            memset(cmd, 0, 512);
            if ((-1 == succeed) || !(WIFEXITED(succeed))
                ||(0 != WEXITSTATUS(succeed)))
            {
                #if DEBUG_PRINT
                printf("modify_status:The system call is failed\n");
                #endif
                return -1;
            }
            fclose(fp);
            return 1;
        }

        count = 0;
        line_num++;
        memset(url_get, 0, MAX_URL_LEN);
        memset(dir_get, 0, MAX_DIR_LEN);
        memset(name_get, 0, MAX_NAME_LEN);
        memset(line, 0, MAX_LINE_LEN);
    }
    return -1;
}

/*****************************************************************
 Function:     aria2c_start
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
int aria2c_start(char *url, char *dir, char *name, int time, char *speed, int type)
{
    int cmd_len = strlen(url) + strlen(dir) + strlen(name) + CMD_APPEND_LEN * 2
                + CMD_HEAD_LEN + CMD_TIME_LEN + CMD_SPEED_LEN + strlen(speed)
                + CMD_DOWNLOAD_RESULT_LEN * 2 + SUFFIX * 2 + CMD_STOP_LEN
                + CMD_QUIET_LEN;
    int status;
    int call_succeed;
    int store_succeed;
    char *cmd = malloc(sizeof(char) * cmd_len);
    char *time_buffer = malloc(sizeof(char) * CMD_TIME_LEN);
    /*char *complete_sh = malloc(strlen(name) + SUFFIX);
    char *not_complete_sh = malloc(strlen(name) + SUFFIX);
    char *result = malloc(strlen(name) + SUFFIX);*/

    memset(cmd, 0, cmd_len);
    memset(time_buffer, 0, CMD_TIME_LEN);
    /*memset(complete_sh, 0, strlen(complete_sh));
    memset(not_complete_sh, 0, strlen(not_complete_sh));*/

    /*strcat(complete_sh, name);
    strcat(complete_sh, ".sh");

    strcat(not_complete_sh, name);
    strcat(not_complete_sh, "1.sh");

    strcat(result, name);
    strcat(result, ".txt");*/

    /*if (!setup_file(complete_sh))
    {
        #if DEBUG_PRINT
        printf("aria2c_start:setup complete_sh\n");
        #endif
        return -1;
    }

    if (!setup_file(not_complete_sh))
    {
        #if DEBUG_PRINT
        printf("aria2c_start:setup not_complete_sh\n");
        #endif
        return -1;
    }

    if (!setup_file(result))
    {
        #if DEBUG_PRINT
        printf("aria2c_start:setup result\n");
        #endif
        return -1;
    }*/

    #if DEBUG_PRINT
    printf("aria2c_start:we start store record here\n");
    #endif
    if (type)
    {
        store_succeed = aria2c_store(url, dir, name, time_buffer, speed, PATH);
        if (store_succeed == -1)
        {
            #if DEBUG_PRINT
            printf("aria2c_start:store_failed\n");
            #endif
            return -1;
        }

        #if DEBUG_PRINT
        printf("aria2c_start:we finish store record here\n");
        #endif
    }

    /*complete_shell(complete_sh, url, dir, name);
    complete_shell(not_complete_sh, result, 0);*/

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
    /*strcat(cmd, " --on-download-complete=");
    strcat(cmd, complete_sh);
    strcat(cmd, " --on-download-stop=");
    strcat(cmd, not_complete_sh);*/
    strcat(cmd, " --daemon=true");
    //strcat(cmd, " &"); 

    #if DEBUG_PRINT
    printf("aria2c_start:the cmd is %s\n", cmd);
    #endif

    call_succeed = system(cmd);
    memset(cmd, 0, cmd_len);
    if ((-1 == call_succeed) || !(WIFEXITED(call_succeed))
        ||(0 != WEXITSTATUS(call_succeed)))
    {
        #if DEBUG_PRINT
        printf("aria2c_start:system call failed\n");
        #endif
        return -1;
    }

    /* system call failed */
    if ((-1 == call_succeed) || !(WIFEXITED(call_succeed))
        ||(0 != WEXITSTATUS(call_succeed)))
    {
        #if DEBUG_PRINT
        printf("aria2c_start:system call failed\n");
        #endif
        return -1;
    }

    //status = get_status(result);

    //delete_file(result);
    //delete_file(complete_sh);
    //delete_file(not_complete_sh);

    free(cmd);
    //free(result);
    //free(complete_sh);
    free(time_buffer);
    //free(not_complete_sh);

    /*if (-1 == status)
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
    }*/
    return 1;
}

int aria2c_stop(char *url, char *dir, char *name)
{
    int succeed;
    int pid = get_pid(url, dir, name);
    char cmd[KILL_PID_LEN];
    char pid_buffer[MAX_PID_LEN];
    memset(cmd, 0, KILL_PID_LEN);
    memset(pid_buffer, 0, MAX_PID_LEN);

    sprintf(pid_buffer, "%d", pid);

    strcat(cmd, "kill ");
    strcat(cmd, pid_buffer);

    #if DEBUG_PRINT
    printf("aria2c_stop:The pid is %d\n", pid);
    #endif

    succeed = system(cmd);

    #if DEBUG_PRINT
    printf("aria2c_stop:The cmd is %s\n", cmd);
    #endif

    /* system call failed */
    if ((-1 == succeed) || !(WIFEXITED(succeed))
        ||(0 != WEXITSTATUS(succeed)))
    {
        #if DEBUG_PRINT
        printf("aria2c_stop:the system call failed\n");
        #endif

        return -1;
    }
    return modify_status(url, dir, name, PATH, 1);
}

int aria2c_restart(char *url, char *dir, char *name)
{
    int len;
    int count;
    int time;
    int line_num = 1;
    char status;
    char lineno_buffer[4];
    char url_get[MAX_URL_LEN];
    char dir_get[MAX_DIR_LEN];
    char name_get[MAX_NAME_LEN];
    char time_get[CMD_TIME_LEN];
    char speed_get[CMD_SPEED_LEN];
    char line[MAX_LINE_LEN];

    memset(url_get, 0, MAX_URL_LEN);
    memset(dir_get, 0, MAX_DIR_LEN);
    memset(name_get, 0, MAX_NAME_LEN);
    memset(line, 0, MAX_LINE_LEN);
    memset(lineno_buffer, 0, 4);
    memset(time_get, 0, CMD_TIME_LEN);

    FILE *fp = fopen(PATH, "a+w");
    if (NULL == fp)
    {
        return -1;
    }

    while (fgets(line, MAX_LINE_LEN, fp))
    {
        status = line[0];
        len = 0;
        count = 2;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            url_get[len ++] = line[count];
        }
        url_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            dir_get[len ++] = line[count];
        }
        dir_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            name_get[len ++] = line[count];
        }
        name_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            time_get[len ++] = line[count];
        }
        time_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            speed_get[len ++] = line[count];
        }
        speed_get[len] = '\0';
        count ++;
        len = 0;

        if (strcmp(url, url_get) == 0 && strcmp(dir, dir_get) == 0
            && strcmp(name, name_get) == 0)
        {
            time = atoi(time_get);
            if (modify_status(url, dir, name, PATH, 0) == -1)
            {
                return -1;
            }
            else
            {
                if (status == '1')
                {
                    return aria2c_start(url, dir, name, time, speed_get, 0);
                }
                else
                {
                    return -1;
                }
            }
        }

        count = 0;
        line_num++;
        memset(url_get, 0, MAX_URL_LEN);
        memset(dir_get, 0, MAX_DIR_LEN);
        memset(name_get, 0, MAX_NAME_LEN);
        memset(line, 0, MAX_LINE_LEN);
    }
}

int aria2c_delete(char *url, char *dir, char *name)
{
    int len;
    int count;
    int time;
    int is_stop = -1;
    char status;
    char file_download[MAX_NAME_LEN];
    char file_aria2[MAX_NAME_LEN];
    char url_get[MAX_URL_LEN];
    char dir_get[MAX_DIR_LEN];
    char name_get[MAX_NAME_LEN];
    char line[MAX_LINE_LEN];

    memset(url_get, 0, MAX_URL_LEN);
    memset(dir_get, 0, MAX_DIR_LEN);
    memset(name_get, 0, MAX_NAME_LEN);
    memset(line, 0, MAX_LINE_LEN);
    memset(file_aria2, 0, MAX_NAME_LEN);
    memset(file_download, 0, MAX_NAME_LEN);

    FILE *fp = fopen(PATH, "a+w");
    if (NULL == fp)
    {
        return -1;
    }

    while (fgets(line, MAX_LINE_LEN, fp))
    {
        status = line[0];
        len = 0;
        count = 2;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            url_get[len ++] = line[count];
        }
        url_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            dir_get[len ++] = line[count];
        }
        dir_get[len] = '\0';
        count ++;
        len = 0;

        for (;line[count] != ' ' && line[count] != '\n'; count ++)
        {
            name_get[len ++] = line[count];
        }
        name_get[len] = '\0';
        count ++;
        len = 0;

        if (strcmp(url, url_get) == 0 && strcmp(dir, dir_get) == 0
            && strcmp(name, name_get) == 0)
        {
            strcat(file_download, dir);
            strcat(file_download, name);
            #if DEBUG_PRINT
            printf("aria2c_delete:the path of download file is %s\n",
                   file_download);
            #endif

            strcat(file_aria2, file_download);
            strcat(file_aria2, ".aria2");
            #if DEBUG_PRINT
            printf("aria2c_delete:the path of *.aria2 is %s\n", file_aria2);
            #endif

            delete_file(file_download);
            delete_file(file_aria2);

            if (status == '1' || status == '2')
            {
                is_stop = 1;
                break;
            }
            else
            {
                is_stop = 0;
                break;
            }
        }
        count = 0;
        memset(url_get, 0, MAX_URL_LEN);
        memset(dir_get, 0, MAX_DIR_LEN);
        memset(name_get, 0, MAX_NAME_LEN);
        memset(line, 0, MAX_LINE_LEN);
    }

    if (is_stop == -1)
    {
        return -1;
    }

    if (!is_stop)
    {
        if (aria2c_stop(url, dir, name) == -1)
        {
            return -1;
        }
    }

    return modify_status(url, dir, name, PATH, 2);
}

int network_manager()
{
    int len;
    int count;
    int succeed;
    int line_num = 1;
    char status;
    char lineno_buffer[4];
    char url_get[MAX_URL_LEN];
    char dir_get[MAX_DIR_LEN];
    char name_get[MAX_NAME_LEN];
    char time_get[CMD_TIME_LEN];
    char speed_get[CMD_SPEED_LEN];
    char line[MAX_LINE_LEN];
    char cmd[512];

    memset(url_get, 0, MAX_URL_LEN);
    memset(dir_get, 0, MAX_DIR_LEN);
    memset(name_get, 0, MAX_NAME_LEN);
    memset(line, 0, MAX_LINE_LEN);
    memset(lineno_buffer, 0, 4);
    memset(cmd, 0, 512);

    succeed = system("killall aria2c");
    /* kill process failed */
    if ((-1 == succeed) || !(WIFEXITED(succeed))
        ||(0 != WEXITSTATUS(succeed)))
    {
        #if DEBUG_PRINT
        printf("network_manager:system call killall failed\n");
        #endif
        return -1;
    }

    FILE *fp = fopen(PATH, "a+w");
    if (NULL == fp)
    {
       return -1;
    }

    while (fgets(line, MAX_LINE_LEN, fp))
    {
        status = line[0];
        len = 0;
        count = 2;

        if (status == '0')
        {
            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                url_get[len ++] = line[count];
            }
            url_get[len] = '\0';
            count ++;
            len = 0;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                dir_get[len ++] = line[count];
            }
            dir_get[len] = '\0';
            count ++;
            len = 0;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                name_get[len ++] = line[count];
            }
            name_get[len] = '\0';
            count ++;
            len = 0;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                time_get[len ++] = line[count];
            }
            time_get[len] = '\0';
            count ++;
            len = 0;

            for (;line[count] != ' ' && line[count] != '\n'; count ++)
            {
                speed_get[len ++] = line[count];
            }
            speed_get[len] = '\0';
            count ++;
            len = 0;

            memset(line, 0, MAX_LINE_LEN);
            line[0] = '0';
            strcat(line, " ");
            strcat(line, url_get);
            strcat(line, " ");
            strcat(line, dir_get);
            strcat(line, " ");
            strcat(line, name_get);
            strcat(line, " ");
            strcat(line, time_get);
            strcat(line, " ");
            strcat(line, "15K");
            strcat(line, "\n");

            #if DEBUG_PRINT
            printf("network_manager:the line number is %d\n",line_num);
            #endif
            sprintf(lineno_buffer, "%d", line_num);
            strcat(cmd, "sed -i ");
            strcat(cmd, "\"");
            strcat(cmd, lineno_buffer);
            strcat(cmd, "c");
            strcat(cmd, line);
            strcat(cmd, "\" ");
            strcat(cmd, "\"");
            strcat(cmd, PATH);
            strcat(cmd, "\"");

            #if DEBUG_PRINT
            printf("network_manager:the cmd is %s\n", cmd);
            #endif
            succeed = system(cmd);
            memset(cmd, 0, 512);
            if ((-1 == succeed) || !(WIFEXITED(succeed))
                ||(0 != WEXITSTATUS(succeed)))
            {
                #if DEBUG_PRINT
                printf("network_manager:system call sed failed\n");
                #endif
                return -1;
            }

            if (aria2c_start(url_get, dir_get, name_get, atoi(time_get),
                             "15K", 0) == -1)
            {
                #if DEBUG_PRINT
                printf("network_manager:call aria2c_start failed\n");
                #endif
                return -1;
            }
            else
            {
                #if DEBUG_PRINT
                printf("network_manager:call aria2c_start succeed\n");
                #endif
                count = 0;
                line_num++;
                memset(url_get, 0, MAX_URL_LEN);
                memset(dir_get, 0, MAX_DIR_LEN);
                memset(name_get, 0, MAX_NAME_LEN);
                memset(time_get, 0, CMD_TIME_LEN);
                memset(speed_get, 0, CMD_SPEED_LEN);
                memset(line, 0, MAX_LINE_LEN);
                continue;
            }
        }
        else
        {
            count = 0;
            line_num++;
            memset(url_get, 0, MAX_URL_LEN);
            memset(dir_get, 0, MAX_DIR_LEN);
            memset(name_get, 0, MAX_NAME_LEN);
            memset(time_get, 0, CMD_TIME_LEN);
            memset(speed_get, 0, CMD_SPEED_LEN);
            memset(line, 0, MAX_LINE_LEN);
            continue;
        }
    }
    fclose(fp);
    return 1;
}

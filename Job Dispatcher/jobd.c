#include "jobd.h"
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <time.h>

const char *jobd_fifo = "./EPL222_HW1/comm";
const char *jobd_in_dir = "./EPL222_HW1/jobs/";
const char *jobd_out_dir = "./EPL222_HW1/out/";
const char *jobd_main_dir = "./EPL222_HW1/";

static int job_id = 1;

int jobd_daemonize(void)
{
    pid_t pid;

    pid = fork();
    /* Error in creation of new child */
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    /* Exit the parent */
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    /* Create new SID for the child process */
    setsid();

    /* Change the current working directory */
    chdir("./");

    /* Change the file mode mask */
    umask(0);

    close(STDIN_FILENO);
    close(STDERR_FILENO);

    return EXIT_SUCCESS;
}

void jobd_init(void)
{
    job_id = 1;

    /* Create directories if they don't exist */
    mkdir(jobd_main_dir, 0777);
    mkdir(jobd_in_dir, 0777);
    mkdir(jobd_out_dir, 0777);

    /* Create the named pipe */
    mkfifo(jobd_fifo, 0777);

    signal(SIGALRM, jobd_dispatcher);

    jobd_daemonize();

    /* printf("Deamon created!\n"); */

    /* Schedule alarm value */
    alarm(POLL_JOB_INTERVAL);
}

void jobd_finalize(void)
{
    char deleteOut[100];
    /*char deleteComm[100];*/

    /* Delete all the files in the out directory */
    /*sprintf(deleteComm, "exec rm -f %s",jobd_fifo);*/
    sprintf(deleteOut, "exec rm -f %s*", jobd_out_dir);

    system(deleteOut);
    /*system(deleteComm);*/
    exit(0);
}
void jobd_dir_check(const char *dir)
{
    /* printf("Directory: %s\n", dir); */
    /* Open directory */
    DIR *dirp = opendir(dir);
    struct dirent *dp;
    if (dirp)
    {
        /* Print all the files in the directory */
        while ((dp = readdir(dirp)) != NULL)
        {
            if (strcmp(dp->d_name, ".") == 0 || strcmp(dp->d_name, "..") == 0)
            {
                continue;
            }
            jobd_dispatch(dp->d_name);
        }
        /* printf("Done\n"); */
        closedir(dirp);
    }
}

void jobd_dispatcher(int sig)
{
    time_t t;
    time(&t);
    /* printf("Check: %s\n",ctime(&t)); */

    jobd_dir_check(jobd_in_dir);
    /* Schedule alarm value */
    alarm(POLL_JOB_INTERVAL);
}

int jobd_dispatch(char *jobname)
{
    /* printf("Filename: %s\n", jobname); */

    char exe_command[100];
    char job_command[100];
    int status_exe;

    sprintf(job_command, "%s%s", jobd_in_dir, jobname);

    /* Give execute permissions to the file */
    chmod(job_command, 0777);

    sprintf(exe_command, "%s%s &> %s%s%d.txt", jobd_in_dir, jobname, jobd_out_dir, JOBD_PREFIX, job_id);

    /* printf("Job command: %s\n", job_command); */

    /* Execute the file */
    /* printf("Execution command: %s\n\n", exe_command); */

    status_exe = system(exe_command);

    job_id++;
    if (job_id == JOBD_QUEUE_MAX)
    {
        job_id = 0;
    }

    /* Delete the original script from job_in_dir */
    remove(job_command);

    return status_exe;
}

void jobd_handle_msg(char *msg)
{
    char delim[2];
    int num;
    char readOutput[100];
    char *ptr;

    strcpy(delim, "\n");

    if (strcmp(msg, "") != 0)
    {
        ptr = strtok(msg, delim);
        while (ptr != NULL && strlen(ptr) > 2)
        {
            /* printf("%s\n",ptr); */
            sscanf(ptr, "S:%d", &num);

            sprintf(readOutput, "cat %sjobd__out__%d.txt", jobd_out_dir, num);
            /* printf("Read output: %s\n",readOutput); */

            system(readOutput);
            ptr = strtok(NULL, delim);
        }
    }
}

int main()
{
    int quit;
    int fp;
    char fifo_msg[80];

    jobd_init();

    /* Set up alarm handler */
    quit = 0;

    /* Open FIFO in read only mode*/
    fp = open(jobd_fifo, O_RDONLY);

    while (!quit)
    {

        /* time_t t;
        time(&t);
        printf("FIFO: %s\n",ctime(&t)); */

        sleep(POLL_CMD_INTERVAL);

        strcpy(fifo_msg, "Start");
        while (strcmp(fifo_msg, "") != 0)
        {
            strcpy(fifo_msg, "");
            /* Read FIFO */
            read(fp, fifo_msg, 80);
            if (fifo_msg[0] == 'Q' && fifo_msg[1] == ':' && fifo_msg[2] == '\n')
            {
                quit = 1;
                break;
            }
            else
            {
                /* Print the read string and close */
                /* printf("FIFO: %s\n",fifo_msg); */
                jobd_handle_msg(fifo_msg);
            }
        }
    }
    close(fp);
    /* Do not proceed until signal is handledAlarm clock */
    while (!quit)
    {
        pause();
    }

    jobd_finalize();
    /* printf("Deamon is dead!\n"); */

    return 0;
}

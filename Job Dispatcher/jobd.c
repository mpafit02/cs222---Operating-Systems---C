#include "jobd.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

const char *jobd_fifo = "/EPL222_HW1/comm";
const char *jobd_in_dir = "/EPL222_HW1/jobs/";
const char *jobd_out_dir = "/EPL222_HW1/out/";
const char *jobd_main_dir = "/EPL222_HW1/";

int jobd_daemonize(void)
{
    pid_t pid, sid;

    pid = fork();
    // Error in creation of new child
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Exit the parent
    if (pid == 0)
    {
        exit(EXIT_SUCCESS);
    }

    printf("child pid: %d\n", pid);

    // CHange the file mode mask
    umask(0);

    // Create new SID for the child process
    sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Change the current working directory
    if ((chdir("/")) < 0)
    {
        exit(EXIT_FAILURE);
    }

    // CLose standard file descriptors
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Deamon initialization

    // The loop
    while (1)
    {
        // Do something

        sleep(1); // wait for 1 second
    }
    exit(EXIT_SUCCESS);
}

void jobd_dir_check(const char *dir)
{
    // oOpen directory
    DIR *dirp = opendir(*jobd_in_dir);

    if (dirp)
    {
        errno = 0;
        // Print all the files in the directory
        while ((dp = readdir(dirp)) != NULL)
        {
            // Print the filename
            printf("%s\n", dp->d_name);
            // Execute the file
            int status = system(strcat("./", dp->d_name));
        }
        closedir(dirp);
    }

    sleep(POLL_JOB_INTERVAL); // wait for POLL_JOB_INTERVAL seconds
}

void jobd_init(void)
{
    // Create directories if they don't exist
    int jobs = mkdir("/EPL222_HW1/jobs/", 0777);
    int out = mkdir("/EPL222_HW1/out/", 0777);
    int comm = mkdir("/EPL222_HW1/comm.txt", 0777);

    jobd_daemonize();
}

void jobd_finalize(void)
{
    // Delete all the files in the out directory
    system("exec rm -f /EPL222_HW1/out/*")
}

int main()
{
}
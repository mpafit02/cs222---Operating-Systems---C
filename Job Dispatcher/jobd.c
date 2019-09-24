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

int main()
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
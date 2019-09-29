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

const char *jobd_fifo = "./EPL222_HW1/comm.txt";
const char *jobd_in_dir = "./EPL222_HW1/jobs/";
const char *jobd_out_dir = "./EPL222_HW1/out/";
const char *jobd_main_dir = "./EPL222_HW1/";

static int job_id = 0;

int jobd_daemonize(void)
{
    pid_t pid;

    pid = fork();
    // Error in creation of new child
    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    // Exit the parent
    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    // Create new SID for the child process
    setsid();

    // Change the current working directory
    chdir("./");

    // Change the file mode mask
    umask(0);

    // CLose standard file descriptors
    //close(STDIN_FILENO);
    //close(STDOUT_FILENO);
    //close(STDERR_FILENO);

    return EXIT_SUCCESS;
}

void jobd_dir_check(const char *dir)
{
    // printf("Directory: %s\n", dir);
    // Open directory
    DIR *dirp = opendir(dir);
    struct dirent *dp;
    if (dirp)
    {
        // Print all the files in the directory
        while ((dp = readdir(dirp)) != NULL)
        {
	    if(strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0){
	       continue;
            }
            jobd_dispatch(dp->d_name);
        }
        // printf("Done\n");
        closedir(dirp);
    }

}

void jobd_dispatcher(int sig){
    printf("Check\n");
    jobd_dir_check(jobd_in_dir);
    // Schedule alarm value
    alarm(POLL_JOB_INTERVAL);
}

int jobd_dispatch(char *jobname){
    //printf("Filename: %s\n", jobname);

    char com[100] = "";
    strcpy(com, jobd_in_dir);
    strcat(com, jobname);

    // Give execute permissions to the file
    chmod(com, 0777);

    char command[100] = "";
    strcat(command,"sh ");
    strcat(command,com);

    // Execute the file
    strcat(command, " ./out > ");
    strcat(command, jobd_out_dir);
    strcat(command, JOBD_PREFIX);
    char buffer[20] = "";            
    sprintf(buffer,"%d",job_id);
    strcat(command, buffer);
    strcat(command, ".txt");

    // printf("Execution command: %s\n\n", command);

    int status_exe = system(command);

    job_id++;

    // Delete the original script from job_in_dir
    remove(com);

    return status_exe;
}

void jobd_init(void)
{

    job_id = 0;

    // Create directories if they don't exist
    mkdir(jobd_main_dir, 0777);
    mkdir(jobd_in_dir, 0777);
    mkdir(jobd_out_dir, 0777);

    // Create the named pipe
    mkfifo(jobd_fifo,0777);
    
    // jobd_daemonize();
    // printf("Deamon created!\n");
}

void jobd_finalize(void)
{
    // Delete all the files in the out directory
    char deleteCommand[100] ="exec rm -f ";
    strcat(deleteCommand, jobd_out_dir);
    strcat(deleteCommand, "*");
    system(deleteCommand);
}

int main()
{
    jobd_init();
    
    // Set up alarm handler
    signal(SIGALRM, jobd_dispatcher);

    // Schedule alarm value
    alarm(POLL_JOB_INTERVAL);

    // Do not proceed until signal is handledAlarm clock
    while(1){
        pause();
    }
    jobd_finalize();
    return 0;
}





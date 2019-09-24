/* jobd API */
/* Constants. */

#ifndef JOBD_H
#define JOBD_H

const char *jobd_fifo = "/path/1/comm";
const char *jobd_in_dir = "/path/1/jobs/";
const char *jobd_out_dir = "/path/1/out/";
const char *jobd_main_dir = "/path/1/";

#define POLL_JOB_INTERVAL 10 // Poll for waiting jobs every 10 secs.
#define POLL_CMD_INTERVAL 5  // Poll for status cmds every 5 secs.
#define JOBD_PREFIX "jobd__out__"
#define JOBD_QUEUE_MAX 500
/*                                                                    
 Initialization routine. Create needed directories if they don't      
 exist, establish any needed signal handlers, establish the named pip 
 external communication and call jobd_daemonize() to become a         
 daemon.                                                              
*/
void jobd_init(void);
/*
 Clean up and exit. You should remove everything done in
 jobd_out_dir.
*/
void jobd_finalize(void);
/*
 Become a daemon.
*/
int jobd_daemonize(void);
/*
 Signal handler, which should be invoked every
 POLL_JOB_INTERVAL seconds. It should call jobd_check_dir()
 in jobd_in_dir to check for available jobs.
*/
void jobd_dispatcher(int sig);
/*
 Check a directory for available jobs (scripts). If a script
 is found, call jobd_dispatch to execute the script.
*/
void jobd_dir_check(const char *dir);
/*
 Dispatch a job (script). Build a command string which should
 execute the script and redirect the output to a unique file
 in jobd_out_dir. Use system() to execute the command string.
 After a correct execution, you should delete the original
 script from jobd_in_dir.
*/
int jobd_dispatch(char *jobname);
/*
 Handle an incoming message from jobd_fifo. This function should
 be called every PALL_CMD_INTERVAL.
*/
void jobd_handle_msg(char *msg);

#endif
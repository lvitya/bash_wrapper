#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// for now LOG_FILE is statically located in current directory
#define LOG_FILE "bash_log"

/**
 * Get a process name from its PID.
 * @param pid PID of the process
 * @param name Name of the process
 * 
 * Source: http://stackoverflow.com/questions/15545341/process-name-from-its-pid-in-linux
 */
static void get_process_name(const pid_t pid, char * name) {
	char procfile[BUFSIZ];
	sprintf(procfile, "/proc/%d/cmdline", pid);
	FILE* f = fopen(procfile, "r");
	if (f) {
		size_t size;
		size = fread(name, sizeof (char), sizeof (procfile), f);
		if (size > 0) {
			if ('\n' == name[size - 1])
				name[size - 1] = '\0';
		}
		fclose(f);
	}
}

/**
 * Get a process cmdline from its PID.
 * @param pid PID of the process
 * @param cmdline Cmdline of the process
 *
 * Source: http://stackoverflow.com/questions/821837/how-to-get-the-command-line-args-passed-to-a-running-process-on-unix-linux-syste
 * http://stackoverflow.com/questions/14393762/executing-bash-command-and-getting-the-output-in-c
 */
static void get_process_cmdline(const pid_t pid, char * cmdline) {
        char cmdstring[BUFSIZ];

        // command to convert string from /proc/%d/cmdline
        sprintf(cmdstring, "xargs -0 < /proc/%d/cmdline", pid);

        FILE* pipe = popen(cmdstring, "r");
        if (pipe) {
                size_t size;
                size = fread(cmdline, sizeof (char), sizeof (cmdstring), pipe);
                if (size > 0) {
                        if ('\n' == cmdline[size - 1])
                                cmdline[size - 1] = '\0';
                }
                pclose(pipe);
        }
}

/**
 * Get the parent PID from a PID
 * @param pid pid
 * @param ppid parent process id
 * 
 * Note: init is 1 and it has a parent id of 0.
 */
static void get_process_parent_id(const pid_t pid, pid_t * ppid) {
	char buffer[BUFSIZ];
	sprintf(buffer, "/proc/%d/stat", pid);
	FILE* fp = fopen(buffer, "r");
	if (fp) {
		size_t size = fread(buffer, sizeof (char), sizeof (buffer), fp);
		if (size > 0) {
			// See: http://man7.org/linux/man-pages/man5/proc.5.html section /proc/[pid]/stat
			strtok(buffer, " "); // (1) pid  %d
			strtok(NULL, " "); // (2) comm  %s
			strtok(NULL, " "); // (3) state  %c
			char * s_ppid = strtok(NULL, " "); // (4) ppid  %d
			*ppid = atoi(s_ppid);
		}
		fclose(fp);
	}
}

void print_call_stack(FILE * fp)
{
    fprintf(fp, "***call stack***\n\n");
    pid_t pid = getpid();
    while (pid != 0) {
        char name[BUFSIZ];

        char* cmdline = name;
        get_process_cmdline(pid, cmdline);
        fprintf(fp, "%6d - %s\n", pid, cmdline);

        get_process_parent_id(pid, & pid);
    }
}

void print_log(int argc, char *argv[])
{
    FILE * fp;

    fp = stdout;
    fp = fopen(LOG_FILE, "a");

    //**************************************************************
    fprintf(fp, "\e[93m"); // Light yellow color
    fprintf(fp, "***bash wrapper***\n\n");
    fprintf(fp, "\e[39m\n"); // default color

    //**************************************************************
    char cwd[BUFSIZ];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(fp, "Current working dir: %s\n", cwd);
    else
        perror("getcwd() error");

    //**************************************************************
    fprintf(fp, "print all args:\n\n");
    // print all args
    int i = 0;
    for (i = 0; i < argc; i++)
        fprintf(fp, "%s\n", argv[i]);

    fprintf(fp, "***   ***\n\n");

    //**************************************************************
    print_call_stack(fp);

    //**************************************************************
    fprintf(fp, "\e[93m"); // Light yellow color
    fprintf(fp, "***   ***\n\n");
    fprintf(fp, "\e[39m\n"); // default color

    if( fp != stdout)
        fclose(fp);
}

int main(int argc, char *argv[])
{
    print_log(argc, argv);

    // call original bash
    execv("/bin/bash_orig", argv);

    return 0;
}
